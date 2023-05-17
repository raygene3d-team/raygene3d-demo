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

  class Spark : public Usable
  {
  protected:
    std::shared_ptr<Property> property;
    std::shared_ptr<Device> device;
    std::shared_ptr<View> view;

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

    std::shared_ptr<Property> prop_lightmaps;

    std::shared_ptr<Property> prop_skybox;
    std::shared_ptr<Property> prop_reflection_probe;

  protected:
    std::shared_ptr<Resource> color_target;
    std::shared_ptr<Resource> depth_target;
    std::shared_ptr<Resource> shadow_map;

    std::shared_ptr<Resource> screen_data;
    std::shared_ptr<Resource> camera_data;
    std::shared_ptr<Resource> shadow_data;
    std::shared_ptr<Resource> reflection_probe_data;

    std::shared_ptr<Resource> scene_instances;
    std::shared_ptr<Resource> scene_triangles;
    std::shared_ptr<Resource> scene_vertices0;
    std::shared_ptr<Resource> scene_vertices1;
    std::shared_ptr<Resource> scene_vertices2;
    std::shared_ptr<Resource> scene_vertices3;

    std::shared_ptr<Resource> scene_textures0;
    std::shared_ptr<Resource> scene_textures1;
    std::shared_ptr<Resource> scene_textures2;
    std::shared_ptr<Resource> scene_textures3;

    std::shared_ptr<Resource> light_maps;

    std::shared_ptr<Resource> reflection_probe;

    std::shared_ptr<Resource> skybox_vertices;
    std::shared_ptr<Resource> skybox_triangles;
    std::shared_ptr<Resource> skybox_texture;

    std::shared_ptr<Resource> graphic_arguments;
    std::shared_ptr<Resource> compute_arguments;

  protected:
    std::shared_ptr<Layout> reflection_probe_layout;
    std::shared_ptr<Layout> shadowmap_layout;
    std::shared_ptr<Layout> unshadowed_layout;
    std::shared_ptr<Layout> shadowed_layout;
    std::shared_ptr<Layout> skybox_layout;
    std::shared_ptr<Layout> present_layout;

  protected:
    std::shared_ptr<Config> reflection_probe_configs[7];
    std::shared_ptr<Config> shadowmap_config;
    std::shared_ptr<Config> unshadowed_config;
    std::shared_ptr<Config> shadowed_config;
    std::shared_ptr<Config> skybox_config;
    std::shared_ptr<Config> present_config;

  protected:
    std::shared_ptr<Pass> reflection_probe_passes[7];
    std::shared_ptr<Pass> shadowmap_passes[6];
    std::shared_ptr<Pass> unshadowed_pass;
    std::shared_ptr<Pass> shadowed_pass;
    //std::shared_ptr<Pass> skybox_pass;
    std::shared_ptr<Pass> present_pass;
   

  protected:
    uint32_t shadow_resolution { 1024 };
    uint32_t reflection_probe_size { 1024 };
    glm::f32vec3 light_position{ -0.605f, 3.515f, 0.387f };

  public:
    enum ShadowType
    {
      NO_SHADOWS = 0,
      POINT_SHADOWS = 1,
    };

    enum ShadingSubpass
    {
      SUBPASS_OPAQUE,
      SUBPASS_SKYBOX,
      SUBPASS_MAX_COUNT
    };

  protected:
    ShadowType shadows{ NO_SHADOWS };

  public:
    void SetShadowType(ShadowType shadows) { this->shadows = shadows; }
    ShadowType GetShadowType() const { return shadows; }

  protected:
    std::shared_ptr<Resource> RegisterColorTarget(const std::string& name);
    std::shared_ptr<Resource> RegisterDepthTarget(const std::string& name);
    std::shared_ptr<Resource> RegisterShadowMap(const std::string& name);
    std::shared_ptr<Resource> RegisterReflectionProbe(const std::string& name);

    std::shared_ptr<Resource> RegisterScreenData(const std::string& name);
    std::shared_ptr<Resource> RegisterCameraData(const std::string& name);
    std::shared_ptr<Resource> RegisterShadowData(const std::string& name);
    std::shared_ptr<Resource> RegisterReflectionProbeData(const std::string& name);

    std::shared_ptr<Resource> RegisterSceneInstances(const std::string& name);
    std::shared_ptr<Resource> RegisterSceneTriangles(const std::string& name);
    std::shared_ptr<Resource> RegisterSceneVertices0(const std::string& name);
    std::shared_ptr<Resource> RegisterSceneVertices1(const std::string& name);
    std::shared_ptr<Resource> RegisterSceneVertices2(const std::string& name);
    std::shared_ptr<Resource> RegisterSceneVertices3(const std::string& name);

    std::shared_ptr<Resource> RegisterSceneTextures0(const std::string& name);
    std::shared_ptr<Resource> RegisterSceneTextures1(const std::string& name);
    std::shared_ptr<Resource> RegisterSceneTextures2(const std::string& name);
    std::shared_ptr<Resource> RegisterSceneTextures3(const std::string& name);

    std::shared_ptr<Resource> RegisterLightMaps(const std::string& name);

    std::shared_ptr<Resource> RegisterSkyboxVertices(const std::string& name);
    std::shared_ptr<Resource> RegisterSkyboxTriangles(const std::string& name);
    std::shared_ptr<Resource> RegisterSkyboxTexture(const std::string& name);

    std::shared_ptr<Resource> RegisterGraphicArguments(const std::string& name);
    std::shared_ptr<Resource> RegisterComputeArguments(const std::string& name);

    std::shared_ptr<Layout> RegisterShadowmapLayout(const std::string& name);
    std::shared_ptr<Config> RegisterShadowmapConfig(const std::string& name);
    std::shared_ptr<Pass> RegisterShadowmapPass(const std::string& name, uint32_t index);

    std::shared_ptr<Layout> RegisterReflectionLayout(const std::string& name);
    std::shared_ptr<Config> RegisterReflectionConfig(const std::string& name, const uint32_t& mip_level);
    std::shared_ptr<Pass> RegisterReflectionPass(const std::string& name, const uint32_t& mip_level);

    std::shared_ptr<Layout> RegisterShadowedLayout(const std::string& name);
    std::shared_ptr<Config> RegisterShadowedConfig(const std::string& name);
    std::shared_ptr<Pass> RegisterShadowedPass(const std::string& name);

    std::shared_ptr<Layout> RegisterUnshadowedLayout(const std::string& name);
    std::shared_ptr<Config> RegisterUnshadowedConfig(const std::string& name);
    std::shared_ptr<Pass> RegisterUnshadowedPass(const std::string& name);

    std::shared_ptr<Layout> RegisterSkyboxLayout(const std::string& name);
    std::shared_ptr<Config> RegisterSkyboxConfig(const std::string& name);
    std::shared_ptr<Pass> RegisterSkyboxPass(const std::string& name);

    std::shared_ptr<Layout> RegisterPresentLayout(const std::string& name);
    std::shared_ptr<Config> RegisterPresentConfig(const std::string& name);
    std::shared_ptr<Pass> RegisterPresentPass(const std::string& name);

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    Spark(const std::shared_ptr<Property>& property, const std::shared_ptr<Device>& device, const std::shared_ptr<View>& output);
    virtual ~Spark();
  };
}