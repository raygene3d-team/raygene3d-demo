#pragma once
#include "../raygene3d-wrap/wrap.h"

namespace RayGene3D
{
  class RenderUIBroker : public Broker
  {
  protected:
    std::shared_ptr<View> backbuffer_rtv;
    std::shared_ptr<View> backbuffer_uav;

  protected:
    std::shared_ptr<Property> prop_camera;

  protected:
    std::shared_ptr<Property> prop_extent_x;
    std::shared_ptr<Property> prop_extent_y;

  protected:
    std::shared_ptr<Property> proj_property;
    std::shared_ptr<Property> font_property;

  protected:
    bool show_test_window{ false };

    const uint32_t vtx_limit{ 10000 };
    const uint32_t idx_limit{ 10000 };

    static const uint32_t sub_limit{ 10 };
    static const uint32_t arg_limit{ 10 };

    std::shared_ptr<Pass> pass;
    std::shared_ptr<Batch> batch;
    std::shared_ptr<Config> config;

    std::shared_ptr<Resource> proj_resource;
    std::shared_ptr<Resource> font_resource;

    std::shared_ptr<Resource> vtx_resource;
    std::shared_ptr<Resource> idx_resource;
    std::shared_ptr<Resource> arg_resource;

  protected:
    std::chrono::high_resolution_clock::time_point time;

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
    RenderUIBroker(Wrap& wrap);
    virtual ~RenderUIBroker();
  };
}