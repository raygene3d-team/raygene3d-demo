#include "render_ui_broker.h"

#include <imgui/imgui.h>

namespace RayGene3D
{
  const uint32_t RenderUIBroker::sub_limit;
  const uint32_t RenderUIBroker::arg_limit;

  void RenderUIBroker::Initialize()
  {
  }

  void RenderUIBroker::Use()
  {
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2(float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()));

    const auto now = std::chrono::high_resolution_clock::now();
    const auto delta = std::chrono::duration_cast<std::chrono::duration<double>>(now - time);
    io.DeltaTime = delta.count();
    time = now;

    // Start the frame
    ImGui::NewFrame();
    if (show_test_window)
    {
      ImGui::ShowDemoWindow(&show_test_window);
      pass->SetEnabled(show_test_window);
    }

    {
      ImGui::Render();

      auto vtx_mapped = vtx_resource->Map();
      auto idx_mapped = idx_resource->Map();
      auto arg_mapped = arg_resource->Map();

      memset(arg_mapped, 0, sizeof(Batch::Graphic) * arg_limit * sub_limit);

      ImDrawData* draw_data = ImGui::GetDrawData();
      const auto pass_count = std::min(sub_limit, uint32_t(draw_data->CmdListsCount));
      for (uint32_t i = 0; i < pass_count; ++i)
      {
        const ImDrawList* cmd_list = draw_data->CmdLists[i];

        const auto vtx_stride = static_cast<uint32_t>(sizeof(ImDrawVert));
        const auto vtx_count = static_cast<uint32_t>(cmd_list->VtxBuffer.Size);
        const auto vtx_data = cmd_list->VtxBuffer.Data;

        const auto idx_stride = static_cast<uint32_t>(sizeof(ImDrawIdx));
        const auto idx_count = static_cast<uint32_t>(cmd_list->IdxBuffer.Size);
        const auto idx_data = cmd_list->IdxBuffer.Data;

        if (vtx_count > vtx_resource->GetMipmapsOrCount() || idx_count > idx_resource->GetMipmapsOrCount())
          continue;

        auto vtx_aligned = &reinterpret_cast<ImDrawVert*>(vtx_mapped)[i * vtx_limit];
        memcpy(vtx_aligned, vtx_data, vtx_stride * vtx_count);
        
        auto idx_aligned = &reinterpret_cast<ImDrawIdx*>(idx_mapped)[i * idx_limit];
        memcpy(idx_aligned, idx_data, idx_stride * idx_count);

        const auto arg_count = std::min(arg_limit, uint32_t(cmd_list->CmdBuffer.Size));
        for (uint32_t j = 0; j < arg_count; ++j)
        {
          const auto& draw_data = cmd_list->CmdBuffer[j];
          auto& graphic_arg = reinterpret_cast<Batch::Graphic*>(arg_mapped)[i * arg_limit + j];
          graphic_arg = { draw_data.ElemCount, 1u, draw_data.IdxOffset, draw_data.VtxOffset, 0u };
        }
      }

      vtx_resource->Unmap();
      idx_resource->Unmap();
      arg_resource->Unmap();
    }
  }


  void RenderUIBroker::Discard()
  {
  }

  void RenderUIBroker::OnCursor(double xpos, double ypos)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)xpos, (float)ypos);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
  }

  void RenderUIBroker::OnKeyboard(int key, int scancode, int action, int mods)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = action == 1 ? true : false;
  }

  void RenderUIBroker::OnMouse(int button, int action, int mods)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[button] = action == 1 ? true : false;
  }

  void RenderUIBroker::OnScroll(double xoffset, double yoffset)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += (float)yoffset;
  }

  void RenderUIBroker::OnChar(unsigned int glyph)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(glyph);
  }

  RenderUIBroker::RenderUIBroker(Wrap& wrap)
    : Broker("imgui_broker", wrap)
  {
    const auto find_view_fn = [this](const std::shared_ptr<View>& view)
    {
      if (view->GetName().compare("backbuffer_ua_view") == 0)
      {
        this->backbuffer_uav = view;
      }

      if (view->GetName().compare("backbuffer_rt_view") == 0)
      {
        this->backbuffer_rtv = view;
      }
    };
    wrap.GetCore()->VisitView(find_view_fn);

    const auto tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_camera = tree->GetObjectItem("camera_property");
    {
      prop_extent_x = prop_camera->GetObjectItem("extent_x");
      prop_extent_y = prop_camera->GetObjectItem("extent_y");
    }

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    auto* device = wrap.GetCore()->GetDevice().get();


    {
      const float L = 0.0f;
      const float R = static_cast<float>(prop_extent_x->GetUint());
      const float B = static_cast<float>(prop_extent_y->GetUint());
      const float T = 0.0f;
      const float mvp[4][4] = {
        { 2.0f / (R - L),     0.0f,               0.0f,       0.0f },
        { 0.0f,               2.0f / (T - B),     0.0f,       0.0f },
        { 0.0f,               0.0f,               0.5f,       0.0f },
        { (R + L) / (L - R),  (T + B) / (B - T),  0.5f,       1.0f },
      };
      proj_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
      proj_property->RawAllocate(uint32_t(sizeof(mvp)));
      proj_property->SetRawBytes({ &mvp[0][0], uint32_t(sizeof(mvp)) }, 0);
      std::pair<const void*, uint32_t> interops[] =
      {
        proj_property->GetRawBytes(0)
      };

      proj_resource = device->CreateResource("imgui_proj_resource",
        Resource::BufferDesc
        {
          Usage(USAGE_CONSTANT_DATA),
          sizeof(float),
          16,
        },
        Resource::HINT_UNKNOWN,
        { interops, uint32_t(std::size(interops)) }
      );
    }

    {
      ImGui::GetIO().Fonts->AddFontFromFileTTF("3rdparty/imgui/fonts/Roboto-Medium.ttf", 20.0f);

      unsigned char* font_data{ nullptr };
      int font_stride{ 4 };
      int font_size_x{ 0 };
      int font_size_y{ 0 };
      ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&font_data, &font_size_x, &font_size_y);

      font_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
      font_property->RawAllocate(font_stride * font_size_x * font_size_y);
      font_property->SetRawBytes({ font_data, font_stride * font_size_x * font_size_y }, 0);
      std::pair<const void*, uint32_t> interops[] =
      {
        font_property->GetRawBytes(0)
      };

      font_resource = device->CreateResource("imgui_font_resource",
        Resource::Tex2DDesc
        {
          Usage(USAGE_SHADER_RESOURCE),
          1,
          1,
          FORMAT_R8G8B8A8_UNORM,
          uint32_t(font_size_x),
          uint32_t(font_size_y),
        },
        Resource::HINT_UNKNOWN,
        { interops, uint32_t(std::size(interops)) }
      );
    }

    {
      vtx_resource = device->CreateResource("imgui_vtx_resource",
        Resource::BufferDesc
        {
          Usage(USAGE_VERTEX_ARRAY),
          uint32_t(sizeof(ImDrawVert)),
          vtx_limit * sub_limit,
        },
        Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );

      idx_resource = device->CreateResource("imgui_idx_resource",
        Resource::BufferDesc
        {
          Usage(USAGE_INDEX_ARRAY),
          uint32_t(sizeof(ImDrawIdx)),
          idx_limit * sub_limit,
        },
        Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );

      arg_resource = device->CreateResource("imgui_arg_resource",
        Resource::BufferDesc
        {
          Usage(USAGE_ARGUMENT_LIST),
          uint32_t(sizeof(Batch::Graphic)),
          arg_limit * sub_limit,
        },
        Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );
    }

    {
      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();
      const auto extent_z = 1u;

      const Pass::RTAttachment rt_attachments[] = {
         backbuffer_rtv, std::nullopt,
      };

      pass = device->CreatePass("imgui_pass",
        Pass::TYPE_GRAPHIC,
        { rt_attachments, uint32_t(std::size(rt_attachments)) },
        {}
      );
    }


    {
      const std::string debug_source =
        "\
      #ifdef USE_SPIRV\n \
      #define VK_BINDING(x) [[vk::binding(x)]]\n \
      #define VK_LOCATION(x) [[vk::location(x)]]\n \
      #else\n \
      #define VK_BINDING(x)\n \
      #define VK_LOCATION(x)\n \
      #endif\n \
      \
      struct VSInput\
      {\
        VK_LOCATION(0) uint vertID : SV_VertexID;\
      };\
      \
      struct VSOutput\
      {\
        VK_LOCATION(0) float4 col : COLOR0;\
        VK_LOCATION(1) float4 pos : SV_Position;\
      };\
      \
      VSOutput vs_main(VSInput input)\
      {\
        float2 positions[3] = {\
        float2( 0.0,-0.5),\
        float2( 0.5, 0.5),\
        float2(-0.5, 0.5)\
        };\
        float3 colors[3] = {\
        float3(1.0, 0.0, 0.0),\
        float3(0.0, 1.0, 0.0),\
        float3(0.0, 0.0, 1.0)\
        };\
        VSOutput output;\
        output.pos = float4(positions[input.vertID], 0.5, 1.0);\
        output.col = float4(colors[input.vertID], 1.0);\
        return output;\
      }\
      struct PSInput\
      {\
        VK_LOCATION(0) float4 col : COLOR0;\
      };\
      \
      struct PSOutput\
      {\
        float4 col : SV_Target;\
      };\
      PSOutput ps_main(PSInput input)\
      {\
        PSOutput output;\
        output.col = float4(input.col.rgb, 1.0);\
        return output;\
      }";

      const std::string source =
        "\
      #ifdef USE_SPIRV\n \
      #define VK_BINDING(x) [[vk::binding(x)]]\n \
      #define VK_LOCATION(x) [[vk::location(x)]]\n \
      #else\n \
      #define VK_BINDING(x)\n \
      #define VK_LOCATION(x)\n \
      #endif\n \
      \
      VK_BINDING(1) cbuffer constant0 : register(b0) \
      {\
        float4x4 ProjectionMatrix; \
      };\
      VK_BINDING(0) sampler sampler0 : register(s0);\
      VK_BINDING(2) Texture2D texture0 : register(t0);\
      struct VSInput\
      {\
        VK_LOCATION(0) float2 pos : register0;\
        VK_LOCATION(1) float2 uv  : register1;\
        VK_LOCATION(2) float4 col : register2;\
      };\
      \
      struct VSOutput\
      {\
        VK_LOCATION(0) float4 col : COLOR0;\
        VK_LOCATION(1) float2 uv  : TEXCOORD0;\
        VK_LOCATION(2) float4 pos : SV_Position;\
      };\
      \
      VSOutput vs_main(VSInput input)\
      {\
        VSOutput output;\
        output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.0f, 1.0f));\
        output.col = input.col;\
        output.uv  = input.uv;\
        return output;\
      }\
      \
      struct PSInput\
      {\
        VK_LOCATION(0) float4 col : COLOR0;\
        VK_LOCATION(1) float2 uv  : TEXCOORD0;\
      };\
      \
      struct PSOutput\
      {\
        float4 col : SV_Target;\
      };\
      PSOutput ps_main(PSInput input)\
      {\
        PSOutput output;\
        output.col = input.col * texture0.Sample(sampler0, input.uv); \
        return output; \
      }";

      const Technique::IAState ia_technique =
      {
        Technique::TOPOLOGY_TRIANGLELIST,
        Technique::INDEXER_16_BIT,
        {
          { 0, 0, 20, FORMAT_R32G32_FLOAT, false },
          { 0, 8, 20, FORMAT_R32G32_FLOAT, false },
          { 0,16, 20, FORMAT_R8G8B8A8_UNORM, false },
        }
      };

      const Technique::RCState rc_technique =
      {
        Technique::FILL_SOLID,
        Technique::CULL_NONE,
        {
          { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
        },
      };

      const Technique::DSState ds_technique =
      {
        false,
        false,
        Technique::COMPARISON_ALWAYS
      };

      const Technique::OMState om_technique =
      {
        false,
        {
          { true, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF }
        }
      };

      technique = pass->CreateTechnique("imgui_config",
        source,
        Technique::Compilation(Technique::COMPILATION_VS | Technique::COMPILATION_PS),
        {},
        ia_technique,
        rc_technique,
        ds_technique,
        om_technique
      );
    }


    {

      Batch::Entity entities[sub_limit * arg_limit];
      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        const auto vtx_view = vtx_resource->CreateView("vtx_view_" + std::to_string(i),
          Usage(USAGE_VERTEX_ARRAY),
          { i * uint32_t(sizeof(ImDrawVert)) * vtx_limit, uint32_t(sizeof(ImDrawVert)) * vtx_limit }
        );

        const auto idx_view = idx_resource->CreateView("idx_view_" + std::to_string(i),
          Usage(USAGE_INDEX_ARRAY),
          { i * uint32_t(sizeof(ImDrawIdx)) * idx_limit, uint32_t(sizeof(ImDrawIdx)) * idx_limit }
        );

        for (uint32_t j = 0; j < arg_limit; ++j)
        {
          const auto argument_view = arg_resource->CreateView("arg_ci_view_" + std::to_string(j),
            Usage(USAGE_ARGUMENT_LIST),
            { (i * arg_limit + j) * uint32_t(sizeof(Batch::Graphic)), uint32_t(sizeof(Batch::Graphic)) }
          );
          entities[i * arg_limit + j] = { {vtx_view}, {idx_view}, argument_view };
        }
      }

      const Batch::Sampler samplers[] = {
        { Batch::Sampler::FILTERING_LINEAR, 0, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_ALWAYS, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
      };

      auto proj_view = proj_resource->CreateView("imgui_proj_view",
        Usage(USAGE_CONSTANT_DATA)
      );
      const std::shared_ptr<View> ub_views[] = {
          proj_view,
      };

      auto font_view = font_resource->CreateView("imgui_font_view",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> ri_views[] = {
          font_view,
      };

      batch = technique->CreateBatch("imgui_layout",
        { entities, uint32_t(std::size(entities)) },
        { samplers, uint32_t(std::size(samplers)) },
        { ub_views, uint32_t(std::size(ub_views)) },
        {},
        { ri_views, uint32_t(std::size(ri_views)) },
        {},
        {},
        {}
      );
    }

    time = std::chrono::high_resolution_clock::now();

    ImGuiIO& io = ImGui::GetIO(); // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Escape] = 256; // GLFW_KEY_ESCAPE             
    io.KeyMap[ImGuiKey_Enter] = 257; // GLFW_KEY_ENTER              
    io.KeyMap[ImGuiKey_Tab] = 258; // GLFW_KEY_TAB                
    io.KeyMap[ImGuiKey_Backspace] = 259; // GLFW_KEY_BACKSPACE          
    io.KeyMap[ImGuiKey_Insert] = 260; // GLFW_KEY_INSERT             
    io.KeyMap[ImGuiKey_Delete] = 261; // GLFW_KEY_DELETE             
    io.KeyMap[ImGuiKey_RightArrow] = 262; // GLFW_KEY_RIGHT              
    io.KeyMap[ImGuiKey_LeftArrow] = 263; // GLFW_KEY_LEFT               
    io.KeyMap[ImGuiKey_DownArrow] = 264; // GLFW_KEY_DOWN               
    io.KeyMap[ImGuiKey_UpArrow] = 265; // GLFW_KEY_UP                 
    io.KeyMap[ImGuiKey_PageUp] = 266; // GLFW_KEY_PAGE_UP            
    io.KeyMap[ImGuiKey_PageDown] = 267; // GLFW_KEY_PAGE_DOWN          
    io.KeyMap[ImGuiKey_Home] = 268; // GLFW_KEY_HOME               
    io.KeyMap[ImGuiKey_End] = 269; // GLFW_KEY_END                             

    io.RenderDrawListsFn = nullptr;  // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    io.ImeWindowHandle = nullptr;
  }

  RenderUIBroker::~RenderUIBroker()
  {
    technique->DestroyBatch(batch);
    batch.reset();

    pass->DestroyTechnique(technique);
    technique.reset();

    wrap.GetCore()->GetDevice()->DestroyPass(pass);
    pass.reset();

    wrap.GetCore()->GetDevice()->DestroyResource(proj_resource);
    proj_resource.reset();

    wrap.GetCore()->GetDevice()->DestroyResource(font_resource);
    font_resource.reset();

    wrap.GetCore()->GetDevice()->DestroyResource(arg_resource);
    arg_resource.reset();

    wrap.GetCore()->GetDevice()->DestroyResource(idx_resource);
    idx_resource.reset();

    wrap.GetCore()->GetDevice()->DestroyResource(vtx_resource);
    vtx_resource.reset();
  }
}
