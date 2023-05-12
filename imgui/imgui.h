#pragma once
#include "../raygene3d-core/base.h"
#include "../raygene3d-util/core.h"

namespace RayGene3D
{
  class Device;
  class Output;
  class Asset;

  class Imgui : public Usable
  {
  protected:
    Core& core;

  public:
    Core& GetCore() { return core; }

  protected:
    std::shared_ptr<Property> root_property;

  private:
    std::shared_ptr<View> output_view;

  protected:
    bool show_test_window{ true };

    const uint32_t vtx_limit{ 10000 };
    const uint32_t idx_limit{ 10000 };

    static const uint32_t sub_limit{ 10 };
    static const uint32_t arg_limit{ 10 };

    std::shared_ptr<Config> config;
    std::shared_ptr<Layout> layout;
    std::shared_ptr<Pass> pass;

    std::shared_ptr<Resource> proj_resource;
    std::shared_ptr<Resource> font_resource;

    //std::array<std::array<std::shared_ptr<View>, arg_limit>, sub_limit> views;
    std::array<std::shared_ptr<Resource>, sub_limit> vtx_resources;
    std::array<std::shared_ptr<Resource>, sub_limit> idx_resources;
    std::array<std::shared_ptr<Resource>, sub_limit> arg_resources;

    std::shared_ptr<Property> proj_property;
    std::shared_ptr<Property> font_property;

  protected:
    std::chrono::high_resolution_clock::time_point time;

  public:
    std::shared_ptr<Property>& AccessRootProperty() { return root_property; }

  public:
    std::shared_ptr<View>& AccessOutputView() { return output_view; }

  public:
    void SetShowTestWindow(bool show_test_window) { this->show_test_window = show_test_window; }
    bool GetShowTestWindow() const { return show_test_window; }

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    void OnCursor(double xpos, double ypos);
    void OnKeyboard(int key, int scancode, int action, int mods);
    void OnMouse(int button, int action, int mods);
    void OnScroll(double xoffset, double yoffset);
    void OnChar(unsigned int glyph);

  public:
    Imgui(Core& core);
    virtual ~Imgui();
  };
}