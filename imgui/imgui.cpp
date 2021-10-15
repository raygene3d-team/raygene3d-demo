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

    const auto core = &this->GetCore();
    const auto device = core->AccessDevice();

    const auto extent_x = property->GetObjectItem("camera")->GetObjectItem("extent_x")->GetUint();
    const auto extent_y = property->GetObjectItem("camera")->GetObjectItem("extent_y")->GetUint();


    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    frame_output = device->ShareResource("screen_backbuffer");
    {
      //frame_output->SetType(Resource::TYPE_TEXTURE2D);
      //frame_output->SetExtentX(extent_x);
      //frame_output->SetExtentY(extent_y);
      //frame_output->SetLayers(1);
      //frame_output->SetMipmaps(1);
      //frame_output->SetFormat(FORMAT_B8G8R8A8_UNORM);
    }

    auto output_view = frame_output->CreateView("output_view");
    {
      output_view->SetBind(View::BIND_RENDER_TARGET);
      //output_view->SetLayerOffset(0);
      //output_view->SetRange(uint32_t(-1));
    }

#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#else
#define VK_BINDING(x) /*[vk::binding(x)]]*/
#endif




    auto shader = device->CreateShader("imgui_shader");
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
      shader->SetSource(source);
      shader->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));

      const Config::Attribute attributes[] = {
        { 0, 0, 20, FORMAT_R32G32_FLOAT, false },
        { 0, 8, 20, FORMAT_R32G32_FLOAT, false },
        { 0,16, 20, FORMAT_R8G8B8A8_UNORM, false },
      };
      shader->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

      //const uint32_t strides[] = {
      //  20,
      //};
      //shader->UpdateStrides({ strides, uint32_t(std::size(strides)) });

      shader->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
      shader->SetIndexer(Config::INDEXER_16_BIT);

      shader->SetFillMode(Config::FILL_SOLID);
      shader->SetCullMode(Config::CULL_NONE);
      shader->SetClipEnabled(false);

      const Config::Blend blends[] = {
        { true, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
      };
      shader->UpdateBlends({ blends, uint32_t(std::size(blends)) });
      shader->SetATCEnabled(false);

      shader->SetDepthEnabled(false);
      shader->SetDepthWrite(false);
      shader->SetDepthComparison(Config::COMPARISON_ALWAYS);

      const Config::Viewport viewports[] = {
        { 0.0f, 0.0f, float(extent_x), float(extent_y), 0.0f, 1.0f },
      };
      shader->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
    }



    auto constants_resource = device->CreateResource("imgui_constants");
    {
      const float L = 0.0f;
      const float R = static_cast<float>(extent_x);
      const float B = static_cast<float>(extent_y);
      const float T = 0.0f;
      const float mvp[4][4] = {
        { 2.0f / (R - L),     0.0f,               0.0f,       0.0f },
        { 0.0f,               2.0f / (T - B),     0.0f,       0.0f },
        { 0.0f,               0.0f,               0.5f,       0.0f },
        { (R + L) / (L - R),  (T + B) / (B - T),  0.5f,       1.0f },
      };

      uint32_t constants_stride{ 4 };
      uint32_t constants_count{ 16 };

      constants_resource->SetType(Resource::TYPE_BUFFER);
      constants_resource->SetStride(constants_stride);
      constants_resource->SetCount(constants_count);

      constants_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
      constants_property->RawAllocate(uint32_t(sizeof(mvp)));
      constants_property->SetRawBytes({ &mvp[0][0], uint32_t(sizeof(mvp)) }, 0);
      constants_resource->SetInteropCount(1);
      constants_resource->SetInteropItem(0, constants_property->GetRawBytes(0));
    }

    auto constants_view = constants_resource->CreateView("constants_cd_view");
    {
      constants_view->SetBind(View::BIND_CONSTANT_DATA);
      //constants_view->SetOffset(0);
      //constants_view->SetRange(uint32_t(-1));
    }


    auto font_resource = device->CreateResource("imgui_font");
    {
      ImGui::GetIO().Fonts->AddFontFromFileTTF("3rdparty/imgui/fonts/Roboto-Medium.ttf", 20.0f);

      unsigned char* font_data{ nullptr };
      int font_stride{ 4 };
      int font_width{ 0 };
      int font_height{ 0 };
      ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&font_data, &font_width, &font_height);

      font_resource->SetType(Resource::TYPE_IMAGE2D);
      font_resource->SetExtentX(font_width);
      font_resource->SetExtentY(font_height);
      font_resource->SetLayers(1);
      font_resource->SetMipmaps(1);
      font_resource->SetFormat(FORMAT_R8G8B8A8_UNORM);

      font_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
      font_property->RawAllocate(font_stride * font_width * font_height);
      font_property->SetRawBytes({ font_data, font_stride * font_width * font_height }, 0);
      font_resource->SetInteropCount(1);
      font_resource->SetInteropItem(0, font_property->GetRawBytes(0));
    }

    auto font_view = font_resource->CreateView("font_sr_view");
    {
      font_view->SetBind(View::BIND_SHADER_RESOURCE);
      //font_view->SetOffset(0);
    }

    auto layout = device->CreateLayout("imgui_layout");
    {
      const std::shared_ptr<View> ub_views[] = {
        constants_view,
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


    auto pass = device->CreatePass("imgui_pass");
    {
      pass->SetType(Pass::TYPE_GRAPHIC);
      pass->SetEnabled(true);

      const std::shared_ptr<View> rt_views[] = {
        output_view,
      };
      pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

      //const std::shared_ptr<View> ds_views[] = {
      //  depth_view,
      //};
      //pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });


      const Pass::RTValue rt_values[] = {
        {}, //std::array<float, 4>{ 0.2f, 0.2f, 0.2f, 0.0f },
      };
      pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

      //const Pass::DSValue ds_values[] = {
      //  { 1.0f, std::nullopt },
      //};
      //pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });


      pass->SetSubpassCount(sub_limit);
      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        auto vtx_resource = device->CreateResource("imgui_vtx_resource_" + std::to_string(i));
        vtx_resource->SetType(Resource::TYPE_BUFFER);
        vtx_resource->SetStride(uint32_t(sizeof(ImDrawVert)));
        vtx_resource->SetCount(vtx_limit);
        vtx_resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);

        auto vtx_view = vtx_resource->CreateView("vtx_va_view_" + std::to_string(i));
        vtx_view->SetBind(View::BIND_VERTEX_ARRAY);
        vtx_view->SetByteOffset(0);

        const std::shared_ptr<View> va_views[] = {
          vtx_view,
        };
        pass->UpdateSubpassVAViews(i, { va_views, uint32_t(std::size(va_views)) });


        auto idx_resource = device->CreateResource("imgui_idx_resource_" + std::to_string(i));
        idx_resource->SetType(Resource::TYPE_BUFFER);
        idx_resource->SetStride(uint32_t(sizeof(ImDrawIdx)));
        idx_resource->SetCount(10000);
        idx_resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);

        auto idx_view = idx_resource->CreateView("idx_ia_view_" + std::to_string(i));
        idx_view->SetBind(View::BIND_INDEX_ARRAY);
        idx_view->SetByteOffset(0);
        idx_view->SetByteCount(2);

        const std::shared_ptr<View> ia_views[] = {
          idx_view,
        };
        pass->UpdateSubpassIAViews(i, { ia_views, uint32_t(std::size(ia_views)) });


        auto arg_resource = device->CreateResource("imgui_arg_resource_" + std::to_string(i));
        arg_resource->SetType(Resource::TYPE_BUFFER);
        arg_resource->SetStride(uint32_t(sizeof(Pass::Graphic)));
        arg_resource->SetCount(10);
        arg_resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);

        for (uint32_t j = 0; j < arg_limit; ++j)
        {
          auto arg_view = arg_resource->CreateView("arg_ci_view_" + std::to_string(j));
          arg_view->SetBind(View::BIND_COMMAND_INDIRECT);
          arg_view->SetByteOffset(j * sizeof(Pass::Graphic));
          arg_view->SetByteCount(sizeof(Pass::Graphic));
          views[i][j] = arg_view;
        }
        pass->UpdateSubpassAAViews(i, { views[i].data(), arg_limit });

        pass->SetSubpassLayout(i, layout);
        pass->SetSubpassShader(i, shader);

        vtx_resources[i] = vtx_resource;
        idx_resources[i] = idx_resource;
        arg_resources[i] = arg_resource;
      }
    }
    this->pass = pass;


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
    const auto core = &this->GetCore();
    const auto extent_x = property->GetObjectItem("camera")->GetObjectItem("extent_x")->GetUint();
    const auto extent_y = property->GetObjectItem("camera")->GetObjectItem("extent_y")->GetUint();

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2(float(extent_x), float(extent_y));


    const auto now = std::chrono::high_resolution_clock::now();
    const auto delta = std::chrono::duration_cast<std::chrono::duration<double>>(now - time);
    io.DeltaTime = delta.count();
    time = now;

    // Start the frame
    ImGui::NewFrame();
    if (show_test_window_)
    {
      ImGui::ShowDemoWindow(&show_test_window_);
    }

    {
      for (uint32_t i = 0; i < sub_limit; ++i)
      {
        auto& arg_resource = arg_resources[i];

        auto arg_mapped = arg_resource->Map();
        for (uint32_t j = 0; j < arg_limit; ++j)
        {
          auto& graphic_arg = reinterpret_cast<Pass::Graphic*>(arg_mapped)[j];
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
          auto& graphic_arg = reinterpret_cast<Pass::Graphic*>(arg_mapped)[j];
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

  Imgui::Imgui(Core& core)
    : Usable("imgui")
    , core(core)
  {
    time = std::chrono::high_resolution_clock::now();
  }

  Imgui::~Imgui()
  {
  }
}
