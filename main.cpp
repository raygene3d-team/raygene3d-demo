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


#include "spark/spark.h"
#include "imgui/imgui.h"

constexpr char app_name[] = { "Spark Demo" };

constexpr uint32_t def_extent_x{ 16u * 80 };
constexpr uint32_t def_extent_y{  9u * 80 };
constexpr float def_fov_x{ 60.0f };
constexpr float def_fov_y{ 60.0f };
constexpr float def_n_plane{ 0.1f };
constexpr float def_f_plane{ 100.0f };
constexpr uint32_t def_device_ordinal{ 0 };
constexpr bool def_device_debug{ false };

constexpr char config_path[] = { "config.json" };

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

std::shared_ptr<RayGene3D::Property> LoadConfig(const std::string& file_path)
{
  const auto directory = std::string("./");
  auto property = RayGene3D::LoadProperty(directory, ExtractName(config_path));

  const auto prop_extent_x = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
  {
    const auto extent_x = property->CheckObjectItem("size_x") ? property->GetObjectItem("size_x")->GetUint() : def_extent_x;
    prop_extent_x->SetUint(extent_x);
  }
  property->SetObjectItem("extent_x", prop_extent_x);

  const auto prop_extent_y = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
  {
    const auto extent_y = property->CheckObjectItem("size_y") ? property->GetObjectItem("size_y")->GetUint() : def_extent_y;
    prop_extent_y->SetUint(extent_y);
  }
  property->SetObjectItem("extent_y", prop_extent_y);

  const auto prop_fov_x = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_REAL));
  {
    const auto fov_x = property->CheckObjectItem("fov_x") ? property->GetObjectItem("fov_x")->GetReal() : def_fov_x;
    prop_fov_x->SetReal(fov_x);
  }
  property->SetObjectItem("fov_x", prop_fov_x);

  const auto prop_fov_y = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_REAL));
  {
    const auto fov_y = property->CheckObjectItem("fov_y") ? property->GetObjectItem("fov_y")->GetReal() : def_fov_y;
    prop_fov_y->SetReal(fov_y);
  }
  property->SetObjectItem("fov_y", prop_fov_y);

  const auto prop_n_plane = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_REAL));
  {
    const auto n_plane = property->CheckObjectItem("n_plane") ? property->GetObjectItem("n_plane")->GetReal() : def_n_plane;
    prop_n_plane->SetReal(n_plane);
  }
  property->SetObjectItem("n_plane", prop_n_plane);

  const auto prop_f_plane = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_REAL));
  {
    const auto f_plane = property->CheckObjectItem("f_plane") ? property->GetObjectItem("f_plane")->GetReal() : def_f_plane;
    prop_f_plane->SetReal(f_plane);
  }
  property->SetObjectItem("f_plane", prop_f_plane);

  const auto prop_d_ordinal = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
  {
    const auto device_ordinal = property->CheckObjectItem("device_ordinal") ? property->GetObjectItem("device_ordinal")->GetUint() : def_device_ordinal;
    prop_d_ordinal->SetUint(device_ordinal);
  }
  property->SetObjectItem("device_ordinal", prop_d_ordinal);

  const auto prop_d_debug = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_BOOL));
  {
    const auto device_debug = property->CheckObjectItem("device_debug") ? property->GetObjectItem("device_debug")->GetBool() : def_device_debug;
    prop_d_debug->SetBool(device_debug);
  }
  property->SetObjectItem("device_debug", prop_d_debug);

  return property;
}

std::shared_ptr<RayGene3D::Property> LoadScene(const std::string& file_path, const std::string& file_name)
{
  auto property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));

  std::ifstream input((file_path + file_name).c_str());
  nlohmann::json json;
  input >> json;
  input.close();

  const auto scene_json_propertry = RayGene3D::ParseJSON(json);

  {
    const auto scene_json_property = scene_json_propertry->GetObjectItem("scene");

    const auto scene_file = scene_json_property->GetObjectItem("file")->GetString();
    const auto scene_flip = scene_json_property->GetObjectItem("flip")->GetBool();
    const auto scene_scale = scene_json_property->GetObjectItem("scale")->GetReal();
    const auto scene_quality = scene_json_property->GetObjectItem("quality")->GetUint();

    const auto directory = std::string("cache/");
    std::shared_ptr<RayGene3D::Property> scene_prop;
    try
    {
      scene_prop = RayGene3D::LoadProperty(directory, ExtractName(scene_file));
    }
    catch (std::exception e)
    {
      const auto extension = ExtractExtension(scene_file);

      if (std::strcmp(extension.c_str(), "obm") == 0)
      {
        scene_prop = RayGene3D::ImportOBJ(file_path, scene_file, scene_flip, scene_scale, scene_quality);
      }
      else if (std::strcmp(extension.c_str(), "gltf") == 0)
      {
        scene_prop = RayGene3D::ImportGLTF(file_path, scene_file, scene_flip, scene_scale, scene_quality);
      }

      RayGene3D::SaveProperty(directory, ExtractName(scene_file), scene_prop);
    }

    property->SetObjectItem("scene", scene_prop);
  }

  {
    const auto camera_json_property = scene_json_propertry->GetObjectItem("camera");

    const auto prop_eye = camera_json_property->GetObjectItem("eye");
    const auto prop_lookat = camera_json_property->GetObjectItem("lookat");
    const auto prop_up = camera_json_property->GetObjectItem("up");

    const auto prop_counter = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
    prop_counter->SetUint(0);

    const auto camera_property = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_OBJECT));

    camera_property->SetObjectItem("eye", prop_eye);
    camera_property->SetObjectItem("lookat", prop_lookat);
    camera_property->SetObjectItem("up", prop_up);

    camera_property->SetObjectItem("counter", prop_counter);

    property->SetObjectItem("camera", camera_property);
  }

  {
    const auto environment_json_property = scene_json_propertry->GetObjectItem("environment");

    const auto environment_file = environment_json_property->GetObjectItem("file")->GetString();
    const auto environment_exposure = environment_json_property->GetObjectItem("exposure")->GetReal();
    const auto environment_quality = environment_json_property->GetObjectItem("quality")->GetUint();

    const auto environment_prop = RayGene3D::ImportEXR(file_path, environment_file, environment_exposure, environment_quality);

    property->SetObjectItem("environment", environment_prop);
  }

  return property;
}

namespace RayGene3D
{
  class GLFWWrapper
  {
  protected:
    GLFWwindow* glfw{ nullptr };

  protected:
    std::shared_ptr<RayGene3D::Core> core;

  protected:
    std::shared_ptr<RayGene3D::Spark> spark;
    std::shared_ptr<RayGene3D::Imgui> imgui;

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
        direction[0] = -1.0 * delta_xpos;
        direction[1] = -1.0 * delta_ypos;

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
          up = glm::rotate(up, camera_roll_speed_ * rotation_sign * float(delta_time), lookat);

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
      auto config_property = LoadConfig(config_path);

      prop_extent_x = config_property->GetObjectItem("extent_x");
      prop_extent_y = config_property->GetObjectItem("extent_y");
      prop_fov_x = config_property->GetObjectItem("fov_x");
      prop_fov_y = config_property->GetObjectItem("fov_y");
      prop_n_plane = config_property->GetObjectItem("n_plane");
      prop_f_plane = config_property->GetObjectItem("f_plane");

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

      const auto file_path = config_property->GetObjectItem("file_path")->GetString();
      const auto file_name = config_property->GetObjectItem("file_name")->GetString();

      auto property = LoadScene(file_path, file_name);

      const auto prop_camera = property->GetObjectItem("camera");
      prop_eye = prop_camera->GetObjectItem("eye");
      prop_lookat = prop_camera->GetObjectItem("lookat");
      prop_up = prop_camera->GetObjectItem("up");
      prop_counter = prop_camera->GetObjectItem("counter");

      prop_camera->SetObjectItem("extent_x", prop_extent_x);
      prop_camera->SetObjectItem("extent_y", prop_extent_y);
      prop_camera->SetObjectItem("fov_x", prop_fov_x);
      prop_camera->SetObjectItem("fov_y", prop_fov_y);
      prop_camera->SetObjectItem("n_plane", prop_n_plane);
      prop_camera->SetObjectItem("f_plane", prop_f_plane);
      

      auto device = core->AccessDevice();
      const auto device_debug = config_property->GetObjectItem("device_debug")->GetBool();
      const auto device_ordinal = config_property->GetObjectItem("device_ordinal")->GetUint();
      device->SetWindow(window_handle);
      device->SetDispaly(display_handle);
      device->SetExtentX(extent_x);
      device->SetExtentY(extent_y);
      device->SetOrdinal(device_ordinal);
      device->SetDebug(device_debug);

      auto screen_backbuffer_resource = core->AccessDevice()->CreateResource("screen_backbuffer");
      {
        std::vector<glm::u8vec4> frame_pixels(extent_x * extent_y, glm::u8vec4(0, 0, 0, 0));

        const auto data = frame_pixels.data();
        const auto stride = uint32_t(sizeof(glm::u8vec4));
        const auto count = uint32_t(extent_x * extent_y);
        prop_screen = CreateBufferProperty(data, stride, count);

        screen_backbuffer_resource->SetType(RayGene3D::Resource::TYPE_IMAGE2D);
        screen_backbuffer_resource->SetExtentX(uint32_t(extent_x));
        screen_backbuffer_resource->SetExtentY(uint32_t(extent_y));
        screen_backbuffer_resource->SetFormat(RayGene3D::FORMAT_B8G8R8A8_UNORM);
        screen_backbuffer_resource->SetInteropCount(1);
        screen_backbuffer_resource->SetInteropItem(0, prop_screen->GetRawBytes(0));
      }
      core->AccessDevice()->SetScreen(screen_backbuffer_resource);

      spark = std::shared_ptr<RayGene3D::Spark>(new RayGene3D::Spark(*core));
      spark->AccessProperty() = property;
      spark->Initialize();
      spark->SetShadowType(Spark::NO_SHADOWS);

      imgui = std::shared_ptr<RayGene3D::Imgui>(new RayGene3D::Imgui(*core));
      imgui->AccessProperty() = property;
      imgui->SetShowTestWindow(false);
      imgui->Initialize();

      core->Initialize();
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
          auto enabled = imgui->GetShowTestWindow();
          enabled = !enabled;
          imgui->SetShowTestWindow(enabled);

          if (enabled)
          {
            glfwSetWindowUserPointer(glfw, this);

            glfwSetCursorPosCallback(glfw, [](GLFWwindow* window, double xpos, double ypos)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->imgui->OnCursor(xpos, ypos); }
            );
            glfwSetKeyCallback(glfw, [](GLFWwindow* window, int key, int scancode, int action, int mods)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->imgui->OnKeyboard(key, scancode, action, mods); }
            );
            glfwSetMouseButtonCallback(glfw, [](GLFWwindow* window, int button, int action, int mods)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->imgui->OnMouse(button, action, mods); }
            );
            glfwSetScrollCallback(glfw, [](GLFWwindow* window, double xoffset, double yoffset)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->imgui->OnScroll(xoffset, yoffset); }
            );
            glfwSetCharCallback(glfw, [](GLFWwindow* window, unsigned int glyph)
              { reinterpret_cast<GLFWWrapper*>(glfwGetWindowUserPointer(window))->imgui->OnChar(glyph); }
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

        if (!imgui->GetShowTestWindow())
        {
          UpdateCamera();
        }

        //if (glfwGetKey(glfw, GLFW_KEY_F2) == GLFW_RELEASE && change_spark)
        //{
        //  auto shading = raygene->GetShading();
        //  if (shading == Raygene::SHADING_CHECKER) { shading = Raygene::SHADING_TRACER; }
        //  else
        //    if (shading == Raygene::SHADING_TRACER) { shading = Raygene::SHADING_CHECKER; }
        //  raygene->SetShading(shading);
        //  change_raygene = false;

        //  auto counter = prop_counter->GetUint();
        //  counter = 1;
        //  prop_counter->SetUint(counter);
        //}

        //if (glfwGetKey(glfw, GLFW_KEY_F2) == GLFW_PRESS && !change_raygene)
        //{
        //  change_raygene = true;
        //}

        if (glfwGetKey(glfw, GLFW_KEY_F2) == GLFW_RELEASE && change_spark)
        {
          auto shadow_type = spark->GetShadowType();
          if (shadow_type == Spark::NO_SHADOWS) { shadow_type = Spark::POINT_SHADOWS; } else
          if (shadow_type == Spark::POINT_SHADOWS) { shadow_type = Spark::NO_SHADOWS; }
          spark->SetShadowType(shadow_type);
          change_spark = false;
        }

        if (glfwGetKey(glfw, GLFW_KEY_F2) == GLFW_PRESS && !change_spark)
        {
          change_spark = true;
        }





        spark->Use();
        imgui->Use();

        core->Use();

        frame_counter += 1;
        frame_period += delta_time;

        if (frame_period > 1.0)
        {
          const auto frame_rate = frame_counter / frame_period;
          const auto adapter_name = core->AccessDevice()->GetName();

          std::stringstream ss;
          ss << app_name << ": " << std::fixed << std::setprecision(1) << frame_rate << " FPS on " << adapter_name;
          glfwSetWindowTitle(glfw, ss.str().c_str());

          frame_counter = 0;
          frame_period = 0.0;
        }
      }
    }

    void Discard()
    {
      core->Discard();

      spark->Discard();
      spark.reset();

      glfwDestroyWindow(glfw);
    }

  public:
    GLFWWrapper()
    {
      glfwInit();

      core = std::shared_ptr<RayGene3D::Core>(new RayGene3D::Core(RayGene3D::Core::API_D11));
    }
    ~GLFWWrapper()
    {
      core.reset();

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
