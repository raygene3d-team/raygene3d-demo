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

#include "broker/render_3d_broker.h"
#include "broker/render_ui_broker.h"

#include "broker/bvh_broker.h"
#include "broker/xatlas_broker.h"
#include "broker/mikktspace_broker.h"
#include "broker/import_broker.h"
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

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#elif _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __OBJC__
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <glfw/glfw3native.h>

#ifdef __OBJC__
#import <Foundation/Foundation.h>
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
  class GLFWWrapper
  {
  protected:
    GLFWwindow* glfw{ nullptr };

  protected:
    std::unique_ptr<RayGene3D::Wrap> wrap;

  protected:
    std::string shadow_name{"No Shadow"};

  protected:
    std::shared_ptr<RayGene3D::Render3DBroker> render_3d_broker;
    std::shared_ptr<RayGene3D::RenderUIBroker> render_ui_broker;

  protected:
    //std::shared_ptr<RayGene3D::IOBroker> io_broker;
    //std::shared_ptr<RayGene3D::ImportBroker> lightmap_broker;
    //std::shared_ptr<RayGene3D::ImportBroker> environment_broker;

  protected:
    std::string config_path;
    std::string config_name;

  protected:
    std::shared_ptr<RayGene3D::Property> scene_property;
    std::shared_ptr<RayGene3D::Property> camera_property;
    std::shared_ptr<RayGene3D::Property> lighting_property;
    std::shared_ptr<RayGene3D::Property> environment_property;

  protected:
    double delta_xpos{ 0.0 };
    double delta_ypos{ 0.0 };
    double delta_time{ 0.0 };

    std::shared_ptr<Property> prop_screen;

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

      if (glfwGetMouseButton(glfw, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
      {
        auto direction = glm::fvec3{ 0.0f, 0.0f, 0.0f };
        direction[0] =-1.0 * delta_xpos;
        direction[1] = 1.0 * delta_ypos;

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
        if (glfwGetKey(glfw, GLFW_KEY_A) == GLFW_PRESS) direction[0] =-1.0f;
        if (glfwGetKey(glfw, GLFW_KEY_D) == GLFW_PRESS) direction[0] = 1.0f;
        if (glfwGetKey(glfw, GLFW_KEY_R) == GLFW_PRESS) direction[1] = 1.0f;
        if (glfwGetKey(glfw, GLFW_KEY_F) == GLFW_PRESS) direction[1] =-1.0f;
        if (glfwGetKey(glfw, GLFW_KEY_W) == GLFW_PRESS) direction[2] = 1.0f;
        if (glfwGetKey(glfw, GLFW_KEY_S) == GLFW_PRESS) direction[2] =-1.0f;

        if (glm::any(glm::notEqual(direction, glm::zero<glm::fvec3>())))
        {
          direction = view * direction;

          eye += direction * camera_speed_ * float(delta_time);
          lookat += direction * camera_speed_ * float(delta_time);

          counter = 0;
        }
      }

      if (glfwGetMouseButton(glfw, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
      {
        if (delta_xpos != 0.0 || delta_ypos != 0.0)
        {
          const float angle_hor = -1.0 * camera_rotation_speed_ * delta_xpos;
          const float angle_ver = -1.0 * camera_rotation_speed_ * delta_ypos;

          lookat = lookat - eye;
          lookat = glm::rotate(lookat, angle_ver, cam_x);
          lookat = glm::rotate(lookat, angle_hor, cam_y);
          lookat = lookat + eye;

          counter = 0;
        }
      }

      {
        float rotation_sign{ 0.0f };

        if (glfwGetKey(glfw, GLFW_KEY_E) == GLFW_PRESS) rotation_sign = 1.0f;
        if (glfwGetKey(glfw, GLFW_KEY_Q) == GLFW_PRESS) rotation_sign =-1.0f;

        if (rotation_sign != 0.0f)
        {
          up = glm::rotate(up, camera_roll_speed_ * rotation_sign * float(delta_time), cam_z);

          counter = 0;
        }
      }

      ++counter;
      if (counter == 1)
      {
        //BLAST_LOG("eye = [%5.3f, %5.3f, %5.3f]; lookat = [%5.3f, %5.3f, %5.3f]; up = [%5.3f, %5.3f, %5.3f]",
        //  eye.x, eye.y, eye.z, lookat.x, lookat.y, lookat.z, up.x, up.y, up.z);
      }

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

      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfw = glfwCreateWindow(extent_x, extent_y, app_name, nullptr, nullptr);

      void* window_handle = nullptr;
      void* display_handle = nullptr;
#ifdef __linux__
      window_handle = reinterpret_cast<void*>(glfwGetX11Window(glfw));
      display_handle = glfwGetX11Display();
#elif _WIN32
      window_handle = glfwGetWin32Window(glfw);
      display_handle = GetModuleHandle(nullptr);
#elif __OBJC__
      id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
      NSWindow* nswin = glfwGetCocoaWindow(glfw);
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
        const auto stride = uint32_t(sizeof(glm::u8vec4));
        const auto count = uint32_t(extent_x * extent_y);
        auto raw = RayGene3D::Raw{ stride * count };
        for (auto i = 0u; i < count; ++i)
        {
          raw.SetElement<glm::u8vec4>({ 0, 0, 0, 0 }, i);
        }
        prop_screen = CreateBufferProperty({ &raw, 1 }, stride, count);
        std::pair<const void*, uint32_t> interops[] =
        {
          prop_screen->GetObjectItem("raws")->GetArrayItem(0)->GetRawBytes(0)
        };

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
          { interops, uint32_t(std::size(interops)) }
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

      //scene_property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));
      {
        const auto scene_name = config_property->GetObjectItem("scene")->GetObjectItem("path")->GetString();
        const auto scene_path = config_path;

        //std::shared_ptr<Property> scene_property;
        wrap->GetUtil()->GetStorage()->Load(ExtractName(scene_name), scene_property);

        if (!scene_property)
        {
          auto import_broker = std::shared_ptr<RayGene3D::ImportBroker>(new RayGene3D::ImportBroker(*wrap));

          import_broker->SetFileName(scene_name);
          import_broker->SetPathName(scene_path);

          const auto scene_rhs = config_property->GetObjectItem("scene")->GetObjectItem("rhs")->GetBool();
          const auto scene_scale = config_property->GetObjectItem("scene")->GetObjectItem("scale")->GetReal();
          const auto scene_quality = config_property->GetObjectItem("scene")->GetObjectItem("quality")->GetUint();

          import_broker->SetCoordinateConversion(scene_rhs);
          import_broker->SetPositionScale(scene_scale);
          import_broker->SetTextureLevel(scene_quality);

          import_broker->Initialize();
          import_broker->Export(scene_property);

          import_broker.reset();

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

      //environment_property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));
      //{
      //  const auto environment_path = "./";
      //  const auto environment_name = config_property->GetObjectItem("environment")->GetObjectItem("file")->GetString();
      //  const auto environment_quality = config_property->GetObjectItem("environment")->GetObjectItem("quality")->GetUint();

      //  const auto extent_x = 1u << int32_t(environment_quality) - 1;
      //  const auto extent_y = 1u << int32_t(environment_quality) - 2;
      //  auto [raws, size_x, size_y] = 
      //    MipmapTextureHDR(environment_quality, 
      //      ResizeTextureHDR(extent_x, extent_y,
      //        LoadTextureHDR(environment_path + environment_name)));
      //  environment_property = CreateTextureProperty({raws.data(), uint32_t(raws.size())}, 
      //    extent_x, extent_y, environment_quality, 1u);
      //}
      tree_property->SetObjectItem("environment", config_property->GetObjectItem("environment"));

      //lighting_property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));
      //{
      //  const auto lighting_theta = config_property->GetObjectItem("lighting")->GetObjectItem("theta");
      //  const auto lighting_phi = config_property->GetObjectItem("lighting")->GetObjectItem("phi");
      //  const auto lighting_intensity = config_property->GetObjectItem("lighting")->GetObjectItem("intensity");

      //  lighting_property->SetObjectItem("theta", lighting_theta);
      //  lighting_property->SetObjectItem("phi", lighting_phi);
      //  lighting_property->SetObjectItem("intensity", lighting_intensity);
      //}
      tree_property->SetObjectItem("lighting", config_property->GetObjectItem("lighting"));

      tree_property->SetObjectItem("illumination", config_property->GetObjectItem("illumination"));

      {
        auto mikktspace_broker = std::shared_ptr<RayGene3D::MikktspaceBroker>(new RayGene3D::MikktspaceBroker(*wrap));

        mikktspace_broker->Initialize();
        mikktspace_broker->Use();
        mikktspace_broker->Discard();

        mikktspace_broker.reset();
      }

      {
        auto xatlas_broker = std::shared_ptr<RayGene3D::XAtlasBroker>(new RayGene3D::XAtlasBroker(*wrap));

        xatlas_broker->Initialize();
        xatlas_broker->Use();
        xatlas_broker->Discard();

        xatlas_broker.reset();
      }

      {
        auto environment_broker = std::shared_ptr<RayGene3D::EnvironmentBroker>(new RayGene3D::EnvironmentBroker(*wrap));

        environment_broker->Initialize();
        environment_broker->Use();
        environment_broker->Discard();

        environment_broker.reset();
      }

      {
        auto bvh_broker = std::shared_ptr<RayGene3D::BVHBroker>(new RayGene3D::BVHBroker(*wrap));

        bvh_broker->Initialize();
        bvh_broker->Use();
        bvh_broker->Discard();

        bvh_broker.reset();
      }

      {
        auto lightmap_broker = std::shared_ptr<RayGene3D::LightmapBroker>(new RayGene3D::LightmapBroker(*wrap));

        lightmap_broker->Initialize();
        lightmap_broker->Use();
        lightmap_broker->Discard();

        lightmap_broker.reset();
      }


      render_3d_broker = std::shared_ptr<RayGene3D::Render3DBroker>(new RayGene3D::Render3DBroker(*wrap));
      render_3d_broker->SetShadowMode(Render3DBroker::NO_SHADOW);

      render_ui_broker = std::shared_ptr<RayGene3D::RenderUIBroker>(new RayGene3D::RenderUIBroker(*wrap));
      render_ui_broker->SetShowTestWindow(false);
    }

    void Use()
    {
      auto frame_counter{ 0u };
      auto frame_period{ 0.0 };
     
      auto change_imgui{ false };
      auto change_spark{ false };;

      auto curr_time = glfwGetTime();
      auto curr_xpos{ 0.0 };
      auto curr_ypos{ 0.0 };
      glfwGetCursorPos(glfw, &curr_xpos, &curr_ypos);

      while (!glfwWindowShouldClose(glfw))
      {
        const auto last_time = curr_time;
        curr_time = glfwGetTime();
        delta_time = curr_time - last_time;

        const auto last_xpos = curr_xpos;
        const auto last_ypos = curr_ypos;
        glfwGetCursorPos(glfw, &curr_xpos, &curr_ypos);
        delta_xpos = curr_xpos - last_xpos;
        delta_ypos = curr_ypos - last_ypos;

        auto counter = prop_counter->GetUint();
        counter += 1;
        prop_counter->SetUint(counter);

        glfwPollEvents();

        if (glfwGetKey(glfw, GLFW_KEY_F1) == GLFW_RELEASE && change_imgui)
        {
          auto enabled = render_ui_broker->GetShowTestWindow();
          enabled = !enabled;
          render_ui_broker->SetShowTestWindow(enabled);

          if (enabled)
          {
            glfwSetWindowUserPointer(glfw, this);

            glfwSetCursorPosCallback(glfw, [](GLFWwindow* window, double xpos, double ypos)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->render_ui_broker->OnCursor(xpos, ypos); }
            );
            glfwSetKeyCallback(glfw, [](GLFWwindow* window, int key, int scancode, int action, int mods)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->render_ui_broker->OnKeyboard(key, scancode, action, mods); }
            );
            glfwSetMouseButtonCallback(glfw, [](GLFWwindow* window, int button, int action, int mods)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->render_ui_broker->OnMouse(button, action, mods); }
            );
            glfwSetScrollCallback(glfw, [](GLFWwindow* window, double xoffset, double yoffset)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->render_ui_broker->OnScroll(xoffset, yoffset); }
            );
            glfwSetCharCallback(glfw, [](GLFWwindow* window, unsigned int glyph)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->render_ui_broker->OnChar(glyph); }
            );
          }
          else
          {
            glfwSetWindowUserPointer(glfw, nullptr);

            glfwSetCursorPosCallback(glfw, nullptr);
            glfwSetKeyCallback(glfw, nullptr);
            glfwSetMouseButtonCallback(glfw, nullptr);
            glfwSetScrollCallback(glfw, nullptr);
            glfwSetCharCallback(glfw, nullptr);
          }

          change_imgui = false;
        }

        if (glfwGetKey(glfw, GLFW_KEY_F1) == GLFW_PRESS && !change_imgui)
        {
          change_imgui = true;
        }

        if (!render_ui_broker->GetShowTestWindow())
        {
          UpdateCamera();
        }

        if (glfwGetKey(glfw, GLFW_KEY_F2) == GLFW_RELEASE && change_spark)
        {
          auto shadow_mode = render_3d_broker->GetShadowMode();
          if (shadow_mode == Render3DBroker::NO_SHADOW)
          {
            shadow_mode = Render3DBroker::CUBEMAP_SHADOW;
            shadow_name = "Cubemap Shadow";
          }
          else if (shadow_mode == Render3DBroker::CUBEMAP_SHADOW)
          {
            shadow_mode = Render3DBroker::SW_TRACED_SHADOW;
            shadow_name = "SW Traced Shadow";
          }
          else if (shadow_mode == Render3DBroker::SW_TRACED_SHADOW)
          {
            shadow_mode = Render3DBroker::HW_TRACED_SHADOW;
            shadow_name = "HW Traced Shadow";
          }
          else if (shadow_mode == Render3DBroker::HW_TRACED_SHADOW)
          {
            shadow_mode = Render3DBroker::NO_SHADOW;
            shadow_name = "No Shadow";
          }
          render_3d_broker->SetShadowMode(shadow_mode);
          change_spark = false;
        }

        if (glfwGetKey(glfw, GLFW_KEY_F2) == GLFW_PRESS && !change_spark)
        {
          change_spark = true;
        }

        render_3d_broker->Use();
        render_ui_broker->Use();

        wrap->Use();

        frame_counter += 1;
        frame_period += delta_time;

        if (frame_period > 1.0)
        {
          const auto frame_rate = frame_counter / frame_period;
          const auto adapter_name = wrap->GetCore()->GetDevice()->GetName();

          std::stringstream ss;
          ss << app_name << " [" << shadow_name << "] " << ": " << std::fixed << std::setprecision(1) << frame_rate << " FPS on " << adapter_name;
          glfwSetWindowTitle(glfw, ss.str().c_str());

          frame_counter = 0;
          frame_period = 0.0;
        }
      }
    }

    void Discard()
    {
      render_3d_broker.reset();
      render_ui_broker.reset();

      const auto device = wrap->GetCore()->GetDevice().get();
      const auto backbuffer_resource = device->GetScreen();
      device->DestroyResource(backbuffer_resource);

      wrap->Discard();

      glfwDestroyWindow(glfw);
    }

  public:
    GLFWWrapper()
    {
      glfwInit();

      wrap = std::unique_ptr<RayGene3D::Wrap>(new RayGene3D::Wrap(RayGene3D::Core::DEVICE_VLK, RayGene3D::Util::STORAGE_LOCAL));
    }

    ~GLFWWrapper()
    {
      wrap.reset();

      glfwTerminate();
    }
  };
}

int main(int argc, char** argv)
{
  //LogSystem::Init();

  FILE* file = fopen("blast3d.log", "w");
  if (file) { fclose(file); }

  try
  {
    RayGene3D::GLFWWrapper wrapper;

    wrapper.Initialize();
    wrapper.Use();
    wrapper.Discard();
  }
  catch (std::exception e)
  {
    const auto exception_message = e.what();
    //MessageBox(nullptr, exception_message, application_name, 0);
    BLAST_LOG(exception_message);
  }
}
