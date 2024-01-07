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


#include "render_3d_broker.h"

namespace RayGene3D
{
  void Render3DBroker::CreateShadowMap()
  {
    scope.shadow_map = wrap.GetCore()->GetDevice()->CreateResource("spark_shadow_map",
      Resource::Tex2DDesc
      {
        Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE),
        1,
        6,
        FORMAT_D16_UNORM,
        shadow_resolution,
        shadow_resolution,        
      },
      Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE)
    );
  }

  void Render3DBroker::CreateColorTarget()
  {
    scope.color_target = wrap.GetCore()->GetDevice()->CreateResource("spark_color_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE | USAGE_UNORDERED_ACCESS),
        1,
        1,
        FORMAT_R11G11B10_FLOAT,
        scope.prop_extent_x->GetUint(),
        scope.prop_extent_y->GetUint(),
      }
    );
  }

  void Render3DBroker::CreateDepthTarget()
  {
    scope.depth_target = wrap.GetCore()->GetDevice()->CreateResource("spark_depth_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_D32_FLOAT,
        scope.prop_extent_x->GetUint(),
        scope.prop_extent_y->GetUint(),
      }
    );
  }

  void Render3DBroker::CreateGBufferTarget0()
  {
    scope.gbuffer_0_target = wrap.GetCore()->GetDevice()->CreateResource("spark_gbuffer_0_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_R8G8B8A8_UNORM,
        scope.prop_extent_x->GetUint(),
        scope.prop_extent_y->GetUint(),
      }
    );
  }

  void Render3DBroker::CreateGBufferTarget1()
  {
    scope.gbuffer_1_target = wrap.GetCore()->GetDevice()->CreateResource("spark_gbuffer_1_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_R8G8B8A8_UNORM,
        scope.prop_extent_x->GetUint(),
        scope.prop_extent_y->GetUint(),
      }
    );
  }

  void Render3DBroker::CreateScreenData()
  {
    scope.screen_data = wrap.GetCore()->GetDevice()->CreateResource("spark_screen_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Screen)),
        1,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );
  }

  void Render3DBroker::CreateCameraData()
  {
    scope.camera_data = wrap.GetCore()->GetDevice()->CreateResource("spark_camera_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Frustum)),
        1,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );
  }

  void Render3DBroker::CreateShadowData()
  {
    scope.shadow_data = wrap.GetCore()->GetDevice()->CreateResource("spark_shadow_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Frustum)),
        6,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );
  }

  void Render3DBroker::CreateSceneInstances()
  {
    const auto [data, count] = scope.prop_instances->GetTypedBytes<Instance>(0);
    std::pair<const void*, uint32_t> interops[] = {
      scope.prop_instances->GetRawBytes(0),
    };

    scope.scene_instances = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_instances",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Instance)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateSceneTriangles()
  {
    const auto [data, count] = scope.prop_triangles->GetTypedBytes<Triangle>(0);
    std::pair<const void*, uint32_t> interops[] = {
      scope.prop_triangles->GetRawBytes(0),
    };

    scope.scene_triangles = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_triangles",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        uint32_t(sizeof(Triangle)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateSceneVertices()
  {
    const auto [data, count] = scope.prop_vertices->GetTypedBytes<Vertex>(0);
    std::pair<const void*, uint32_t> interops[] = {
      scope.prop_vertices->GetRawBytes(0),
    };

    scope.scene_vertices = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_vertices",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        uint32_t(sizeof(Vertex)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateSceneTBoxes()
  {
    const auto [data, count] = scope.prop_t_boxes->GetTypedBytes<Box>(0);
    std::pair<const void*, uint32_t> interops[] = {
      scope.prop_t_boxes->GetRawBytes(0),
    };

    scope.scene_t_boxes = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_t_boxes",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        uint32_t(sizeof(Box)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateSceneBBoxes()
  {
    const auto [data, count] = scope.prop_b_boxes->GetTypedBytes<Box>(0);
    std::pair<const void*, uint32_t> interops[] = {
      scope.prop_b_boxes->GetRawBytes(0),
    };

    scope.scene_b_boxes = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_b_boxes",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        uint32_t(sizeof(Box)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateTraceInstances()
  {
    const auto [data, count] = scope.prop_instances->GetTypedBytes<Instance>(0);
    std::pair<const void*, uint32_t> interops[] = {
      scope.prop_instances->GetRawBytes(0),
    };

    scope.trace_instances = wrap.GetCore()->GetDevice()->CreateResource("spark_trace_instances",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        uint32_t(sizeof(Instance)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateTraceTriangles()
  {
    const auto [data, count] = scope.prop_triangles->GetTypedBytes<Triangle>(0);
    std::pair<const void*, uint32_t> interops[] = {
      scope.prop_triangles->GetRawBytes(0),
    };

    scope.trace_triangles = wrap.GetCore()->GetDevice()->CreateResource("spark_trace_triangles",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE | USAGE_RAYTRACING_INPUT),
        uint32_t(sizeof(Triangle)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_ADDRESS_BUFFER),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateTraceVertices()
  {
    const auto [data, count] = scope.prop_vertices->GetTypedBytes<Vertex>(0);
    std::pair<const void*, uint32_t> interops[] = {
      scope.prop_vertices->GetRawBytes(0),
    };

    scope.trace_vertices = wrap.GetCore()->GetDevice()->CreateResource("spark_trace_vertices",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE | USAGE_RAYTRACING_INPUT),
        uint32_t(sizeof(Vertex)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_ADDRESS_BUFFER),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateSceneTextures0()
  {
    const auto layers = scope.prop_textures0->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != scope.prop_textures0->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < scope.prop_textures0->GetArraySize(); ++i)
    {
      interops.emplace_back(scope.prop_textures0->GetArrayItem(i)->GetRawBytes(0));
    }

    scope.scene_textures0 = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_textures0",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DBroker::CreateSceneTextures1()
  {
    const auto layers = scope.prop_textures1->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != scope.prop_textures1->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < scope.prop_textures1->GetArraySize(); ++i)
    {
      interops.emplace_back(scope.prop_textures1->GetArrayItem(i)->GetRawBytes(0));
    }

    scope.scene_textures1 = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_textures1",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DBroker::CreateSceneTextures2()
  {
    const auto layers = scope.prop_textures2->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != scope.prop_textures2->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < scope.prop_textures2->GetArraySize(); ++i)
    {
      interops.emplace_back(scope.prop_textures2->GetArrayItem(i)->GetRawBytes(0));
    }

    scope.scene_textures2 = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_textures2",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DBroker::CreateSceneTextures3()
  {
    const auto layers = scope.prop_textures3->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != scope.prop_textures3->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < scope.prop_textures3->GetArraySize(); ++i)
    {
      interops.emplace_back(scope.prop_textures3->GetArrayItem(i)->GetRawBytes(0));
    }

    scope.scene_textures3 = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_textures3",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DBroker::CreateLightMaps()
  {
    const auto layers = scope.prop_lightmaps->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != scope.prop_lightmaps->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < scope.prop_lightmaps->GetArraySize(); ++i)
    {
      interops.emplace_back(scope.prop_lightmaps->GetArrayItem(i)->GetRawBytes(0));
    }

    scope.light_maps = wrap.GetCore()->GetDevice()->CreateResource("spark_light_maps",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DBroker::CreateScreenQuadVertices()
  {
    static const std::array<glm::f32vec2, 4> quad_vtx = {
      glm::f32vec2(-1.0f, 1.0f),
      glm::f32vec2(1.0f, 1.0f),
      glm::f32vec2(-1.0f,-1.0f),
      glm::f32vec2(1.0f,-1.0f),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_vtx.data(), uint32_t(quad_vtx.size() * sizeof(glm::f32vec2)) },
    };

    scope.screen_quad_vertices = wrap.GetCore()->GetDevice()->CreateResource("spark_screen_quad_vertices",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        uint32_t(sizeof(glm::f32vec2)),
        uint32_t(quad_vtx.size()),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateScreenQuadTriangles()
  {
    static const std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_idx.data(), uint32_t(quad_idx.size() * sizeof(glm::u32vec3)) },
    };

    scope.screen_quad_triangles = wrap.GetCore()->GetDevice()->CreateResource("spark_screen_quad_triangles",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        uint32_t(sizeof(glm::u32vec3)),
        uint32_t(quad_idx.size()),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DBroker::CreateSkyboxTexture()
  {
    const auto layers = scope.prop_skybox->GetArraySize();
    const auto format = FORMAT_R32G32B32A32_FLOAT;
    const auto bpp = 16u;

    auto mipmaps = 1u;
    auto extent_x = 2u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != scope.prop_skybox->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < scope.prop_skybox->GetArraySize(); ++i)
    {
      interops.emplace_back(scope.prop_skybox->GetArrayItem(i)->GetRawBytes(0));
    }

    scope.skybox_texture = wrap.GetCore()->GetDevice()->CreateResource("spark_skybox_texture",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_UNKNOWN),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DBroker::CreateGraphicArguments()
  {
    const auto [data, count] = scope.prop_instances->GetTypedBytes<Instance>(0);

    scope.graphic_arguments = wrap.GetCore()->GetDevice()->CreateResource("spark_graphic_arguments",
      Resource::BufferDesc
      {
        Usage(USAGE_ARGUMENT_INDIRECT),
        uint32_t(sizeof(Batch::Graphic)),
        uint32_t(count),
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );
  }

  void Render3DBroker::CreateComputeArguments()
  {
    scope.compute_arguments = wrap.GetCore()->GetDevice()->CreateResource("spark_compute_arguments",
      Resource::BufferDesc
      {
        Usage(USAGE_ARGUMENT_INDIRECT),
        uint32_t(sizeof(Batch::Compute)),
        1u,
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );
  }












  void Render3DBroker::Initialize()
  {
  }

  void Render3DBroker::DestroyColorTarget()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(color_target);
    color_target.reset();
  }

  void Render3DBroker::DestroyDepthTarget()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(depth_target);
    depth_target.reset();
  }

  void Render3DBroker::DestroyShadowMap()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(shadow_map);
    shadow_map.reset();
  }

  void Render3DBroker::DestroyGBufferTarget0()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(gbuffer_0_target);
    gbuffer_0_target.reset();
  }

  void Render3DBroker::DestroyGBufferTarget1()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(gbuffer_1_target);
    gbuffer_1_target.reset();
  }

  void Render3DBroker::DestroyScreenData()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(screen_data);
    screen_data.reset();
  }

  void Render3DBroker::DestroyCameraData()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(camera_data);
    camera_data.reset();
  }

  void Render3DBroker::DestroyShadowData()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(shadow_data);
    shadow_data.reset();
  }

  void Render3DBroker::DestroySceneInstances()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_instances);
    scene_instances.reset();
  }

  void Render3DBroker::DestroySceneTriangles()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_triangles);
    scene_triangles.reset();
  }

  void Render3DBroker::DestroySceneVertices()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_vertices);
    scene_vertices.reset();
  }

  void Render3DBroker::DestroySceneTBoxes()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_t_boxes);
    scene_t_boxes.reset();
  }

  void Render3DBroker::DestroySceneBBoxes()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_b_boxes);
    scene_b_boxes.reset();
  }

  void Render3DBroker::DestroyTraceInstances()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_instances);
    trace_instances.reset();
  }

  void Render3DBroker::DestroyTraceTriangles()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_triangles);
    trace_triangles.reset();
  }

  void Render3DBroker::DestroyTraceVertices()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_vertices);
    trace_vertices.reset();
  }
  
  void Render3DBroker::DestroySceneTextures0()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures0);
    scene_textures0.reset();
  }

  void Render3DBroker::DestroySceneTextures1()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures1);
    scene_textures1.reset();
  }

  void Render3DBroker::DestroySceneTextures2()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures2);
    scene_textures2.reset();
  }

  void Render3DBroker::DestroySceneTextures3()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures3);
    scene_textures3.reset();
  }

  void Render3DBroker::DestroyLightMaps()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(light_maps);
    light_maps.reset();
  }

  void Render3DBroker::DestroyScreenQuadVertices()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(screen_quad_vertices);
    screen_quad_vertices.reset();
  }

  void Render3DBroker::DestroyScreenQuadTriangles()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(screen_quad_triangles);
    screen_quad_triangles.reset();
  }

  void Render3DBroker::DestroySkyboxTexture()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(skybox_texture);
    skybox_texture.reset();
  }

  void Render3DBroker::DestroyGraphicArguments()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(graphic_arguments);
    graphic_arguments.reset();
  }

  void Render3DBroker::DestroyComputeArguments()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(compute_arguments);
    compute_arguments.reset();
  }



  void Render3DBroker::Use()
  {
    switch (shadows)
    {
    case DISABLED_SHADOW:
    {
      shadowmap_passes[0]->SetEnabled(false);
      shadowmap_passes[1]->SetEnabled(false);
      shadowmap_passes[2]->SetEnabled(false);
      shadowmap_passes[3]->SetEnabled(false);
      shadowmap_passes[4]->SetEnabled(false);
      shadowmap_passes[5]->SetEnabled(false);
      geometry_pass->SetEnabled(true);
      unshadowed_pass->SetEnabled(true);
      shadowed_pass->SetEnabled(false);
      sw_traced_pass->SetEnabled(false);
      hw_traced_pass->SetEnabled(false);
      present_pass->SetEnabled(true);
      break;
    }
    case CUBEMAP_SHADOW:
    {
      shadowmap_passes[0]->SetEnabled(true);
      shadowmap_passes[1]->SetEnabled(true);
      shadowmap_passes[2]->SetEnabled(true);
      shadowmap_passes[3]->SetEnabled(true);
      shadowmap_passes[4]->SetEnabled(true);
      shadowmap_passes[5]->SetEnabled(true);
      geometry_pass->SetEnabled(true);
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(true);
      sw_traced_pass->SetEnabled(false);
      hw_traced_pass->SetEnabled(false);
      present_pass->SetEnabled(true);
      break;
    }
    case SW_TRACED_SHADOW:
    {
      shadowmap_passes[0]->SetEnabled(false);
      shadowmap_passes[1]->SetEnabled(false);
      shadowmap_passes[2]->SetEnabled(false);
      shadowmap_passes[3]->SetEnabled(false);
      shadowmap_passes[4]->SetEnabled(false);
      shadowmap_passes[5]->SetEnabled(false);
      geometry_pass->SetEnabled(true);
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(false);
      sw_traced_pass->SetEnabled(true);
      hw_traced_pass->SetEnabled(false);
      present_pass->SetEnabled(true);
      break;
    }
    case HW_TRACED_SHADOW:
    {
      shadowmap_passes[0]->SetEnabled(false);
      shadowmap_passes[1]->SetEnabled(false);
      shadowmap_passes[2]->SetEnabled(false);
      shadowmap_passes[3]->SetEnabled(false);
      shadowmap_passes[4]->SetEnabled(false);
      shadowmap_passes[5]->SetEnabled(false);
      geometry_pass->SetEnabled(true);
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(false);
      sw_traced_pass->SetEnabled(false);
      hw_traced_pass->SetEnabled(true);
      present_pass->SetEnabled(true);
      break;
    }
    default:
    {
      shadowmap_passes[0]->SetEnabled(false);
      shadowmap_passes[1]->SetEnabled(false);
      shadowmap_passes[2]->SetEnabled(false);
      shadowmap_passes[3]->SetEnabled(false);
      shadowmap_passes[4]->SetEnabled(false);
      shadowmap_passes[5]->SetEnabled(false);
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(false);
      sw_traced_pass->SetEnabled(false);
      hw_traced_pass->SetEnabled(false);
      present_pass->SetEnabled(false);
      break;
    }
    }

    {
      auto graphic_arg = reinterpret_cast<Batch::Graphic*>(graphic_arguments->Map());

      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      for (uint32_t i = 0; i < instance_count; ++i)
      {
        graphic_arg[i].idx_count = instance_data[i].prim_count * 3u;
        graphic_arg[i].ins_count = 1u;
        graphic_arg[i].idx_offset = instance_data[i].prim_offset * 3u;
        graphic_arg[i].vtx_offset = instance_data[i].vert_offset * 1u;
        graphic_arg[i].ins_offset = 0u;
      }

      graphic_arguments->Unmap();
    }

    {
      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();

      auto compute_arg = reinterpret_cast<Batch::Compute*>(compute_arguments->Map());
      {
        compute_arg[0].grid_x = extent_x / 8u;
        compute_arg[0].grid_y = extent_y / 8u;
        compute_arg[0].grid_z = 1u;
      }
      compute_arguments->Unmap();
    }

    {
      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();

      const auto counter = prop_counter->GetUint();

      Screen screen;
      screen.extent_x = extent_x;
      screen.extent_y = extent_y;
      screen.rnd_base = counter;
      screen.rnd_seed = rand();

      auto screen_mapped = screen_data->Map();
      memcpy(screen_mapped, &screen, sizeof(Screen));
      screen_data->Unmap();
    }

    {
      const auto eye = glm::f32vec3{
        prop_eye->GetArrayItem(0)->GetReal(),
        prop_eye->GetArrayItem(1)->GetReal(),
        prop_eye->GetArrayItem(2)->GetReal()
      };

      const auto lookat = glm::f32vec3{
        prop_lookat->GetArrayItem(0)->GetReal(),
        prop_lookat->GetArrayItem(1)->GetReal(),
        prop_lookat->GetArrayItem(2)->GetReal()
      };

      const auto up = glm::f32vec3{
        prop_up->GetArrayItem(0)->GetReal(),
        prop_up->GetArrayItem(1)->GetReal(),
        prop_up->GetArrayItem(2)->GetReal()
      };

      const auto fov_x = prop_fov_x->GetReal();
      const auto fov_y = prop_fov_y->GetReal();

      const auto n_plane = prop_n_plane->GetReal();
      const auto f_plane = prop_f_plane->GetReal();

      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();

      Frustum camera_frustum;
      camera_frustum.proj = glm::perspective(glm::radians(fov_y), float(extent_x) / float(extent_y), n_plane, f_plane);
      camera_frustum.proj_inv = glm::inverse(camera_frustum.proj);
      camera_frustum.view = glm::lookAt(eye, lookat, up);
      camera_frustum.view_inv = glm::inverse(camera_frustum.view);

      auto camera_mapped = camera_data->Map();
      memcpy(camera_mapped, &camera_frustum, sizeof(Frustum));
      camera_data->Unmap();
    }

    {
      const auto proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 100.0f);
      const auto proj_inv = glm::inverse(proj);

      const auto x = glm::f32vec3{ 1.0f, 0.0f, 0.0f };
      const auto y = glm::f32vec3{ 0.0f, 1.0f, 0.0f };
      const auto z = glm::f32vec3{ 0.0f, 0.0f, 1.0f };

      Frustum shadow_frustums[6];
      for (uint32_t i = 0; i < 6; ++i)
      {
        auto view = glm::f32mat4x4(1.0);

        switch (i)
        {
        case 0:
          view = glm::lookAt(light_position, light_position + x, y); break;
        case 1:
          view = glm::lookAt(light_position, light_position - x, y); break;
        case 2:
          view = glm::lookAt(light_position, light_position + y, -z); break;
        case 3:
          view = glm::lookAt(light_position, light_position - y, z); break;
        case 4:
          view = glm::lookAt(light_position, light_position + z, y); break;
        case 5:
          view = glm::lookAt(light_position, light_position - z, y); break;
        };

        const auto view_inv = glm::inverse(view);

        shadow_frustums[i].view = view;
        shadow_frustums[i].view_inv = view_inv;
        shadow_frustums[i].proj = proj;
        shadow_frustums[i].proj_inv = proj_inv;
      }

      auto shadow_mapped = shadow_data->Map();
      memcpy(shadow_mapped, &shadow_frustums, sizeof(Frustum) * 6);
      shadow_data->Unmap();
    }
  }

  void Render3DBroker::Discard()
  {
  }

  Render3DBroker::Render3DBroker(Wrap& wrap)
    : Broker("spark_broker", wrap)
  {
    const auto find_view_fn = [this](const std::shared_ptr<View>& view)
    {
      if (view->GetName().compare("backbuffer_ua_view") == 0)
      {
        this->backbuffer_uav = view;
      }

      if (view->GetName().compare("backbuffer_rt_view") == 0)
      {
        this->backbuffer_rtv = view;
      }
    };
    wrap.GetCore()->VisitView(find_view_fn);


    const auto tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_scene = tree->GetObjectItem("scene_property");
    {
      prop_instances = prop_scene->GetObjectItem("instances");
      prop_triangles = prop_scene->GetObjectItem("triangles");
      prop_vertices = prop_scene->GetObjectItem("vertices");

      //prop_vertices0 = prop_scene->GetObjectItem("vertices0");
      //prop_vertices1 = prop_scene->GetObjectItem("vertices1");
      //prop_vertices2 = prop_scene->GetObjectItem("vertices2");
      //prop_vertices3 = prop_scene->GetObjectItem("vertices3");

      prop_t_boxes = prop_scene->GetObjectItem("t_boxes");
      prop_b_boxes = prop_scene->GetObjectItem("b_boxes");

      prop_textures0 = prop_scene->GetObjectItem("textures0");
      prop_textures1 = prop_scene->GetObjectItem("textures1");
      prop_textures2 = prop_scene->GetObjectItem("textures2");
      prop_textures3 = prop_scene->GetObjectItem("textures3");

      prop_lightmaps = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
      {
        prop_lightmaps->SetArraySize(uint32_t(1));

        const auto texel_value = glm::u8vec4(255, 255, 255, 255);
        const auto texel_size = uint32_t(sizeof(texel_value));

        const auto texels_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
        texels_property->RawAllocate(texel_size);
        texels_property->SetRawBytes({ &texel_value, texel_size }, 0);
        prop_lightmaps->SetArrayItem(0, texels_property);
      }

      //prop_lightmaps = prop_scene->GetObjectItem("textures4");
    }

    prop_camera = tree->GetObjectItem("camera_property");
    {
      prop_eye = prop_camera->GetObjectItem("eye");
      prop_lookat = prop_camera->GetObjectItem("lookat");
      prop_up = prop_camera->GetObjectItem("up");

      prop_fov_x = prop_camera->GetObjectItem("fov_x");
      prop_fov_y = prop_camera->GetObjectItem("fov_y");

      prop_extent_x = prop_camera->GetObjectItem("extent_x");
      prop_extent_y = prop_camera->GetObjectItem("extent_y");

      prop_n_plane = prop_camera->GetObjectItem("n_plane");
      prop_f_plane = prop_camera->GetObjectItem("f_plane");

      prop_counter = prop_camera->GetObjectItem("counter");
    }

    prop_skybox = tree->GetObjectItem("environment_property");

    auto* device = wrap.GetCore()->GetDevice().get();

    CreateColorTarget();
    CreateDepthTarget();
    CreateShadowMap();

    CreateGBufferTarget0();
    CreateGBufferTarget1();

    CreateScreenData();
    CreateCameraData();
    CreateShadowData();

    CreateSceneInstances();
    CreateSceneTriangles();
    CreateSceneVertices();

    CreateSceneTBoxes();
    CreateSceneBBoxes();

    CreateTraceInstances();
    CreateTraceTriangles();
    CreateTraceVertices();

    CreateSceneTextures0();
    CreateSceneTextures1();
    CreateSceneTextures2();
    CreateSceneTextures3();

    CreateScreenQuadVertices();
    CreateScreenQuadTriangles();
    CreateSkyboxTexture();

    CreateLightMaps();

    CreateGraphicArguments();
    CreateComputeArguments();




  }

  Render3DBroker::~Render3DBroker()
  {
    auto* device = wrap.GetCore()->GetDevice().get();

    DestroyGraphicArguments();
    DestroyComputeArguments();

    DestroyScreenQuadVertices();
    DestroyScreenQuadTriangles();
    DestroySkyboxTexture();

    DestroyLightMaps();

    DestroySceneTextures0();
    DestroySceneTextures1();
    DestroySceneTextures2();
    DestroySceneTextures3();

    DestroyTraceInstances();
    DestroyTraceTriangles();
    DestroyTraceVertices();

    DestroySceneTBoxes();
    DestroySceneBBoxes();

    DestroySceneInstances();
    DestroySceneTriangles();
    DestroySceneVertices();

    DestroyScreenData();
    DestroyCameraData();
    DestroyShadowData();

    DestroyColorTarget();
    DestroyDepthTarget();
    DestroyShadowMap();

    DestroyGBufferTarget0();
    DestroyGBufferTarget1();
  }
}