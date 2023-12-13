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
    SPtrView backbuffer_uav;
    SPtrView backbuffer_rtv;

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
    SPtrResource color_target; // 10-10-10-2 - color (GI+occlusion / material)
    SPtrResource depth_target; // 32 - depth
    SPtrResource shadow_map;

    SPtrResource gbuffer_0_target; // 8-8-8-8 - albedo / metallic
    SPtrResource gbuffer_1_target; // 8-8-8-8 - normal / smoothness

    SPtrResource screen_data;
    SPtrResource camera_data;
    SPtrResource shadow_data;
    SPtrResource reflection_probe_data;

    SPtrResource scene_instances;
    SPtrResource scene_triangles;
    SPtrResource scene_vertices;
    //SPtrResource scene_vertices0;
    //SPtrResource scene_vertices1;
    //SPtrResource scene_vertices2;
    //SPtrResource scene_vertices3;

    SPtrResource scene_t_boxes;
    SPtrResource scene_b_boxes;

    //Copies of original resources bacause of DX11 limitations
    SPtrResource trace_instances;
    SPtrResource trace_triangles;
    SPtrResource trace_vertices;

    SPtrResource scene_textures0;
    SPtrResource scene_textures1;
    SPtrResource scene_textures2;
    SPtrResource scene_textures3;

    SPtrResource light_maps;

    SPtrResource reflection_probe;

    SPtrResource screen_quad_vertices;
    SPtrResource screen_quad_triangles;

    SPtrResource skybox_texture;

    SPtrResource graphic_arguments;
    SPtrResource compute_arguments;

  protected:
    SPtrBatch shadowmap_batches[6];
    SPtrBatch unshadowed_batch;
    SPtrBatch shadowed_batch;
    SPtrBatch sw_traced_batch;
    SPtrBatch hw_traced_batch;
    SPtrBatch geometry_batch;
    SPtrBatch skybox_batch;
    SPtrBatch present_batch;

  protected:
    SPtrTechnique reflection_probe_techniques[7];
    SPtrTechnique shadowmap_techniques[6];
    SPtrTechnique unshadowed_technique;
    SPtrTechnique shadowed_technique;
    SPtrTechnique sw_traced_technique;
    SPtrTechnique hw_traced_technique;
    SPtrTechnique geometry_technique;
    SPtrTechnique skybox_technique;
    SPtrTechnique present_technique;

  protected:
    SPtrPass reflection_probe_passes[7];
    SPtrPass shadowmap_passes[6];
    SPtrPass unshadowed_pass;
    SPtrPass shadowed_pass;
    SPtrPass sw_traced_pass;
    SPtrPass hw_traced_pass;
    SPtrPass geometry_pass;
    SPtrPass present_pass;
   

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
    bool use_normal_oct_quad_encoding{ false };

  protected:
    void CreateColorTarget();
    void CreateDepthTarget();
    void CreateShadowMap();

    void CreateGBufferTarget0();
    void CreateGBufferTarget1();

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

    void CreateScreenQuadVertices();
    void CreateScreenQuadTriangles();
    void CreateSkyboxTexture();

    void CreateGraphicArguments();
    void CreateComputeArguments();

    void CreateShadowmapPass(uint32_t index);
    void CreateShadowmapTechnique(uint32_t index);
    void CreateShadowmapBatch(uint32_t index);
    
    void CreateShadowedPass();
    void CreateShadowedTechnique();
    void CreateShadowedBatch();
    
    void CreateSWTracedPass();
    void CreateSWTracedTechnique();
    void CreateSWTracedBatch();
    
    void CreateHWTracedPass();
    void CreateHWTracedTechnique();
    void CreateHWTracedBatch();
    
    void CreateUnshadowedPass();
    void CreateUnshadowedTechnique();
    void CreateUnshadowedBatch();
   
    void CreateGeometryPass();
    void CreateGeometryTechnique();
    void CreateGeometryBatch();
    
    void CreateSkyboxTechnique();
    void CreateSkyboxBatch();
    
    void CreatePresentPass();
    void CreatePresentTechnique();
    void CreatePresentBatch();
    
  protected:
    void DestroyColorTarget();
    void DestroyDepthTarget();
    void DestroyShadowMap();

    void DestroyGBufferTarget0();
    void DestroyGBufferTarget1();

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

    void DestroyScreenQuadVertices();
    void DestroyScreenQuadTriangles();
    void DestroySkyboxTexture();

    void DestroyGraphicArguments();
    void DestroyComputeArguments();

    void DestroyGeometryBatch();
    void DestroyGeometryTechnique();
    void DestroyGeometryPass();

    void DestroyShadowmapBatch(uint32_t index);
    void DestroyShadowmapTechnique(uint32_t index);
    void DestroyShadowmapPass(uint32_t index);

    void DestroyShadowedBatch();
    void DestroyShadowedTechnique();
    void DestroyShadowedPass();

    void DestroySWTracedBatch();
    void DestroySWTracedTechnique();
    void DestroySWTracedPass();

    void DestroyHWTracedBatch();
    void DestroyHWTracedTechnique();
    void DestroyHWTracedPass();

    void DestroyUnshadowedBatch();
    void DestroyUnshadowedTechnique();
    void DestroyUnshadowedPass();

    void DestroySkyboxBatch();
    void DestroySkyboxTechnique();

    void DestroyPresentBatch();
    void DestroyPresentTechnique();
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