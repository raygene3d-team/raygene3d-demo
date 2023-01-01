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
  private:
    std::shared_ptr<Property> property;
    std::shared_ptr<Device> device;
    std::shared_ptr<View> view;

  private:
    std::shared_ptr<Resource> color_target;
    std::shared_ptr<Resource> depth_target;
    std::shared_ptr<Resource> shadow_map;

  private:
    std::shared_ptr<Resource> screen_data;
    std::shared_ptr<Resource> camera_data;
    std::shared_ptr<Resource> shadow_data;

  private:
    std::shared_ptr<Resource> scene_instances;
    std::shared_ptr<Resource> scene_triangles;
    std::shared_ptr<Resource> scene_vertices0;
    std::shared_ptr<Resource> scene_vertices1;
    std::shared_ptr<Resource> scene_vertices2;
    std::shared_ptr<Resource> scene_vertices3;

  private:
    std::shared_ptr<Resource> scene_textures0;
    std::shared_ptr<Resource> scene_textures1;
    std::shared_ptr<Resource> scene_textures2;
    std::shared_ptr<Resource> scene_textures3;

    std::shared_ptr<Resource> light_maps;

    std::array<glm::f32vec4, 4> quad_vtx = {
      glm::f32vec4(-1.0f, 1.0f, 0.0f, 0.0f),
      glm::f32vec4( 1.0f, 1.0f, 1.0f, 0.0f),
      glm::f32vec4(-1.0f,-1.0f, 0.0f, 1.0f),
      glm::f32vec4( 1.0f,-1.0f, 1.0f, 1.0f),
    };
    std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    std::shared_ptr<Resource> skybox_vertices;
    std::shared_ptr<Resource> skybox_triangles;
    std::shared_ptr<Resource> skybox_texture;

  private:
    std::shared_ptr<Resource> graphic_arguments;
    std::shared_ptr<Resource> compute_arguments;

  protected:
    std::shared_ptr<Pass> shadowmap_passes[6];
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
    std::shared_ptr<Pass> skybox_pass;
    std::shared_ptr<Layout> skybox_layout;
    std::shared_ptr<Config> skybox_config;

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

    std::shared_ptr<Property> prop_lightmaps;

    std::shared_ptr<Property> prop_skybox;

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

  protected:
  //  void CreateResources();
  //  void CreatePasses();
  //  void InitializeResources();
  //  void InitializePasses();
  //  void DiscardResources();
  //  void DiscardPasses();
  //  void DestroyResources();
  //  void DestroyPasses();

  //protected:
  //  void CreateAttachments();
  //  void CreateConstants();
  //  void CreateGeometries();
  //  void CreateTextures();
  //  void CreateArguments();

    std::shared_ptr<Resource> ConfigureColorTarget(const std::string& name);
    std::shared_ptr<Resource> ConfigureDepthTarget(const std::string& name);
    std::shared_ptr<Resource> ConfigureShadowMap(const std::string& name);

    std::shared_ptr<Resource> ConfigureScreenData(const std::string& name);
    std::shared_ptr<Resource> ConfigureCameraData(const std::string& name);
    std::shared_ptr<Resource> ConfigureShadowData(const std::string& name);

    std::shared_ptr<Resource> ConfigureSceneInstances(const std::string& name);
    std::shared_ptr<Resource> ConfigureSceneTriangles(const std::string& name);
    std::shared_ptr<Resource> ConfigureSceneVertices0(const std::string& name);
    std::shared_ptr<Resource> ConfigureSceneVertices1(const std::string& name);
    std::shared_ptr<Resource> ConfigureSceneVertices2(const std::string& name);
    std::shared_ptr<Resource> ConfigureSceneVertices3(const std::string& name);

    std::shared_ptr<Resource> ConfigureSceneTextures0(const std::string& name);
    std::shared_ptr<Resource> ConfigureSceneTextures1(const std::string& name);
    std::shared_ptr<Resource> ConfigureSceneTextures2(const std::string& name);
    std::shared_ptr<Resource> ConfigureSceneTextures3(const std::string& name);

    std::shared_ptr<Resource> ConfigureLightMaps(const std::string& name);

    std::shared_ptr<Resource> ConfigureSkyboxVertices(const std::string& name);
    std::shared_ptr<Resource> ConfigureSkyboxTriangles(const std::string& name);
    std::shared_ptr<Resource> ConfigureSkyboxTexture(const std::string& name);

    std::shared_ptr<Resource> ConfigureGraphicArguments(const std::string& name);
    std::shared_ptr<Resource> ConfigureComputeArguments(const std::string& name);

    std::shared_ptr<Layout> ConfigureShadowmapLayout(const std::string& name);
    std::shared_ptr<Config> ConfigureShadowmapConfig(const std::string& name);
    std::shared_ptr<Pass> ConfigureShadowmapPass(const std::string& name, uint32_t index);

    std::shared_ptr<Layout> ConfigureShadowedLayout(const std::string& name);
    std::shared_ptr<Config> ConfigureShadowedConfig(const std::string& name);
    std::shared_ptr<Pass> ConfigureShadowedPass(const std::string& name);

    std::shared_ptr<Layout> ConfigureUnshadowedLayout(const std::string& name);
    std::shared_ptr<Config> ConfigureUnshadowedConfig(const std::string& name);
    std::shared_ptr<Pass> ConfigureUnshadowedPass(const std::string& name);

    std::shared_ptr<Layout> ConfigureSkyboxLayout(const std::string& name);
    std::shared_ptr<Config> ConfigureSkyboxConfig(const std::string& name);
    std::shared_ptr<Pass> ConfigureSkyboxPass(const std::string& name);

    std::shared_ptr<Layout> ConfigurePresentLayout(const std::string& name);
    std::shared_ptr<Config> ConfigurePresentConfig(const std::string& name);
    std::shared_ptr<Pass> ConfigurePresentPass(const std::string& name);

    //void CreateShadowmap();
    //void CreateUnshadowed();
    //void CreateShadowed();
    //void CreateEnvironment();
    //void CreatePresent();

    //void CreateResources();
    //void CreateLayouts();
    //void CreateConfigs();
    //void CreatePasses();





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
    Spark(const std::shared_ptr<Property>& property, const std::shared_ptr<Device>& device, const std::shared_ptr<View>& output);
    virtual ~Spark();
  };
}