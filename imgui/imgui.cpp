#include "imgui.h"
//#include "../core/device/device.h"


#include <imgui/imgui.h>

namespace RayGene3D
{
  const uint32_t Imgui::sub_limit;
  const uint32_t Imgui::arg_limit;

  void Imgui::Initialize()
  {
    Discard();

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
    root.GetCore()->VisitView(find_view_fn);

    const auto tree = root.GetData()->GetStorage()->GetTree();

    prop_camera = tree->GetObjectItem("camera_property");
    {
      prop_extent_x = prop_camera->GetObjectItem("extent_x");
      prop_extent_y = prop_camera->GetObjectItem("extent_y");
    }

    //prop_extent_x = prop_camera->GetObjectItem("extent_x");
    //prop_extent_y = prop_camera->GetObjectItem("extent_y");

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    auto* device = root.GetCore()->GetDevice().get();


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
      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        vtx_resources[i] = device->CreateResource("imgui_vtx_resource_" + std::to_string(i),
          Resource::BufferDesc
          {
            Usage(USAGE_VERTEX_ARRAY),
            uint32_t(sizeof(ImDrawVert)),
            vtx_limit,
          },
          Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
        );

        idx_resources[i] = device->CreateResource("imgui_idx_resource_" + std::to_string(i),
          Resource::BufferDesc
          {
            Usage(USAGE_INDEX_ARRAY),
            uint32_t(sizeof(ImDrawIdx)),
            idx_limit,
          },
          Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
        );

        arg_resources[i] = device->CreateResource("imgui_arg_resource_" + std::to_string(i),
          Resource::BufferDesc
          {
            Usage(USAGE_COMMAND_INDIRECT),
            uint32_t(sizeof(Pass::Argument)),
            arg_limit,
          },
          Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
        );
      }
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
          { true, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
        }
      };

      config = device->CreateConfig("imgui_config",
        source,
        Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
        {},
        ia_state,
        rc_state,
        ds_state,
        om_state
      );
    }


    {
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

      const Layout::Sampler samplers[] = {
        { Layout::Sampler::FILTERING_LINEAR, 0, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_ALWAYS, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
      };

      layout = device->CreateLayout("imgui_layout",
        { ub_views, uint32_t(std::size(ub_views)) },
        {},
        { ri_views, uint32_t(std::size(ri_views)) },
        {},
        {},
        {},
        { samplers, uint32_t(std::size(samplers)) },
        {}
      );
    }


    {
      Pass::Subpass subpasses[sub_limit];
      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        const auto vtx_view = vtx_resources[i]->CreateView("vtx_view_" + std::to_string(i),
          Usage(USAGE_VERTEX_ARRAY)
        );
        const std::shared_ptr<View> va_views[] = {
          vtx_view,
        };

        const auto idx_view = idx_resources[i]->CreateView("idx_view_" + std::to_string(i),
          Usage(USAGE_INDEX_ARRAY)
        );
        const std::shared_ptr<View> ia_views[] = {
          idx_view,
        };

        Pass::Command commands[arg_limit];
        for (uint32_t j = 0; j < arg_limit; ++j)
        {
          const auto argument_view = arg_resources[i]->CreateView("arg_ci_view_" + std::to_string(j),
            Usage(USAGE_COMMAND_INDIRECT),
            { j * uint32_t(sizeof(Pass::Argument)), uint32_t(sizeof(Pass::Argument)) }
          );
          commands[j].view = argument_view;
        }

        subpasses[i] =
        {
          config, layout,
          { commands, commands + std::size(commands) },
          { va_views, va_views + std::size(va_views) },
          { ia_views, ia_views + std::size(ia_views) }
        };
      }

      const Pass::RTAttachment rt_attachments[] = {
        backbuffer_rtv, std::nullopt,
      };

      pass = device->CreatePass("imgui_pass",
        Pass::TYPE_GRAPHIC,
        { subpasses, uint32_t(std::size(subpasses)) },
        { rt_attachments, uint32_t(std::size(rt_attachments)) },
        {}
      );
    }


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

  void Imgui::Use()
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
    }

    {
      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        auto& arg_resource = arg_resources[i];

        auto arg_mapped = arg_resource->Map();
        for (uint32_t j = 0; j < arg_limit; ++j)
        {
          auto& graphic_arg = reinterpret_cast<Pass::Argument*>(arg_mapped)[j];
          graphic_arg = { 0, 0, 0, 0, 0 };
        }
        arg_resource->Unmap();
      }
    }


    {
      ImGui::Render();

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

        auto& vtx_resource = vtx_resources[i];
        auto& idx_resource = idx_resources[i];
        auto& arg_resource = arg_resources[i];

        if (vtx_count > vtx_resource->GetCount() || idx_count > idx_resource->GetCount())
          continue;

        auto vtx_mapped = vtx_resource->Map();
        memcpy(vtx_mapped, vtx_data, vtx_stride * vtx_count);
        vtx_resource->Unmap();

        auto idx_mapped = idx_resource->Map();
        memcpy(idx_mapped, idx_data, idx_stride * idx_count);
        idx_resource->Unmap();


        const auto arg_count = std::min(arg_limit, uint32_t(cmd_list->CmdBuffer.Size));

        auto arg_mapped = arg_resource->Map();
        for (uint32_t j = 0; j < arg_count; ++j)
        {
          const auto& draw_data = cmd_list->CmdBuffer[j];
          auto& graphic_arg = reinterpret_cast<Pass::Argument*>(arg_mapped)[j];
          graphic_arg = { draw_data.ElemCount, 1, draw_data.IdxOffset, draw_data.VtxOffset, 0 };
        }
        arg_resource->Unmap();
      }
    }
  }


  void Imgui::Discard()
  {
  }

  void Imgui::OnCursor(double xpos, double ypos)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)xpos, (float)ypos);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
  }

  void Imgui::OnKeyboard(int key, int scancode, int action, int mods)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = action == 1 ? true : false;
  }

  void Imgui::OnMouse(int button, int action, int mods)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[button] = action == 1 ? true : false;
  }

  void Imgui::OnScroll(double xoffset, double yoffset)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += (float)yoffset;
  }

  void Imgui::OnChar(unsigned int glyph)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(glyph);
  }

  Imgui::Imgui(Root& root)
    : Broker("imgui_broker", root)
  {
    time = std::chrono::high_resolution_clock::now();
  }

  Imgui::~Imgui()
  {
  }
}
