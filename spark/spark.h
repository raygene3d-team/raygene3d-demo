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
#include "../raygene3d-wrap/wrap.h"

namespace RayGene3D
{
  class Spark : public Broker
  {
  protected:
    std::shared_ptr<View> backbuffer_uav;
    std::shared_ptr<View> backbuffer_rtv;

  protected:
    std::shared_ptr<Property> prop_scene;
    std::shared_ptr<Property> prop_camera;
    //std::shared_ptr<Property> prop_environment;

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
    std::shared_ptr<Property> prop_vertices;

    //std::shared_ptr<Property> prop_vertices0;
    //std::shared_ptr<Property> prop_vertices1;
    //std::shared_ptr<Property> prop_vertices2;
    //std::shared_ptr<Property> prop_vertices3;

    std::shared_ptr<Property> prop_t_boxes;
    std::shared_ptr<Property> prop_b_boxes;

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

    std::shared_ptr<Resource> gbuffer_0_target; // 8-8-8-8 - albedo / metallic
    std::shared_ptr<Resource> gbuffer_1_target; // 8-8-8-8 - normal / smoothness
    std::shared_ptr<Resource> gbuffer_2_target; // 10-10-10-2 - color (GI+occlusion / material)
    std::shared_ptr<Resource> gbuffer_3_target; // 32 - depth

    std::shared_ptr<Resource> screen_data;
    std::shared_ptr<Resource> camera_data;
    std::shared_ptr<Resource> shadow_data;
    std::shared_ptr<Resource> reflection_probe_data;

    std::shared_ptr<Resource> scene_instances;
    std::shared_ptr<Resource> scene_triangles;
    std::shared_ptr<Resource> scene_vertices;
    //std::shared_ptr<Resource> scene_vertices0;
    //std::shared_ptr<Resource> scene_vertices1;
    //std::shared_ptr<Resource> scene_vertices2;
    //std::shared_ptr<Resource> scene_vertices3;

    std::shared_ptr<Resource> scene_t_boxes;
    std::shared_ptr<Resource> scene_b_boxes;

    //Copies of original resources bacause of DX11 limitations
    std::shared_ptr<Resource> trace_instances;
    std::shared_ptr<Resource> trace_triangles;
    std::shared_ptr<Resource> trace_vertices;

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
    std::shared_ptr<Layout> shadowmap_layout;
    std::shared_ptr<Layout> unshadowed_layout;
    std::shared_ptr<Layout> shadowed_layout;
    std::shared_ptr<Layout> sw_traced_layout;
    std::shared_ptr<Layout> hw_traced_layout;
    std::shared_ptr<Layout> geometry_layout;
    std::shared_ptr<Layout> skybox_layout;
    std::shared_ptr<Layout> present_layout;

  protected:
    std::shared_ptr<Config> reflection_probe_configs[7];
    std::shared_ptr<Config> shadowmap_config;
    std::shared_ptr<Config> unshadowed_config;
    std::shared_ptr<Config> shadowed_config;
    std::shared_ptr<Config> sw_traced_config;
    std::shared_ptr<Config> hw_traced_config;
    std::shared_ptr<Config> geometry_config;
    std::shared_ptr<Config> skybox_config;
    std::shared_ptr<Config> present_config;

  protected:
    std::shared_ptr<Pass> reflection_probe_passes[7];
    std::shared_ptr<Pass> shadowmap_passes[6];
    std::shared_ptr<Pass> unshadowed_pass;
    std::shared_ptr<Pass> shadowed_pass;
    std::shared_ptr<Pass> sw_traced_pass;
    std::shared_ptr<Pass> hw_traced_pass;
    std::shared_ptr<Pass> geometry_pass;
    std::shared_ptr<Pass> skybox_pass;
    std::shared_ptr<Pass> present_pass;
   

  protected:
    uint32_t shadow_resolution { 1024 };
    uint32_t reflection_probe_size { 1024 };
    glm::f32vec3 light_position{ -0.605f, 3.515f, 0.387f };

  public:
    enum ShadowType
    {
      DISABLED_SHADOW = 0,
      CUBEMAP_SHADOW = 1,
      SW_TRACED_SHADOW = 2,
      HW_TRACED_SHADOW = 3,
    };

    enum ShadingSubpass
    {
      SUBPASS_OPAQUE,
      SUBPASS_SKYBOX,
      SUBPASS_MAX_COUNT
    };

  protected:
    ShadowType shadows{ DISABLED_SHADOW };

  public:
    void SetShadowType(ShadowType shadows) { this->shadows = shadows; }
    ShadowType GetShadowType() const { return shadows; }

  protected:
    void CreateColorTarget();
    void CreateShadowMap();

    void CreateGBufferTarget0();
    void CreateGBufferTarget1();
    void CreateGBufferTarget2();
    void CreateGBufferTarget3();

    void CreateScreenData();
    void CreateCameraData();
    void CreateShadowData();

    void CreateSceneInstances();
    void CreateSceneTriangles();
    void CreateSceneVertices();

    void CreateSceneTBoxes();
    void CreateSceneBBoxes();

    void CreateTraceInstances();
    void CreateTraceTriangles();
    void CreateTraceVertices();

    void CreateSceneTextures0();
    void CreateSceneTextures1();
    void CreateSceneTextures2();
    void CreateSceneTextures3();

    void CreateLightMaps();

    void CreateSkyboxVertices();
    void CreateSkyboxTriangles();
    void CreateSkyboxTexture();

    void CreateGraphicArguments();
    void CreateComputeArguments();

    void CreateShadowmapLayout();
    void CreateShadowmapConfig();
    void CreateShadowmapPass(uint32_t index);

    void CreateShadowedLayout();
    void CreateShadowedConfig();
    void CreateShadowedPass();

    void CreateSWTracedLayout();
    void CreateSWTracedConfig();
    void CreateSWTracedPass();

    void CreateHWTracedLayout();
    void CreateHWTracedConfig();
    void CreateHWTracedPass();

    void CreateUnshadowedLayout();
    void CreateUnshadowedConfig();
    void CreateUnshadowedPass();

    void CreateGeometryLayout();
    void CreateGeometryConfig();
    void CreateGeometryPass();

    void CreateSkyboxLayout();
    void CreateSkyboxConfig();
    void CreateSkyboxPass();

    void CreatePresentLayout();
    void CreatePresentConfig();
    void CreatePresentPass();

  protected:
    void DestroyColorTarget();
    void DestroyDepthTarget();
    void DestroyShadowMap();

    void DestroyGBufferTarget0();
    void DestroyGBufferTarget1();
    void DestroyGBufferTarget2();
    void DestroyGBufferTarget3();

    void DestroyScreenData();
    void DestroyCameraData();
    void DestroyShadowData();

    void DestroySceneInstances();
    void DestroySceneTriangles();
    void DestroySceneVertices();

    void DestroySceneTBoxes();
    void DestroySceneBBoxes();

    void DestroyTraceInstances();
    void DestroyTraceTriangles();
    void DestroyTraceVertices();

    void DestroySceneTextures0();
    void DestroySceneTextures1();
    void DestroySceneTextures2();
    void DestroySceneTextures3();

    void DestroyLightMaps();

    void DestroySkyboxVertices();
    void DestroySkyboxTriangles();
    void DestroySkyboxTexture();

    void DestroyGraphicArguments();
    void DestroyComputeArguments();

    void DestroyGeometryLayout();
    void DestroyGeometryConfig();
    void DestroyGeometryPass();

    void DestroyShadowmapLayout();
    void DestroyShadowmapConfig();
    void DestroyShadowmapPass(uint32_t index);

    void DestroyShadowedLayout();
    void DestroyShadowedConfig();
    void DestroyShadowedPass();

    void DestroySWTracedLayout();
    void DestroySWTracedConfig();
    void DestroySWTracedPass();

    void DestroyHWTracedLayout();
    void DestroyHWTracedConfig();
    void DestroyHWTracedPass();

    void DestroyUnshadowedLayout();
    void DestroyUnshadowedConfig();
    void DestroyUnshadowedPass();

    void DestroySkyboxLayout();
    void DestroySkyboxConfig();
    void DestroySkyboxPass();

    void DestroyPresentLayout();
    void DestroyPresentConfig();
    void DestroyPresentPass();

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    Spark(Wrap& wrap);
    virtual ~Spark();
  };
}