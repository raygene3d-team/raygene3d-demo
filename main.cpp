/*================================================================================
RayGene3D Framework
--------------------------------------------------------------------------------
RayGene3D is licensed under MIT License
================================================================================
The MIT License
--------------------------------------------------------------------------------
Copyright (c) 2021

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
================================================================================*/

#include "raygene3d-wrap/wrap.h"

#include "raygene3d-util/util/array.h"
#include "raygene3d-util/util/buffer.h"

#include "broker/render_3d_broker.h"
#include "broker/render_ui_broker.h"

#include "broker/trace_broker.h"
#include "broker/xatlas_broker.h"
#include "broker/io_broker.h"
#include "broker/scene_broker.h"
#include "broker/lightmap_broker.h"
#include "broker/environment_broker.h"

constexpr char app_name[] = { "Raygene3D Demo" };

constexpr uint32_t def_extent_x{ 16u * 80 };
constexpr uint32_t def_extent_y{  9u * 80 };
constexpr float def_fov_x{ 60.0f };
constexpr float def_fov_y{ 60.0f };
constexpr float def_n_plane{ 0.1f };
constexpr float def_f_plane{ 100.0f };
constexpr uint32_t def_device_ordinal{ 0 };
constexpr bool def_device_debug{ false };

constexpr char setup_name[] = { "setup.json" };

#include <cstdint>
#include <stdexcept>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#import <simd/simd.h>
#endif

std::shared_ptr<RayGene3D::Property> LoadSetup(const std::string& setup_name)
{
  const auto directory = std::string("./");
  auto property = RayGene3D::LoadProperty(directory, ExtractName(setup_name));

  const auto prop_extent_x = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
  {
    const auto extent_x = property->HasObjectItem("size_x") ? property->GetObjectItem("size_x")->GetUint() : def_extent_x;
    prop_extent_x->SetUint(extent_x);
  }
  property->SetObjectItem("extent_x", prop_extent_x);

  const auto prop_extent_y = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
  {
    const auto extent_y = property->HasObjectItem("size_y") ? property->GetObjectItem("size_y")->GetUint() : def_extent_y;
    prop_extent_y->SetUint(extent_y);
  }
  property->SetObjectItem("extent_y", prop_extent_y);

  const auto prop_fov_x = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_REAL));
  {
    const auto fov_x = property->HasObjectItem("fov_x") ? property->GetObjectItem("fov_x")->GetReal() : def_fov_x;
    prop_fov_x->SetReal(fov_x);
  }
  property->SetObjectItem("fov_x", prop_fov_x);

  const auto prop_fov_y = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_REAL));
  {
    const auto fov_y = property->HasObjectItem("fov_y") ? property->GetObjectItem("fov_y")->GetReal() : def_fov_y;
    prop_fov_y->SetReal(fov_y);
  }
  property->SetObjectItem("fov_y", prop_fov_y);

  const auto prop_n_plane = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_REAL));
  {
    const auto n_plane = property->HasObjectItem("n_plane") ? property->GetObjectItem("n_plane")->GetReal() : def_n_plane;
    prop_n_plane->SetReal(n_plane);
  }
  property->SetObjectItem("n_plane", prop_n_plane);

  const auto prop_f_plane = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_REAL));
  {
    const auto f_plane = property->HasObjectItem("f_plane") ? property->GetObjectItem("f_plane")->GetReal() : def_f_plane;
    prop_f_plane->SetReal(f_plane);
  }
  property->SetObjectItem("f_plane", prop_f_plane);

  const auto prop_d_ordinal = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
  {
    const auto device_ordinal = property->HasObjectItem("device_ordinal") ? property->GetObjectItem("device_ordinal")->GetUint() : def_device_ordinal;
    prop_d_ordinal->SetUint(device_ordinal);
  }
  property->SetObjectItem("device_ordinal", prop_d_ordinal);

  const auto prop_d_debug = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_BOOL));
  {
    const auto device_debug = property->HasObjectItem("device_debug") ? property->GetObjectItem("device_debug")->GetBool() : def_device_debug;
    prop_d_debug->SetBool(device_debug);
  }
  property->SetObjectItem("device_debug", prop_d_debug);

  return property;
}

namespace RayGene3D
{
  class SDLWrapper
  {
  protected:
    SDL_Window* window{ nullptr };

  protected:
    std::unique_ptr<RayGene3D::Wrap> wrap;

  protected:
    std::string shadow_name{ "No Shadow"};
    std::string pipeline_name{ "IA Pipeline" };

  protected:
    std::shared_ptr<RayGene3D::LightmapBroker> lightmap_broker;
    std::shared_ptr<RayGene3D::Render3DBroker> render_3d_broker;
    std::shared_ptr<RayGene3D::RenderUIBroker> render_ui_broker;

  protected:
    std::string config_path;
    std::string config_name;

  protected:
    std::shared_ptr<RayGene3D::Property> scene_property;
    std::shared_ptr<RayGene3D::Property> camera_property;
    std::shared_ptr<RayGene3D::Property> lighting_property;
    std::shared_ptr<RayGene3D::Property> environment_property;

  protected:
    bool ui_enabled{ false };
    bool change_imgui{ false };
    bool change_shadow{ false };
    bool change_pipeline{ false };

  protected:
    double delta_time{ 0.0 };
    double curr_time{ 0.0 };
    float delta_xpos{ 0.0f };
    float delta_ypos{ 0.0f };
    float curr_xpos{ 0.0f };
    float curr_ypos{ 0.0f };

    uint32_t frame_counter{ 0u };
    double frame_period{ 0.0 };

    //std::shared_ptr<Property> prop_screen;

    std::shared_ptr<Property> prop_eye;
    std::shared_ptr<Property> prop_lookat;
    std::shared_ptr<Property> prop_up;

    std::shared_ptr<Property> prop_fov_x;
    std::shared_ptr<Property> prop_fov_y;
    std::shared_ptr<Property> prop_extent_x;
    std::shared_ptr<Property> prop_extent_y;
    std::shared_ptr<Property> prop_n_plane;
    std::shared_ptr<Property> prop_f_plane;

    std::shared_ptr<Property> prop_counter;

    float camera_rotation_speed_{ 0.0025f };
    float camera_roll_speed_{ 0.5f };
    float camera_speed_{ 0.5f };
    float camera_pan_speed_{ 0.005f };

  protected:
    static double NowSeconds()
    {
      return static_cast<double>(SDL_GetTicksNS()) * 1.0e-9;
    }

    // Layout-dependent key state, equivalent to the former glfwGetKey(GLFW_KEY_*) polling
    static bool IsKeyDown(SDL_Keycode key)
    {
      const bool* keys = SDL_GetKeyboardState(nullptr);
      const auto scancode = SDL_GetScancodeFromKey(key, nullptr);
      return keys[scancode];
    }

    static bool IsMouseButtonDown(Uint8 button)
    {
      return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_MASK(button)) != 0;
    }

    // SDL buttons: 1 = left, 2 = middle, 3 = right. ImGui buttons: 0 = left, 1 = right, 2 = middle
    static int ToImGuiButton(Uint8 button)
    {
      switch (button)
      {
        case SDL_BUTTON_LEFT: return 0;
        case SDL_BUTTON_RIGHT: return 1;
        case SDL_BUTTON_MIDDLE: return 2;
        default: return -1;
      }
    }

  public:
    void UpdateCamera()
    {
      auto counter = prop_counter->GetUint();

      auto eye = glm::f32vec3{
        prop_eye->GetArrayItem(0)->GetReal(),
        prop_eye->GetArrayItem(1)->GetReal(),
        prop_eye->GetArrayItem(2)->GetReal()
      };

      auto lookat = glm::f32vec3{
        prop_lookat->GetArrayItem(0)->GetReal(),
        prop_lookat->GetArrayItem(1)->GetReal(),
        prop_lookat->GetArrayItem(2)->GetReal()
      };

      auto up = glm::f32vec3{
        prop_up->GetArrayItem(0)->GetReal(),
        prop_up->GetArrayItem(1)->GetReal(),
        prop_up->GetArrayItem(2)->GetReal()
      };

      const auto cam_z = glm::normalize(lookat - eye);
      const auto cam_x = glm::normalize(glm::cross(up, cam_z));
      const auto cam_y = glm::cross(cam_z, cam_x);
      const auto view = glm::mat3x3(cam_x, cam_y, cam_z);

      if (IsMouseButtonDown(SDL_BUTTON_MIDDLE))
      {
        auto direction = glm::fvec3{ 0.0f, 0.0f, 0.0f };
        direction[0] =-1.0f * delta_xpos;
        direction[1] = 1.0f * delta_ypos;

        if (glm::any(glm::notEqual(direction, glm::zero<glm::fvec3>())))
        {
          direction = view * direction;

          eye += direction * camera_pan_speed_;
          lookat += direction * camera_pan_speed_;

          counter = 0;
        }
      }

      {
        auto direction = glm::fvec3{ 0.0f, 0.0f, 0.0f };
        if (IsKeyDown(SDLK_A)) direction[0] =-1.0f;
        if (IsKeyDown(SDLK_D)) direction[0] = 1.0f;
        if (IsKeyDown(SDLK_R)) direction[1] = 1.0f;
        if (IsKeyDown(SDLK_F)) direction[1] =-1.0f;
        if (IsKeyDown(SDLK_W)) direction[2] = 1.0f;
        if (IsKeyDown(SDLK_S)) direction[2] =-1.0f;

        if (glm::any(glm::notEqual(direction, glm::zero<glm::fvec3>())))
        {
          direction = view * direction;

          eye += direction * camera_speed_ * float(delta_time);
          lookat += direction * camera_speed_ * float(delta_time);

          counter = 0;
        }
      }

      if (IsMouseButtonDown(SDL_BUTTON_LEFT))
      {
        if (delta_xpos != 0.0f || delta_ypos != 0.0f)
        {
          const float angle_hor = -1.0f * camera_rotation_speed_ * delta_xpos;
          const float angle_ver = -1.0f * camera_rotation_speed_ * delta_ypos;

          lookat = lookat - eye;
          lookat = glm::rotate(lookat, angle_ver, cam_x);
          lookat = glm::rotate(lookat, angle_hor, cam_y);
          lookat = lookat + eye;

          counter = 0;
        }
      }

      {
        float rotation_sign{ 0.0f };

        if (IsKeyDown(SDLK_E)) rotation_sign = 1.0f;
        if (IsKeyDown(SDLK_Q)) rotation_sign =-1.0f;

        if (rotation_sign != 0.0f)
        {
          up = glm::rotate(up, camera_roll_speed_ * rotation_sign * float(delta_time), cam_z);

          counter = 0;
        }
      }

      ++counter;

      prop_eye->GetArrayItem(0)->SetReal(eye.x);
      prop_eye->GetArrayItem(1)->SetReal(eye.y);
      prop_eye->GetArrayItem(2)->SetReal(eye.z);

      prop_lookat->GetArrayItem(0)->SetReal(lookat.x);
      prop_lookat->GetArrayItem(1)->SetReal(lookat.y);
      prop_lookat->GetArrayItem(2)->SetReal(lookat.z);

      prop_up->GetArrayItem(0)->SetReal(up.x);
      prop_up->GetArrayItem(1)->SetReal(up.y);
      prop_up->GetArrayItem(2)->SetReal(up.z);

      prop_counter->SetUint(counter);
    }

  public:
    void Initialize()
    {
      auto setup_property = LoadSetup(setup_name);

      config_path = setup_property->GetObjectItem("file_path")->GetString();
      config_name = setup_property->GetObjectItem("file_name")->GetString();

      prop_extent_x = setup_property->GetObjectItem("extent_x");
      prop_extent_y = setup_property->GetObjectItem("extent_y");
      prop_fov_x = setup_property->GetObjectItem("fov_x");
      prop_fov_y = setup_property->GetObjectItem("fov_y");
      prop_n_plane = setup_property->GetObjectItem("n_plane");
      prop_f_plane = setup_property->GetObjectItem("f_plane");

      //prop_counter = CreateUIntProperty();

      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();

#ifdef __linux__
      // The Vulkan surface path is Xlib-only, so force X11 when SDL3 would otherwise pick Wayland
      SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
#endif

      if (!SDL_Init(SDL_INIT_VIDEO))
      {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
      }

      window = SDL_CreateWindow(app_name, int(extent_x), int(extent_y), 0);
      if (!window)
      {
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
      }

      const auto window_props = SDL_GetWindowProperties(window);

      void* window_handle = nullptr;
      void* display_handle = nullptr;
#ifdef __linux__
      window_handle = reinterpret_cast<void*>(static_cast<uintptr_t>(
        SDL_GetNumberProperty(window_props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0)));
      display_handle = SDL_GetPointerProperty(window_props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
#elif _WIN32
      window_handle = SDL_GetPointerProperty(window_props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
      display_handle = SDL_GetPointerProperty(window_props, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
#elif __OBJC__
      id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
      NSWindow* nswin = (__bridge NSWindow*)SDL_GetPointerProperty(window_props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
      CAMetalLayer* layer = [CAMetalLayer layer];
      layer.device = dev;
      layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
      nswin.contentView.layer = layer;
      nswin.contentView.wantsLayer = YES;
      window_handle = (__bridge void*)layer;
#endif

      const auto device = wrap->GetCore()->GetDevice().get();
      const auto device_debug = setup_property->GetObjectItem("device_debug")->GetBool();
      const auto device_ordinal = setup_property->GetObjectItem("device_ordinal")->GetUint();
      device->SetWindow(window_handle);
      device->SetDisplay(display_handle);
      device->SetPath("./asset/shaders/");
      device->SetExtentX(extent_x);
      device->SetExtentY(extent_y);
      device->SetOrdinal(device_ordinal);
      device->SetDebug(device_debug);
      wrap->GetCore()->Initialize();

      {
        auto array = TextureArrayLDR(FORMAT_B8G8R8A8_UNORM, extent_x, extent_y, 1);

        const auto& backbuffer_resource = device->CreateResource("backbuffer_resource",
          Resource::Tex2DDesc
          {
            Usage(USAGE_RENDER_TARGET | USAGE_UNORDERED_ACCESS),
            1,
            1,
            FORMAT_B8G8R8A8_UNORM,
            uint32_t(extent_x),
            uint32_t(extent_y),
          },
          Resource::HINT_UNKNOWN,
          array.Bytes()
        );
        wrap->GetCore()->GetDevice()->SetScreen(backbuffer_resource);

        const auto& backbuffer_rt_view = backbuffer_resource->CreateView("backbuffer_rt_view",
          Usage(USAGE_RENDER_TARGET)
        );
        wrap->GetCore()->AddView(backbuffer_rt_view);

        const auto& backbuffer_ua_view = backbuffer_resource->CreateView("backbuffer_ua_view",
          Usage(USAGE_UNORDERED_ACCESS)
        );
        wrap->GetCore()->AddView(backbuffer_ua_view);
      }

      const auto storage = wrap->GetUtil()->GetStorage().get();

      auto tree_property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));
      storage->SetTree(tree_property);

      std::ifstream input((config_path + config_name).c_str());
      nlohmann::json json;
      input >> json;
      input.close();
      const auto config_property = RayGene3D::ParseJSON(json);

      {
        const auto& scene_name = config_property->GetObjectItem("scene")->GetObjectItem("path")->GetString();
        const auto& scene_path = config_path;

        wrap->GetUtil()->GetStorage()->Load(ExtractName(scene_name), scene_property);

        if (!scene_property)
        {
          auto io_broker = std::shared_ptr<RayGene3D::IOBroker>(new RayGene3D::IOBroker(*wrap));

          const auto scene_rhs = config_property->GetObjectItem("scene")->GetObjectItem("rhs")->GetBool();
          const auto scene_scale = config_property->GetObjectItem("scene")->GetObjectItem("scale")->GetReal();
          const auto scene_quality = config_property->GetObjectItem("scene")->GetObjectItem("quality")->GetUint();

          io_broker->SetConversionRHS(scene_rhs);
          io_broker->SetPositionScale(scene_scale);
          io_broker->SetTextureLevel(scene_quality);

          io_broker->SetFileName(scene_name);
          io_broker->SetPathName(scene_path);
          io_broker->SetDataOperation(IOBroker::LOAD_OPERATION);

          io_broker->Initialize();
          io_broker->Use();
          io_broker->Discard();

          io_broker.reset();

          scene_property = wrap->GetUtil()->GetStorage()->GetTree()->GetObjectItem("scene");
          wrap->GetUtil()->GetStorage()->Save(ExtractName(scene_name), scene_property);
        }
      }
      tree_property->SetObjectItem("scene", scene_property);

      camera_property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));
      {
        prop_eye = config_property->GetObjectItem("camera")->GetObjectItem("eye");
        prop_lookat = config_property->GetObjectItem("camera")->GetObjectItem("lookat");
        prop_up = config_property->GetObjectItem("camera")->GetObjectItem("up");

        prop_counter = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
        prop_counter->SetUint(0);

        camera_property->SetObjectItem("eye", prop_eye);
        camera_property->SetObjectItem("lookat", prop_lookat);
        camera_property->SetObjectItem("up", prop_up);

        camera_property->SetObjectItem("counter", prop_counter);

        camera_property->SetObjectItem("extent_x", prop_extent_x);
        camera_property->SetObjectItem("extent_y", prop_extent_y);
        camera_property->SetObjectItem("fov_x", prop_fov_x);
        camera_property->SetObjectItem("fov_y", prop_fov_y);
        camera_property->SetObjectItem("n_plane", prop_n_plane);
        camera_property->SetObjectItem("f_plane", prop_f_plane);
      }
      tree_property->SetObjectItem("camera", camera_property);

      environment_property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));
      {
        const auto environment_path = "./";
        const auto environment_name = config_property->GetObjectItem("environment")->GetObjectItem("path")->GetString();
        const auto environment_quality = config_property->GetObjectItem("environment")->GetObjectItem("quality")->GetUint();

        const auto extent_x = 1u << int32_t(environment_quality) - 1;
        const auto extent_y = 1u << int32_t(environment_quality) - 2;
        auto environment_array = TextureArrayHDR(FORMAT_R32G32B32A32_FLOAT, extent_x, extent_y, 1);
        environment_array.Load((environment_path + environment_name).c_str(), 0);

        environment_property = environment_array.Export();
      }
      tree_property->SetObjectItem("environment", config_property->GetObjectItem("environment"));

      lighting_property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));
      {
        const auto lighting_theta = config_property->GetObjectItem("lighting")->GetObjectItem("theta");
        const auto lighting_phi = config_property->GetObjectItem("lighting")->GetObjectItem("phi");
        const auto lighting_intensity = config_property->GetObjectItem("lighting")->GetObjectItem("intensity");

        lighting_property->SetObjectItem("theta", lighting_theta);
        lighting_property->SetObjectItem("phi", lighting_phi);
        lighting_property->SetObjectItem("intensity", lighting_intensity);
      }
      tree_property->SetObjectItem("lighting", config_property->GetObjectItem("lighting"));

      tree_property->SetObjectItem("illumination", config_property->GetObjectItem("illumination"));

      {
        auto scene_broker = std::shared_ptr<RayGene3D::SceneBroker>(new RayGene3D::SceneBroker(*wrap));

        scene_broker->Initialize();
        scene_broker->Use();
        scene_broker->Discard();

        scene_broker.reset();
      }

      {
        auto environment_broker = std::shared_ptr<RayGene3D::EnvironmentBroker>(new RayGene3D::EnvironmentBroker(*wrap));

        environment_broker->Initialize();
        environment_broker->Use();
        environment_broker->Discard();

        environment_broker.reset();
      }

      lightmap_broker = std::shared_ptr<RayGene3D::LightmapBroker>(new RayGene3D::LightmapBroker(*wrap));

      render_3d_broker = std::shared_ptr<RayGene3D::Render3DBroker>(new RayGene3D::Render3DBroker(*wrap));

      render_ui_broker = std::shared_ptr<RayGene3D::RenderUIBroker>(new RayGene3D::RenderUIBroker(*wrap));
    }

  public:
    // Called from SDL_AppEvent for every SDL event. Returns false when the app should quit.
    bool HandleEvent(const SDL_Event& event)
    {
      switch (event.type)
      {
        case SDL_EVENT_QUIT:
          return false;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
          if (event.window.windowID == SDL_GetWindowID(window)) return false;
          break;

        case SDL_EVENT_MOUSE_MOTION:
          if (ui_enabled)
          {
            render_ui_broker->OnCursor(event.motion.x, event.motion.y);
          }
          break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
          if (ui_enabled)
          {
            const auto button = ToImGuiButton(event.button.button);
            if (button >= 0)
            {
              render_ui_broker->OnMouse(button, event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            }
          }
          break;

        case SDL_EVENT_MOUSE_WHEEL:
          if (ui_enabled)
          {
            render_ui_broker->OnScroll(event.wheel.x, event.wheel.y);
          }
          break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
          if (ui_enabled)
          {
            render_ui_broker->OnKeyboard(int(event.key.scancode), event.type == SDL_EVENT_KEY_DOWN);
          }
          break;

        case SDL_EVENT_TEXT_INPUT:
          if (ui_enabled)
          {
            render_ui_broker->OnChar(event.text.text);
          }
          break;

        default:
          break;
      }

      return true;
    }

  public:
    // Per-frame update, the body of the former GLFW while loop
    void Iterate()
    {
      const auto last_time = curr_time;
      curr_time = NowSeconds();
      delta_time = curr_time - last_time;

      const auto last_xpos = curr_xpos;
      const auto last_ypos = curr_ypos;
      SDL_GetMouseState(&curr_xpos, &curr_ypos);
      delta_xpos = curr_xpos - last_xpos;
      delta_ypos = curr_ypos - last_ypos;

      auto counter = prop_counter->GetUint();
      counter += 1;
      prop_counter->SetUint(counter);

      const bool f1_down = IsKeyDown(SDLK_F1);
      if (!f1_down && change_imgui)
      {
        auto enabled = render_ui_broker->GetShowTestWindow();
        enabled = !enabled;
        render_ui_broker->SetShowTestWindow(enabled);
        ui_enabled = enabled;

        change_imgui = false;
      }

      if (f1_down && !change_imgui)
      {
        change_imgui = true;
      }

      if (!render_ui_broker->GetShowTestWindow())
      {
        UpdateCamera();
      }

      const bool f2_down = IsKeyDown(SDLK_F2);
      if (!f2_down && change_shadow)
      {
        auto shadow_type = render_3d_broker->GetShadowType();
        if (shadow_type == Render3DBroker::NO_SHADOW)
        {
          shadow_type = Render3DBroker::CUBEMAP_SHADOW;
          shadow_name = "Cubemap Shadow";
        }
        else if (shadow_type == Render3DBroker::CUBEMAP_SHADOW)
        {
          shadow_type = Render3DBroker::SW_TRACED_SHADOW;
          shadow_name = "SW Traced Shadow";
        }
        else if (shadow_type == Render3DBroker::SW_TRACED_SHADOW)
        {
          shadow_type = Render3DBroker::HW_TRACED_SHADOW;
          shadow_name = "HW Traced Shadow";
        }
        else if (shadow_type == Render3DBroker::HW_TRACED_SHADOW)
        {
          shadow_type = Render3DBroker::NO_SHADOW;
          shadow_name = "No Shadow";
        }
        render_3d_broker->SetShadowType(shadow_type);
        render_3d_broker->Update();
        change_shadow = false;
      }

      if (f2_down && !change_shadow)
      {
        change_shadow = true;
      }

      const bool f3_down = IsKeyDown(SDLK_F3);
      if (!f3_down && change_pipeline)
      {
        auto pipeline_type = render_3d_broker->GetPipelineType();
        if (pipeline_type == Render3DBroker::IA_PIPELINE)
        {
          pipeline_type = Render3DBroker::MESH_PIPELINE;
          pipeline_name = "Mesh Pipeline";
        }
        else if (pipeline_type == Render3DBroker::MESH_PIPELINE)
        {
          pipeline_type = Render3DBroker::IA_PIPELINE;
          pipeline_name = "IA Pipeline";
        }
        render_3d_broker->SetPipelineType(pipeline_type);
        render_3d_broker->Update();
        change_pipeline = false;
      }

      if (f3_down && !change_pipeline)
      {
        change_pipeline = true;
      }

      lightmap_broker->Use();
      render_3d_broker->Use();
      render_ui_broker->Use();

      wrap->Use();

      frame_counter += 1;
      frame_period += delta_time;

      if (frame_period > 1.0)
      {
        const auto frame_rate = frame_counter / frame_period;
        const auto& adapter_name = wrap->GetCore()->GetDevice()->GetName();

        std::stringstream ss;
        ss << app_name << " [" << shadow_name << "] [" << pipeline_name << "] " << ": " << std::fixed << std::setprecision(1) << frame_rate << " FPS on " << adapter_name;
        SDL_SetWindowTitle(window, ss.str().c_str());

        frame_counter = 0;
        frame_period = 0.0;
      }
    }

  public:
    void Discard()
    {
      lightmap_broker.reset();
      render_3d_broker.reset();
      render_ui_broker.reset();

      if (wrap)
      {
        const auto device = wrap->GetCore()->GetDevice().get();
        const auto backbuffer_resource = device->GetScreen();
        device->DestroyResource(backbuffer_resource);

        wrap->Discard();
      }

      if (window)
      {
        SDL_DestroyWindow(window);
        window = nullptr;
      }
    }

  public:
    SDLWrapper()
    {
      wrap = std::unique_ptr<RayGene3D::Wrap>(new RayGene3D::Wrap(RayGene3D::Core::DEVICE_D11, RayGene3D::Util::STORAGE_LOCAL));
    }

    ~SDLWrapper()
    {
      wrap.reset();

      SDL_Quit();
    }
  };
}

namespace
{
  // Owned by SDL_AppInit/SDL_AppQuit; created on the main thread as the callbacks run there
  RayGene3D::SDLWrapper* g_app{ nullptr };
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
  FILE* file = fopen("blast3d.log", "w");
  if (file) { fclose(file); }

  try
  {
    g_app = new RayGene3D::SDLWrapper();
    g_app->Initialize();

    *appstate = g_app;
    return SDL_APP_CONTINUE;
  }
  catch (const std::exception& e)
  {
    BLAST_LOG(e.what());
    return SDL_APP_FAILURE;
  }
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
  auto* app = static_cast<RayGene3D::SDLWrapper*>(appstate);
  return app->HandleEvent(*event) ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
  auto* app = static_cast<RayGene3D::SDLWrapper*>(appstate);

  try
  {
    app->Iterate();
    return SDL_APP_CONTINUE;
  }
  catch (const std::exception& e)
  {
    BLAST_LOG(e.what());
    return SDL_APP_FAILURE;
  }
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
  auto* app = static_cast<RayGene3D::SDLWrapper*>(appstate);
  if (app)
  {
    app->Discard();
  }

  delete app;
  g_app = nullptr;
}
