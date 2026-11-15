#include "render_ui_broker.h"

#include <imgui/imgui.h>

#include <SDL3/SDL_scancode.h>

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

        if (vtx_count > vtx_resource->GetLevelsOrLength() || idx_count > idx_resource->GetLevelsOrLength())
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

  void RenderUIBroker::OnKeyboard(int scancode, bool down)
  {
    ImGuiIO& io = ImGui::GetIO();
    if (scancode >= 0 && scancode < IM_ARRAYSIZE(io.KeysDown))
    {
      io.KeysDown[scancode] = down;
    }
  }

  void RenderUIBroker::OnMouse(int button, bool down)
  {
    ImGuiIO& io = ImGui::GetIO();
    if (button >= 0 && button < IM_ARRAYSIZE(io.MouseDown))
    {
      io.MouseDown[button] = down;
    }
  }

  void RenderUIBroker::OnScroll(double xoffset, double yoffset)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += (float)yoffset;
  }

  void RenderUIBroker::OnChar(const char* text)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharactersUTF8(text);
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

    const auto& tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_camera = tree->GetObjectItem("camera");
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
      const auto mvp = glm::f32mat4x4{
        { 2.0f / (R - L),     0.0f,               0.0f,       0.0f },
        { 0.0f,               2.0f / (T - B),     0.0f,       0.0f },
        { 0.0f,               0.0f,               0.5f,       0.0f },
        { (R + L) / (L - R),  (T + B) / (B - T),  0.5f,       1.0f },
      };

      proj_resource = device->CreateResource("imgui_proj_resource",
        Resource::BufferDesc
        {
          Usage(USAGE_CONSTANT_DATA),
          sizeof(glm::f32mat4x4),
          1u,
        },
        Resource::HINT_UNKNOWN,
        { reinterpret_cast<const uint8_t*>(&mvp), sizeof(glm::f32mat4x4) }
      );
    }

    {
      ImGui::GetIO().Fonts->AddFontFromFileTTF("3rdparty/imgui/fonts/Roboto-Medium.ttf", 20.0f);

      unsigned char* font_data{ nullptr };
      int font_stride{ 4 };
      int font_size_x{ 0 };
      int font_size_y{ 0 };
      ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&font_data, &font_size_x, &font_size_y);

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
        { font_data, font_stride * font_size_x * font_size_y }
      );
    }

    {
      vtx_resource = device->CreateResource("imgui_vtx_resource",
        Resource::BufferDesc
        {
          Usage(USAGE_VERTEX_ARRAY),
          sizeof(ImDrawVert),
          vtx_limit * sub_limit,
        },
        Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );

      idx_resource = device->CreateResource("imgui_idx_resource",
        Resource::BufferDesc
        {
          Usage(USAGE_INDEX_ARRAY),
          sizeof(ImDrawIdx),
          idx_limit * sub_limit,
        },
        Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );

      arg_resource = device->CreateResource("imgui_arg_resource",
        Resource::BufferDesc
        {
          Usage(USAGE_ARGUMENT_LIST),
          sizeof(Batch::Graphic),
          arg_limit * sub_limit,
        },
        Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );
    }

    {
      const auto size_x = prop_extent_x->GetUint();
      const auto size_y = prop_extent_y->GetUint();
      const auto layers = 1u;

      const Pass::RTAttachment rt_attachments[] = {
         backbuffer_rtv, std::nullopt,
      };

      pass = device->CreatePass("imgui_pass",
        Pass::TYPE_GRAPHIC,
        size_x,
        size_y,
        layers,
        { rt_attachments, std::size(rt_attachments) },
        {}
      );
    }


    {
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

      const Config::IAState ia_state =
      {
        Config::TOPOLOGY_TRIANGLELIST,
        Config::INDEXER_16_BIT,
        {
          { 0, 0, 20, FORMAT_R32G32_FLOAT, false },
          { 0, 8, 20, FORMAT_R32G32_FLOAT, false },
          { 0,16, 20, FORMAT_R8G8B8A8_UNORM, false },
        }
      };

      const Config::RCState rc_state =
      {
        Config::FILL_SOLID,
        Config::CULL_NONE,
        {
          { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
        },
      };

      const Config::DSState ds_state =
      {
        false,
        false,
        Config::COMPARISON_ALWAYS
      };

      const Config::OMState om_state =
      {
        false,
        {
          { true, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF }
        }
      };

      config = pass->CreateConfig("imgui_config",
        source,
        Config::Compilation(Config::COMPILATION_VERT | Config::COMPILATION_FRAG),
        {},
        ia_state,
        rc_state,
        ds_state,
        om_state
      );
    }


    {
      std::vector<Batch::Entity> entities(sub_limit * arg_limit);
      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        const auto vtx_view = vtx_resource->CreateView("vtx_view_" + std::to_string(i),
          Usage(USAGE_VERTEX_ARRAY),
          { i * sizeof(ImDrawVert) * vtx_limit, sizeof(ImDrawVert) * vtx_limit }
        );

        const auto idx_view = idx_resource->CreateView("idx_view_" + std::to_string(i),
          Usage(USAGE_INDEX_ARRAY),
          { i * sizeof(ImDrawIdx) * idx_limit, sizeof(ImDrawIdx) * idx_limit }
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
        { Batch::Sampler::FILTERING_LINEAR, 0, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
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

      batch = config->CreateBatch("imgui_layout",
        { entities.data(), entities.size()},
        { samplers, std::size(samplers) },
        { ub_views, std::size(ub_views) },
        {},
        { ri_views, std::size(ri_views) },
        {},
        {},
        {}
      );
    }

    time = std::chrono::high_resolution_clock::now();

    ImGuiIO& io = ImGui::GetIO(); // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE; // SDL_SCANCODE_ESCAPE             
    io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN; // SDL_SCANCODE_RETURN              
    io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB; // SDL_SCANCODE_TAB                
    io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE; // SDL_SCANCODE_BACKSPACE          
    io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT; // SDL_SCANCODE_INSERT             
    io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE; // SDL_SCANCODE_DELETE             
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT; // SDL_SCANCODE_RIGHT              
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT; // SDL_SCANCODE_LEFT               
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN; // SDL_SCANCODE_DOWN               
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP; // SDL_SCANCODE_UP                 
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP; // SDL_SCANCODE_PAGEUP            
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN; // SDL_SCANCODE_PAGEDOWN          
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME; // SDL_SCANCODE_HOME               
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END; // SDL_SCANCODE_END                             

    io.RenderDrawListsFn = nullptr;  // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    io.ImeWindowHandle = nullptr;
  }

  RenderUIBroker::~RenderUIBroker()
  {
    config->DestroyBatch(batch);
    batch.reset();

    pass->DestroyConfig(config);
    config.reset();

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
