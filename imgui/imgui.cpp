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

    prop_extent_x = prop_camera->GetObjectItem("extent_x");
    prop_extent_y = prop_camera->GetObjectItem("extent_y");

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    auto* device = root.GetCore()->GetDevice().get();


    {
      auto proj_resource = device->CreateResource("imgui_proj_resource");
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

        uint32_t constants_stride{ 4 };
        uint32_t constants_count{ 16 };

        proj_resource->SetType(Resource::TYPE_BUFFER);
        proj_resource->SetStride(constants_stride);
        proj_resource->SetCount(constants_count);
        proj_resource->SetInteropCount(1);
        proj_resource->SetInteropItem(0, proj_property->GetRawBytes(0));
      }
      this->proj_resource = proj_resource;
    }

    {
      auto font_resource = device->CreateResource("imgui_font_resource");
      {
        ImGui::GetIO().Fonts->AddFontFromFileTTF("3rdparty/imgui/fonts/Roboto-Medium.ttf", 20.0f);

        unsigned char* font_data{ nullptr };
        int font_stride{ 4 };
        int font_width{ 0 };
        int font_height{ 0 };
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&font_data, &font_width, &font_height);

        font_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
        font_property->RawAllocate(font_stride * font_width * font_height);
        font_property->SetRawBytes({ font_data, font_stride * font_width * font_height }, 0);
        font_resource->SetInteropCount(1);

        font_resource->SetType(Resource::TYPE_IMAGE2D);
        font_resource->SetExtentX(font_width);
        font_resource->SetExtentY(font_height);
        font_resource->SetLayers(1);
        font_resource->SetMipmaps(1);
        font_resource->SetFormat(FORMAT_R8G8B8A8_UNORM);
        font_resource->SetInteropItem(0, font_property->GetRawBytes(0));
      }
      this->font_resource = font_resource;
    }

    {
      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        auto vtx_resource = device->CreateResource("imgui_vtx_resource_" + std::to_string(i));
        vtx_resource->SetType(Resource::TYPE_BUFFER);
        vtx_resource->SetStride(uint32_t(sizeof(ImDrawVert)));
        vtx_resource->SetCount(vtx_limit);
        vtx_resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);

        auto idx_resource = device->CreateResource("imgui_idx_resource_" + std::to_string(i));
        idx_resource->SetType(Resource::TYPE_BUFFER);
        idx_resource->SetStride(uint32_t(sizeof(ImDrawIdx)));
        idx_resource->SetCount(10000);
        idx_resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);

        auto arg_resource = device->CreateResource("imgui_arg_resource_" + std::to_string(i));
        arg_resource->SetType(Resource::TYPE_BUFFER);
        arg_resource->SetStride(uint32_t(sizeof(Pass::Argument)));
        arg_resource->SetCount(10);
        arg_resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);

        vtx_resources[i] = vtx_resource;
        idx_resources[i] = idx_resource;
        arg_resources[i] = arg_resource;
      }
    }


    {
      auto config = device->CreateConfig("imgui_config");
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
        config->SetSource(source);
        config->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));

        const Config::Attribute attributes[] = {
          { 0, 0, 20, FORMAT_R32G32_FLOAT, false },
          { 0, 8, 20, FORMAT_R32G32_FLOAT, false },
          { 0,16, 20, FORMAT_R8G8B8A8_UNORM, false },
        };
        config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

        config->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
        config->SetIndexer(Config::INDEXER_16_BIT);

        config->SetFillMode(Config::FILL_SOLID);
        config->SetCullMode(Config::CULL_NONE);
        config->SetClipEnabled(false);

        const Config::Blend blends[] = {
          { true, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
        };
        config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
        config->SetATCEnabled(false);

        config->SetDepthEnabled(false);
        config->SetDepthWrite(false);
        config->SetDepthComparison(Config::COMPARISON_ALWAYS);

        const Config::Viewport viewports[] = {
          { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f },
        };
        config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
      }
      this->config = config;
    }


    {
      auto proj_view = proj_resource->CreateView("imgui_proj_view");
      {
        proj_view->SetBind(View::BIND_CONSTANT_DATA);
      }

      auto font_view = font_resource->CreateView("imgui_font_view");
      {
        font_view->SetBind(View::BIND_SHADER_RESOURCE);
      }

      auto layout = device->CreateLayout("imgui_layout");
      {
        const std::shared_ptr<View> ub_views[] = {
          proj_view,
        };
        layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

        const std::shared_ptr<View> ri_views[] = {
          font_view,
        };
        layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

        const Layout::Sampler samplers[] = {
           { Layout::Sampler::FILTERING_LINEAR, 0, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_ALWAYS, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
        };
        layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });
      }
      this->layout = layout;
    }


    {
      auto pass = device->CreatePass("imgui_pass");
      {
        pass->SetType(Pass::TYPE_GRAPHIC);
        pass->SetEnabled(true);

        const std::shared_ptr<View> rt_views[] = {
          backbuffer_rtv,
        };
        pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

        //const std::shared_ptr<View> ds_views[] = {
        //  depth_view,
        //};
        //pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });


        const Pass::RTValue rt_values[] = {
          {},
        };
        pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

        //const Pass::DSValue ds_values[] = {
        //  { 1.0f, std::nullopt },
        //};
        //pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });


        pass->SetSubpassCount(sub_limit);
        for (uint32_t i = 0; i < sub_limit; ++i)
        {
          auto vtx_view = vtx_resources[i]->CreateView("vtx_view_" + std::to_string(i));
          vtx_view->SetBind(View::BIND_VERTEX_ARRAY);
          vtx_view->SetByteOffset(0);

          const std::shared_ptr<View> va_views[] = {
            vtx_view,
          };
          pass->UpdateSubpassVAViews(i, { va_views, uint32_t(std::size(va_views)) });

          auto idx_view = idx_resources[i]->CreateView("idx_view_" + std::to_string(i));
          idx_view->SetBind(View::BIND_INDEX_ARRAY);
          idx_view->SetByteOffset(0);
          idx_view->SetByteCount(2);

          const std::shared_ptr<View> ia_views[] = {
            idx_view,
          };
          pass->UpdateSubpassIAViews(i, { ia_views, uint32_t(std::size(ia_views)) });

          std::array<Pass::Command, arg_limit> commands;
          for (uint32_t j = 0; j < arg_limit; ++j)
          {
            const auto argument_view = arg_resources[i]->CreateView("arg_ci_view_" + std::to_string(j));
            argument_view->SetBind(View::BIND_COMMAND_INDIRECT);
            argument_view->SetByteOffset(j * sizeof(Pass::Argument));
            argument_view->SetByteCount(sizeof(Pass::Argument));

            commands[j].view = argument_view;
          }
          pass->UpdateSubpassCommands(i, { commands.data(), uint32_t(commands.size()) });

          pass->SetSubpassLayout(i, layout);
          pass->SetSubpassConfig(i, config);
        }
      }
      this->pass = pass;
    }

    {
      proj_resource->Initialize();
      font_resource->Initialize();

      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        vtx_resources[i]->Initialize();
        idx_resources[i]->Initialize();
        arg_resources[i]->Initialize();
      }

      config->Initialize();
      layout->Initialize();
      pass->Initialize();
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
