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
    std::shared_ptr<Property> root_property;

  private:
    std::shared_ptr<View> output_view;

  private:
    std::shared_ptr<Resource> render_target;
    std::shared_ptr<Resource> depth_stencil;
    std::shared_ptr<Resource> shadow_map;

  private:
    std::shared_ptr<Resource> screen_data;
    std::shared_ptr<Resource> camera_data;
    std::shared_ptr<Resource> shadow_data;

  private:
    std::shared_ptr<Resource> instance_items;
    std::shared_ptr<Resource> triangle_items;
    std::shared_ptr<Resource> vertex0_items;
    std::shared_ptr<Resource> vertex1_items;
    std::shared_ptr<Resource> vertex2_items;
    std::shared_ptr<Resource> vertex3_items;

  private:
    std::shared_ptr<Resource> texture0_items;
    std::shared_ptr<Resource> texture1_items;
    std::shared_ptr<Resource> texture2_items;
    std::shared_ptr<Resource> texture3_items;

    std::shared_ptr<Resource> texture4_items;

    std::array<glm::f32vec4, 4> environment_vtx = {
      glm::f32vec4(-1.0f, 1.0f, 0.0f, 0.0f),
      glm::f32vec4( 1.0f, 1.0f, 1.0f, 0.0f),
      glm::f32vec4(-1.0f,-1.0f, 0.0f, 1.0f),
      glm::f32vec4( 1.0f,-1.0f, 1.0f, 1.0f),
    };
    std::array<glm::u32vec3, 2> environment_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    std::shared_ptr<Resource> environment_vtx_data;
    std::shared_ptr<Resource> environment_idx_data;
    std::shared_ptr<Resource> environment_item;

  private:
    std::shared_ptr<Resource> raster_arguments;
    std::shared_ptr<Resource> compute_arguments;

  protected:
    std::array<std::shared_ptr<Pass>, 6> shadowmap_passes;
    std::shared_ptr<Layout> shadowmap_layout;
    std::shared_ptr<Config> shadowmap_config;

  protected:
    std::shared_ptr<Pass> unshadowed_pass;
    std::shared_ptr<Layout> unshadowed_layout;
    std::shared_ptr<Config> unshadowed_config;

  protected:
    std::shared_ptr<Pass> shadowed_pass;
    std::shared_ptr<Layout> shadowed_layout;
    std::shared_ptr<Config> shadowed_config;

  protected:
    std::shared_ptr<Pass> environment_pass;
    std::shared_ptr<Layout> environment_layout;
    std::shared_ptr<Config> environment_config;

  protected:
    std::shared_ptr<Pass> present_pass;
    std::shared_ptr<Layout> present_layout;
    std::shared_ptr<Config> present_config;
    

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

    std::shared_ptr<Property> prop_instances;
    std::shared_ptr<Property> prop_triangles;

    std::shared_ptr<Property> prop_vertices0;
    std::shared_ptr<Property> prop_vertices1;
    std::shared_ptr<Property> prop_vertices2;
    std::shared_ptr<Property> prop_vertices3;

    std::shared_ptr<Property> prop_textures0;
    std::shared_ptr<Property> prop_textures1;
    std::shared_ptr<Property> prop_textures2;
    std::shared_ptr<Property> prop_textures3;

    std::shared_ptr<Property> prop_textures4;

  protected:
    uint32_t shadow_resolution { 1024 };
    glm::f32vec3 light_position{ -0.605f, 3.515f, 0.387f };

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
    std::shared_ptr<Property>& AccessRootProperty() { return root_property; }

  public:
    std::shared_ptr<View>& AccessOutputView() { return output_view; }


  protected:
    void CreateResources(const std::shared_ptr<Device>& device);
    void CreatePasses(const std::shared_ptr<Device>& device);
    void InitializeResources();
    void InitializePasses();
    void DiscardResources();
    void DiscardPasses();
    void DestroyResources(const std::shared_ptr<Device>& device);
    void DestroyPasses(const std::shared_ptr<Device>& device);

  protected:
    void CreateAttachments(const std::shared_ptr<Device>& device);
    void CreateConstants(const std::shared_ptr<Device>& device);
    void CreateGeometries(const std::shared_ptr<Device>& device);
    void CreateTextures(const std::shared_ptr<Device>& device);
    void CreateArguments(const std::shared_ptr<Device>& device);

    void CreateShadowmap(const std::shared_ptr<Device>& device);
    void CreateUnshadowed(const std::shared_ptr<Device>& device);
    void CreateShadowed(const std::shared_ptr<Device>& device);
    void CreateEnvironment(const std::shared_ptr<Device>& device);
    void CreatePresent(const std::shared_ptr<Device>& device);


    //void InitializeShadow();
    //void InitializeSimple();
    //void InitializeAdvanced();
    //void InitializeEnvironment();
    //void InitializePresent();

    //void DiscardRaster();
    //void DiscardTracer();
    //void DiscardPresent();
    //void DiscardResources();

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    Spark(Core& core);
    virtual ~Spark();
  };
}