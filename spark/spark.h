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


#pragma once
#include "../raygene3d-core/core.h"
#include "../raygene3d-util/property.h"

namespace RayGene3D
{
  class Device;
  class Output;
  class Asset;

  class Spark : public Usable
  {
  protected:
    Core& core;

  public:
    Core& GetCore() { return core; }

  private:
    std::shared_ptr<Property> property;

  private:
    std::shared_ptr<Resource> render_target;
    std::shared_ptr<Resource> depth_stencil;
    std::shared_ptr<Resource> shadow_map;

    std::shared_ptr<Resource> frame_output;

    std::array<Camera, 6> point_light;

    std::shared_ptr<Resource> shadow_data;
    std::shared_ptr<Resource> camera_data;

    std::shared_ptr<Resource> instance_items;
    std::shared_ptr<Resource> triangle_items;
    std::shared_ptr<Resource> vertex_items;

    std::shared_ptr<Resource> texture0_items;
    std::shared_ptr<Resource> texture1_items;
    std::shared_ptr<Resource> texture2_items;
    std::shared_ptr<Resource> texture3_items;

    std::array<glm::f32vec4, 4> environment_vtx;
    std::shared_ptr<Resource> environment_vtx_data;
    std::array<glm::u32vec3, 2> environment_idx;
    std::shared_ptr<Resource> environment_idx_data;
    std::shared_ptr<Resource> environment_item;

  protected:
    std::shared_ptr<Resource> raster_arguments;

    std::array<std::shared_ptr<Pass>, 6> shadow_passes;
    std::shared_ptr<Layout> shadow_layout;
    std::shared_ptr<Config> shadow_shader;

  protected:
    std::shared_ptr<Pass> no_shadow_raster_pass;
    std::shared_ptr<Layout> no_shadow_raster_layout;
    std::shared_ptr<Config> no_shadow_raster_shader;

    std::shared_ptr<Pass> shadow_map_raster_pass;
    std::shared_ptr<Layout> shadow_map_raster_layout;
    std::shared_ptr<Config> shadow_map_raster_shader;

  protected:
    std::shared_ptr<Pass> environment_pass;
    std::shared_ptr<Layout> environment_layout;
    std::shared_ptr<Config> environment_shader;
    //std::shared_ptr<Resource> environment_arguments;

  protected:
    std::shared_ptr<Pass> present_pass;
    std::shared_ptr<Layout> present_layout;
    std::shared_ptr<Config> present_shader;
    std::shared_ptr<Resource> present_arguments;

  protected:
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


  protected:
    uint32_t shadow_map_size { 1024 };

  public:
    enum ShadowType
    {
      NO_SHADOWS = 0,
      POINT_SHADOWS = 1,
    };

  protected:
    ShadowType shadows{ NO_SHADOWS };

  public:
    void SetShadowType(ShadowType shadows) { this->shadows = shadows; }
    ShadowType GetShadowType() const { return shadows; }

  public:
    std::shared_ptr<Property>& AccessProperty() { return property; }

  protected:
    void InitializeResources();
    void InitializeShadow();
    void InitializeSimple();
    void InitializeAdvanced();
    void InitializeEnvironment();
    void InitializePresent();

    void DiscardRaster();
    void DiscardTracer();
    void DiscardPresent();
    void DiscardResources();

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    Spark(Core& core);
    virtual ~Spark();
  };
}