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


#include "spark.h"

namespace RayGene3D
{
  void Spark::CreateShadowMap()
  {
    shadow_map = wrap.GetCore()->GetDevice()->CreateResource("spark_shadow_map",
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

  void Spark::CreateColorTarget()
  {
    color_target = wrap.GetCore()->GetDevice()->CreateResource("spark_color_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE | USAGE_UNORDERED_ACCESS),
        1,
        1,
        FORMAT_R11G11B10_FLOAT,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
      }
    );
  }

  void Spark::CreateDepthTarget()
  {
    depth_target = wrap.GetCore()->GetDevice()->CreateResource("spark_depth_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_D32_FLOAT,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
      }
    );
  }

  void Spark::CreateGBufferTarget0()
  {
    gbuffer_0_target = wrap.GetCore()->GetDevice()->CreateResource("spark_gbuffer_0_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_R8G8B8A8_UNORM,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
      }
    );
  }

  void Spark::CreateGBufferTarget1()
  {
    gbuffer_1_target = wrap.GetCore()->GetDevice()->CreateResource("spark_gbuffer_1_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_R8G8B8A8_UNORM,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
      }
    );
  }

  void Spark::CreateScreenData()
  {
    screen_data = wrap.GetCore()->GetDevice()->CreateResource("spark_screen_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Screen)),
        1,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );
  }

  void Spark::CreateCameraData()
  {
    camera_data = wrap.GetCore()->GetDevice()->CreateResource("spark_camera_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Frustum)),
        1,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );
  }

  void Spark::CreateShadowData()
  {
    shadow_data = wrap.GetCore()->GetDevice()->CreateResource("spark_shadow_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Frustum)),
        6,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );
  }

  void Spark::CreateSceneInstances()
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_instances->GetRawBytes(0),
    };

    scene_instances = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_instances",
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

  void Spark::CreateSceneTriangles()
  {
    const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_triangles->GetRawBytes(0),
    };

    scene_triangles = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_triangles",
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

  void Spark::CreateSceneVertices()
  {
    const auto [data, count] = prop_vertices->GetTypedBytes<Vertex>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_vertices->GetRawBytes(0),
    };

    scene_vertices = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_vertices",
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

  void Spark::CreateSceneTBoxes()
  {
    const auto [data, count] = prop_t_boxes->GetTypedBytes<Box>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_t_boxes->GetRawBytes(0),
    };

    scene_t_boxes = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_t_boxes",
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

  void Spark::CreateSceneBBoxes()
  {
    const auto [data, count] = prop_b_boxes->GetTypedBytes<Box>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_b_boxes->GetRawBytes(0),
    };

    scene_b_boxes = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_b_boxes",
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

  void Spark::CreateTraceInstances()
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_instances->GetRawBytes(0),
    };

    trace_instances = wrap.GetCore()->GetDevice()->CreateResource("spark_trace_instances",
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

  void Spark::CreateTraceTriangles()
  {
    const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_triangles->GetRawBytes(0),
    };

    trace_triangles = wrap.GetCore()->GetDevice()->CreateResource("spark_trace_triangles",
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

  void Spark::CreateTraceVertices()
  {
    const auto [data, count] = prop_vertices->GetTypedBytes<Vertex>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_vertices->GetRawBytes(0),
    };

    trace_vertices = wrap.GetCore()->GetDevice()->CreateResource("spark_trace_vertices",
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

  void Spark::CreateSceneTextures0()
  {
    const auto layers = prop_textures0->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures0->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures0->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures0->GetArrayItem(i)->GetRawBytes(0));
    }

    scene_textures0 = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_textures0",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Spark::CreateSceneTextures1()
  {
    const auto layers = prop_textures1->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures1->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures1->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures1->GetArrayItem(i)->GetRawBytes(0));
    }

    scene_textures1 = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_textures1",
      Resource::Tex2DDesc
      {

        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Spark::CreateSceneTextures2()
  {
    const auto layers = prop_textures2->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures2->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures2->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures2->GetArrayItem(i)->GetRawBytes(0));
    }

    scene_textures2 = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_textures2",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Spark::CreateSceneTextures3()
  {
    const auto layers = prop_textures3->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures3->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures3->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures3->GetArrayItem(i)->GetRawBytes(0));
    }

    scene_textures3 = wrap.GetCore()->GetDevice()->CreateResource("spark_scene_textures3",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Spark::CreateLightMaps()
  {
    const auto layers = prop_lightmaps->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_lightmaps->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_lightmaps->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_lightmaps->GetArrayItem(i)->GetRawBytes(0));
    }

    light_maps = wrap.GetCore()->GetDevice()->CreateResource("spark_light_maps",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Spark::CreateScreenQuadVertices()
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

    screen_quad_vertices = wrap.GetCore()->GetDevice()->CreateResource("spark_screen_quad_vertices",
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

  void Spark::CreateScreenQuadTriangles()
  {
    static const std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_idx.data(), uint32_t(quad_idx.size() * sizeof(glm::u32vec3)) },
    };

    screen_quad_triangles = wrap.GetCore()->GetDevice()->CreateResource("spark_screen_quad_triangles",
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

  void Spark::CreateSkyboxTexture()
  {
    const auto layers = prop_skybox->GetArraySize();
    const auto format = FORMAT_R32G32B32A32_FLOAT;
    const auto bpp = 16u;

    auto mipmaps = 1u;
    auto size_x = 2u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_skybox->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_skybox->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_skybox->GetArrayItem(i)->GetRawBytes(0));
    }

    skybox_texture = wrap.GetCore()->GetDevice()->CreateResource("spark_skybox_texture",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_UNKNOWN),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Spark::CreateGraphicArguments()
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

    graphic_arguments = wrap.GetCore()->GetDevice()->CreateResource("spark_graphic_arguments",
      Resource::BufferDesc
      {
        Usage(USAGE_ARGUMENT_INDIRECT),
        uint32_t(sizeof(Mesh::Graphic)),
        uint32_t(count),
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );
  }

  void Spark::CreateComputeArguments()
  {
    compute_arguments = wrap.GetCore()->GetDevice()->CreateResource("spark_compute_arguments",
      Resource::BufferDesc
      {
        Usage(USAGE_ARGUMENT_INDIRECT),
        uint32_t(sizeof(Mesh::Compute)),
        1u,
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );
  }



  void Spark::CreateShadowmapPass(uint32_t index)
  {
    const auto extent_x = shadow_resolution;
    const auto extent_y = shadow_resolution;
    const auto extent_z = 0u;

    auto shadowmap_shadow_map = shadow_map->CreateView("spark_shadowmap_shadow_map_" + std::to_string(index),
      Usage(USAGE_DEPTH_STENCIL),
      { index, 1 }
    );

    const Pass::DSAttachment ds_attachments[] = {
      { shadowmap_shadow_map, { 1.0f, std::nullopt }},
    };

    shadowmap_passes[index] = wrap.GetCore()->GetDevice()->CreatePass("spark_shadowmap_pass_" + std::to_string(index),
      Pass::TYPE_GRAPHIC,
      extent_x,
      extent_y,
      extent_z,
      {},
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }


  void Spark::CreateShadowmapTechnique(uint32_t index)
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadowmap.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    const Technique::IAState ia_state =
    {
      Technique::TOPOLOGY_TRIANGLELIST,
      Technique::INDEXER_32_BIT,
      {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false }
      }
    };

    const Technique::RCState rc_state =
    {
      Technique::FILL_SOLID,
      Technique::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(shadow_resolution), float(shadow_resolution), 0.0f, 1.0f }
      },
    };

    const Technique::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      Technique::COMPARISON_LESS //depth_comparison
    };

    const Technique::OMState om_state =
    {
      false,
      {
        { false, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF }
      }
    };

    shadowmap_techniques[index] = shadowmap_passes[index]->CreateTechnique("spark_shadowmap_technique",
      shader_ss.str(),
      Technique::Compilation(Technique::COMPILATION_VS),
      {},
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }

  void Spark::CreateShadowmapBatch(uint32_t index)
  {
    auto shadowmap_shadow_data = shadow_data->CreateView("spark_shadowmap_shadow_data",
      USAGE_CONSTANT_DATA,
      { uint32_t(sizeof(Frustum)) * index, uint32_t(sizeof(Frustum)) }
    );

    const std::shared_ptr<View> ub_views[] = {
      shadowmap_shadow_data,
    };

    shadowmap_batches[index] = shadowmap_techniques[index]->CreateBatch("spark_shadowmap_batch",
      {},
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      {},
      {},
      {},
      {}
    );
  }


  void Spark::CreateShadowmapMesh(uint32_t index)
  {
    const auto shadowmap_scene_vertices = scene_vertices->CreateView("spark_shadowmap_scene_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      shadowmap_scene_vertices,
    };

    const auto shadowmap_scene_triangles = scene_triangles->CreateView("spark_shadowmap_scene_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      shadowmap_scene_triangles,
    };

    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    auto subsets = std::vector<Mesh::Subset>(count);
    for (auto i = 0u; i < count; ++i)
    {
      const auto shadowmap_graphic_arguments = graphic_arguments->CreateView("spark_shadowmap_graphic_argument_" + std::to_string(index) + "_" + std::to_string(i),
        Usage(USAGE_ARGUMENT_INDIRECT),
        { uint32_t(sizeof(Mesh::Graphic)) * i, uint32_t(sizeof(Mesh::Graphic)) }
      );

      const auto& vtx_range = View::Range{ data[i].vert_offset, data[i].vert_count };
      const auto& idx_range = View::Range{ data[i].prim_offset * 3, data[i].prim_count * 3 };
      const auto& ins_range = View::Range{ 0u,  1u };
      const auto& sb_offset = std::nullopt;
      const auto& push_data = std::nullopt;

      subsets[i] = { nullptr, vtx_range, idx_range, ins_range, 0u, 0u, 0u, sb_offset, push_data };
    }

    shadowmap_meshes[index] = shadowmap_batches[index]->CreateMesh("spark_shadowmap_mesh_" + std::to_string(index),
      { subsets.data(), uint32_t(subsets.size()) },
      { va_views, uint32_t(std::size(va_views)) },
      { ia_views, uint32_t(std::size(ia_views)) }
    ); 
  }



  void Spark::CreateHWTracedPass()
  {
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();
    const auto extent_z = 0u;

    hw_traced_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_hw_traced_pass",
      Pass::TYPE_RAYTRACING,
      extent_x,
      extent_y,
      extent_z
    );
  }


  void Spark::CreateHWTracedTechnique()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_hw_traced.glsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    hw_traced_technique = hw_traced_pass->CreateTechnique("spark_hw_traced_technique",
      shader_ss.str(),
      Technique::Compilation(Technique::COMPILATION_RGEN | Technique::COMPILATION_MISS),
      { defines.data(), uint32_t(defines.size()) },
      {},
      {},
      {},
      {}
    );
  }

  void Spark::CreateHWTracedBatch()
  {
    const Batch::Sampler samplers[] = {
      { Batch::Sampler::FILTERING_NEAREST, 1, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };

    auto hw_traced_screen_data = screen_data->CreateView("spark_hw_traced_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto hw_traced_camera_data = camera_data->CreateView("spark_hw_traced_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto hw_traced_shadow_data = shadow_data->CreateView("spark_hw_traced_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, sizeof(Frustum) }
    );
    const std::shared_ptr<View> ub_views[] = {
      hw_traced_screen_data,
      hw_traced_camera_data,
      hw_traced_shadow_data,
    };

    auto hw_traced_gbuffer_0_texture = gbuffer_0_target->CreateView("spark_hw_traced_gbuffer_0_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto hw_traced_gbuffer_1_texture = gbuffer_1_target->CreateView("spark_hw_traced_gbuffer_1_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto hw_traced_depth_texture = depth_target->CreateView("spark_hw_traced_depth_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      hw_traced_gbuffer_0_texture,
      hw_traced_gbuffer_1_texture,
      hw_traced_depth_texture,
    };

    auto hw_traced_color_texture = color_target->CreateView("spark_hw_traced_color_texture",
      Usage(USAGE_UNORDERED_ACCESS)
    );
    const std::shared_ptr<View> wi_views[] = {
      hw_traced_color_texture,
    };

    hw_traced_batch = hw_traced_technique->CreateBatch("spark_hw_traced_batch",
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      { wi_views, uint32_t(std::size(wi_views)) },
      {},
      {}
    );
  }


  void Spark::CreateHWTracedMesh()
  {
    auto hw_traced_trace_vertices = trace_vertices->CreateView("spark_hw_traced_trace_vertices",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> va_views[] = {
      hw_traced_trace_vertices,
    };

    auto hw_traced_trace_triangles = trace_triangles->CreateView("spark_hw_traced_trace_triangles",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ia_views[] = {
      hw_traced_trace_triangles,
    };

    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    auto subsets = std::vector<Mesh::Subset>(count);
    for (auto i = 0u; i < count; ++i)
    {
      const auto& vtx_range = View::Range{ data[i].vert_count, data[i].vert_offset };
      const auto& idx_range = View::Range{ data[i].prim_count * 3, data[i].prim_offset * 3 };
      const auto& ins_range = View::Range{ 1u,  0u };
      const auto& sb_offset = std::nullopt;
      const auto& push_data = std::nullopt;

      subsets[i] = { nullptr, vtx_range, idx_range, ins_range, 0u, 0u, 0u, sb_offset, push_data };
    }

    hw_traced_mesh = hw_traced_batch->CreateMesh("spark_hw_traced_mesh",
      { subsets.data(), uint32_t(subsets.size()) },
      { va_views, uint32_t(std::size(va_views)) },
      { ia_views, uint32_t(std::size(ia_views)) }
    );
  }




  void Spark::CreateGeometryPass()
  {
    //Pass::Subpass subpasses[ShadingSubpass::SUBPASS_MAX_COUNT] = {};
    //{
    //  const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    //  auto geometry_graphic_arguments = std::vector<std::shared_ptr<View>>(instance_count);
    //  for (uint32_t j = 0; j < uint32_t(geometry_graphic_arguments.size()); ++j)
    //  {
    //    geometry_graphic_arguments[j] = graphic_arguments->CreateView("spark_geometry_graphic_argument_" + std::to_string(j),
    //      Usage(USAGE_ARGUMENT_INDIRECT),
    //      { j * uint32_t(sizeof(Pass::Argument)), uint32_t(sizeof(Pass::Argument)) }
    //    );
    //  }
    //  std::vector<Pass::Command> geometry_commands(instance_count);
    //  for (uint32_t j = 0; j < uint32_t(geometry_commands.size()); ++j)
    //  {
    //    geometry_commands[j].view = geometry_graphic_arguments[j];
    //    geometry_commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
    //  }

    //  auto geometry_scene_vertices = scene_vertices->CreateView("spark_geometry_scene_vertices",
    //    Usage(USAGE_VERTEX_ARRAY)
    //  );
    //  std::vector<std::shared_ptr<View>> geometry_va_views = {
    //    geometry_scene_vertices,
    //  };

    //  auto geometry_scene_triangles = scene_triangles->CreateView("spark_geometry_scene_triangles",
    //    Usage(USAGE_INDEX_ARRAY)
    //  );
    //  std::vector<std::shared_ptr<View>> geometry_ia_views = {
    //    geometry_scene_triangles,
    //  };

    //  subpasses[ShadingSubpass::SUBPASS_OPAQUE] =
    //  {
    //    geometry_technique, geometry_batch,
    //    std::move(geometry_commands),
    //    std::move(geometry_va_views),
    //    std::move(geometry_ia_views)
    //  };
    //}
    //{
    //  const Pass::Command skybox_commands[] = {
    //    {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
    //  };

    //  auto skybox_skybox_vertices = screen_quad_vertices->CreateView("spark_skybox_vertices",
    //    Usage(USAGE_VERTEX_ARRAY)
    //  );
    //  const std::shared_ptr<View> skybox_va_views[] = {
    //    skybox_skybox_vertices,
    //  };

    //  auto skybox_skybox_triangles = screen_quad_triangles->CreateView("spark_skybox_triangles",
    //    Usage(USAGE_INDEX_ARRAY)
    //  );
    //  const std::shared_ptr<View> skybox_ia_views[] = {
    //    skybox_skybox_triangles,
    //  };

    //  subpasses[ShadingSubpass::SUBPASS_SKYBOX] =
    //  {
    //    skybox_technique, skybox_batch,
    //    { skybox_commands, skybox_commands + std::size(skybox_commands) },
    //    { skybox_va_views, skybox_va_views + std::size(skybox_va_views) },
    //    { skybox_ia_views, skybox_ia_views + std::size(skybox_ia_views) }
    //  };
    //}

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();
    const auto extent_z = 0u;

    auto geometry_color_target = color_target->CreateView("spark_geometry_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    auto geometry_gbuffer_0_target = gbuffer_0_target->CreateView("spark_geometry_gbuffer_0_target",
      Usage(USAGE_RENDER_TARGET)
    );
    auto geometry_gbuffer_1_target = gbuffer_1_target->CreateView("spark_geometry_gbuffer_1_target",
      Usage(USAGE_RENDER_TARGET)
    );

    const Pass::RTAttachment rt_attachments[] = {
      { geometry_color_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
      { geometry_gbuffer_0_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
      { geometry_gbuffer_1_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
    };

    auto geometry_depth_target = depth_target->CreateView("spark_geometry_depth_target",
      Usage(USAGE_DEPTH_STENCIL)
    );
    const Pass::DSAttachment ds_attachments[] = {
      geometry_depth_target, { 1.0f, std::nullopt },
    };

    geometry_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_geometry_pass",
      Pass::TYPE_GRAPHIC,
      extent_x,
      extent_y,
      extent_z,
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Spark::CreateGeometryTechnique()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_geometry.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const Technique::IAState ia_state =
    {
      Technique::TOPOLOGY_TRIANGLELIST,
      Technique::INDEXER_32_BIT,
      {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 12, 64, FORMAT_R8G8B8A8_UNORM, false },
        { 0, 16, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 28, 64, FORMAT_R32_UINT, false },
        { 0, 32, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 44, 64, FORMAT_R32_FLOAT, false },
        { 0, 48, 64, FORMAT_R32G32_FLOAT, false },
        { 0, 56, 64, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Technique::RCState rc_state =
    {
      Technique::FILL_SOLID,
      Technique::CULL_BACK,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Technique::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      Technique::COMPARISON_LESS //depth_comparison
    };

    const Technique::OMState om_state =
    {
      false,
      {
        { false, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF },
        { false, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF },
        { false, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF },
      }
    };

    geometry_technique = geometry_pass->CreateTechnique("spark_geometry_technique",
      shader_ss.str(),
      Technique::Compilation(Technique::COMPILATION_VS | Technique::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }


  void Spark::CreateGeometryBatch()
  {
    const Batch::Sampler samplers[] = {
  { Batch::Sampler::FILTERING_ANISOTROPIC, 16, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };

    auto geometry_screen_data = screen_data->CreateView("spark_geometry_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );

    auto geometry_camera_data = camera_data->CreateView("spark_geometry_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );

    auto geometry_shadow_data = shadow_data->CreateView("spark_geometry_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Frustum)) }
    );

    const std::shared_ptr<View> ub_views[] = {
      geometry_screen_data,
      geometry_camera_data,
      geometry_shadow_data,
    };


    auto geometry_scene_instances = scene_instances->CreateView("spark_geometry_scene_instances",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Instance)) }
    );

    const std::shared_ptr<View> sb_views[] = {
      geometry_scene_instances
    };


    auto geometry_scene_textures0 = scene_textures0->CreateView("spark_geometry_scene_textures0",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_scene_textures1 = scene_textures1->CreateView("spark_geometry_scene_textures1",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_scene_textures2 = scene_textures2->CreateView("spark_geometry_scene_textures2",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_scene_textures3 = scene_textures3->CreateView("spark_geometry_scene_textures3",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_light_maps = light_maps->CreateView("spark_geometry_light_maps",
      Usage(USAGE_SHADER_RESOURCE)
    );

    const std::shared_ptr<View> ri_views[] = {
      geometry_scene_textures0,
      geometry_scene_textures1,
      geometry_scene_textures2,
      geometry_scene_textures3,
    };

    geometry_batch = geometry_technique->CreateBatch("spark_geometry_batch",
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      { sb_views, uint32_t(std::size(sb_views)) },
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {}
    );
  }

  void Spark::CreateGeometryMesh()
  {
    const auto geometry_scene_vertices = scene_vertices->CreateView("spark_geometry_scene_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    std::shared_ptr<View> va_views[] = {
      geometry_scene_vertices,
    };

    const auto geometry_scene_triangles = scene_triangles->CreateView("spark_geometry_scene_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    std::shared_ptr<View> ia_views[] = {
      geometry_scene_triangles,
    };

    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    auto subsets = std::vector<Mesh::Subset>(count);
    for (auto i = 0u; i < count; ++i)
    {
      const auto geometry_graphic_arguments = graphic_arguments->CreateView("spark_geometry_graphic_argument_" + std::to_string(i),
        Usage(USAGE_ARGUMENT_INDIRECT),
        { uint32_t(sizeof(Mesh::Graphic)) * i, uint32_t(sizeof(Mesh::Graphic)) }
      );

      const auto& vtx_range = View::Range{ data[i].vert_offset, data[i].vert_count };
      const auto& idx_range = View::Range{ data[i].prim_offset * 3, data[i].prim_count * 3 };
      const auto& ins_range = View::Range{ 0u,  1u };
      const auto& sb_offset = std::array<uint32_t, 4>{ uint32_t(sizeof(Frustum))* i, 0u, 0u, 0u };
      const auto& push_data = std::nullopt;

      subsets[i] = { geometry_graphic_arguments, vtx_range, idx_range, ins_range, 0u, 0u, 0u, sb_offset, push_data };
    }

    geometry_mesh = geometry_batch->CreateMesh("spark_geometry_mesh",
      { subsets.data(), uint32_t(subsets.size()) },
      { va_views, uint32_t(std::size(va_views)) },
      { ia_views, uint32_t(std::size(ia_views)) }
    );
  }


  void Spark::CreateSkyboxTechnique()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_environment.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::pair<std::string, std::string> defines[] =
    {
      { "TEST", "1" },
    };

    const Technique::IAState ia_state =
    {
      Technique::TOPOLOGY_TRIANGLELIST,
      Technique::INDEXER_32_BIT,
      {
        { 0, 0, 8, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Technique::RCState rc_state =
    {
      Technique::FILL_SOLID,
      Technique::CULL_BACK,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Technique::DSState ds_state =
    {
      true, //depth_enabled
      false, //depth_write
      Technique::COMPARISON_EQUAL //depth_comparison
    };

    const Technique::OMState om_state =
    {
      false,
      {
        { false, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF },
        { false, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF },
        { false, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF },
      }
    };

    skybox_technique = geometry_pass->CreateTechnique("spark_skybox_technique",
      shader_ss.str(),
      Technique::Compilation(Technique::COMPILATION_VS | Technique::COMPILATION_PS),
      { defines, uint32_t(std::size(defines)) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }


  void Spark::CreateSkyboxBatch()
  {
    auto skybox_screen_data = screen_data->CreateView("spark_skybox_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto skybox_camera_data = camera_data->CreateView("spark_skybox_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    const std::shared_ptr<View> ub_views[] = {
      skybox_screen_data,
      skybox_camera_data,
    };

    auto skybox_skybox_texture = skybox_texture->CreateView("spark_skybox_skybox_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      skybox_skybox_texture,
    };

    const Batch::Sampler samplers[] = {
      { Batch::Sampler::FILTERING_ANISOTROPIC, 16, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };

    skybox_batch = skybox_technique->CreateBatch("spark_skybox_batch",
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {}
    );
  }

  void Spark::CreateSkyboxMesh()
  {
    auto skybox_screen_quad_vertices = screen_quad_vertices->CreateView("spark_skybox_screen_quad_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      skybox_screen_quad_vertices,
    };

    auto skybox_screen_quad_triangles = screen_quad_triangles->CreateView("spark_skybox_screen_quad_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      skybox_screen_quad_triangles,
    };

    const Mesh::Subset subsets[] = { 
      {nullptr, { 0u, 4u }, { 0u, 6u }, { 0u, 1u }}
    };

    skybox_mesh = skybox_batch->CreateMesh("spark_skybox_mesh",
      { subsets, uint32_t(std::size(subsets)) },
      { va_views, uint32_t(std::size(va_views)) },
      { ia_views, uint32_t(std::size(ia_views)) }
    );
  }

  void Spark::CreateUnshadowedPass()
  {
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();
    const auto extent_z = 0u;

    auto unshadowed_color_target = color_target->CreateView("spark_unshadowed_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      unshadowed_color_target, std::nullopt,
    };

    unshadowed_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_unshadowed_pass",
      Pass::TYPE_GRAPHIC,
      extent_x,
      extent_y,
      extent_z,
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      {}
    );
  }

  void Spark::CreateUnshadowedTechnique()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_unshadowed.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const Technique::IAState ia_state =
    {
      Technique::TOPOLOGY_TRIANGLELIST,
      Technique::INDEXER_32_BIT,
      {
        { 0, 0, 8, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Technique::RCState rc_state =
    {
      Technique::FILL_SOLID,
      Technique::CULL_BACK,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Technique::DSState ds_state =
    {
      false, //depth_enabled
      false, //depth_write
      Technique::COMPARISON_ALWAYS //depth_comparison
    };

    const Technique::OMState om_state =
    {
      false,
      {
        { true, Technique::OPERAND_ONE, Technique::OPERAND_ONE, Technique::OPERATION_ADD, Technique::OPERAND_ONE, Technique::OPERAND_ONE, Technique::OPERATION_ADD, 0xF },
      }
    };

    unshadowed_technique = unshadowed_pass->CreateTechnique("spark_unshadowed_technique",
      shader_ss.str(),
      Technique::Compilation(Technique::COMPILATION_VS | Technique::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }



  void Spark::CreateUnshadowedBatch()
  {
    auto unshadowed_screen_data = screen_data->CreateView("spark_unshadowed_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto unshadowed_camera_data = camera_data->CreateView("spark_unshadowed_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto unshadowed_shadow_data = shadow_data->CreateView("spark_unshadowed_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Frustum)) }
    );
    const std::shared_ptr<View> ub_views[] = {
      unshadowed_screen_data,
      unshadowed_camera_data,
      unshadowed_shadow_data
    };

    auto unshadowed_gbuffer_0_texture = gbuffer_0_target->CreateView("spark_unshadowed_gbuffer_0_target",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto unshadowed_gbuffer_1_texture = gbuffer_1_target->CreateView("spark_unshadowed_gbuffer_1_target",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto unshadowed_depth_texture = depth_target->CreateView("spark_unshadowed_depth_target",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      unshadowed_gbuffer_0_texture,
      unshadowed_gbuffer_1_texture,
      unshadowed_depth_texture,
    };

    unshadowed_batch = unshadowed_technique->CreateBatch("spark_unshadowed_batch",
      {},
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) }
    );
  }

  void Spark::CreateUnshadowedMesh()
  {
    auto unshadowed_screen_quad_vertices = screen_quad_vertices->CreateView("spark_unshadowed_screen_quad_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      unshadowed_screen_quad_vertices,
    };

    auto unshadowed_screen_quad_triangles = screen_quad_triangles->CreateView("spark_unshadowed_screen_quad_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      unshadowed_screen_quad_triangles,
    };

    const Mesh::Subset subsets[] = {
      {nullptr, { 0u, 4u }, { 0u, 6u }, { 0u, 1u }}
    };

    unshadowed_mesh = unshadowed_batch->CreateMesh("spark_unshadowed_mesh",
      { subsets, uint32_t(std::size(subsets)) },
      { va_views, uint32_t(std::size(va_views)) },
      { ia_views, uint32_t(std::size(ia_views)) }
    );
  }



  void Spark::CreateShadowedPass()
  {
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();
    const auto extent_z = 0u;

    auto shadowed_color_target = color_target->CreateView("spark_shadowed_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      shadowed_color_target, std::nullopt,
    };

    shadowed_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_shadowed_pass",
      Pass::TYPE_GRAPHIC,
      extent_x,
      extent_y,
      extent_z,
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      {}
    );
  }

  void Spark::CreateShadowedTechnique()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadowed.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const Technique::IAState ia_state =
    {
      Technique::TOPOLOGY_TRIANGLELIST,
      Technique::INDEXER_32_BIT,
      {
        { 0, 0, 8, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Technique::RCState rc_state =
    {
      Technique::FILL_SOLID,
      Technique::CULL_BACK,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Technique::DSState ds_state =
    {
      false, //depth_enabled
      false, //depth_write
      Technique::COMPARISON_ALWAYS //depth_comparison
    };

    const Technique::OMState om_state =
    {
      false,
      {
        { true, Technique::OPERAND_ONE, Technique::OPERAND_ONE, Technique::OPERATION_ADD, Technique::OPERAND_ONE, Technique::OPERAND_ONE, Technique::OPERATION_ADD, 0xF },
      }
    };

    shadowed_technique = shadowed_pass->CreateTechnique("spark_shadowed_technique",
      shader_ss.str(),
      Technique::Compilation(Technique::COMPILATION_VS | Technique::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }


  void Spark::CreateShadowedBatch()
  {
    auto shadowed_screen_data = screen_data->CreateView("spark_shadowed_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto shadowed_camera_data = camera_data->CreateView("spark_shadowed_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto shadowed_shadow_data = shadow_data->CreateView("spark_shadowed_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Frustum)) }
    );
    const std::shared_ptr<View> ub_views[] = {
      shadowed_screen_data,
      shadowed_camera_data,
      shadowed_shadow_data
    };

    auto shadowed_gbuffer_0_texture = gbuffer_0_target->CreateView("spark_shadowed_gbuffer_0_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_gbuffer_1_texture = gbuffer_1_target->CreateView("spark_shadowed_gbuffer_1_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_depth_texture = depth_target->CreateView("spark_shadowed_depth_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_shadow_map = shadow_map->CreateView("spark_shadowed_shadow_map",
      Usage(USAGE_SHADER_RESOURCE),
      View::Bind(View::BIND_CUBEMAP_LAYER)
    );
    const std::shared_ptr<View> ri_views[] = {
      shadowed_gbuffer_0_texture,
      shadowed_gbuffer_1_texture,
      shadowed_depth_texture,
      shadowed_shadow_map,
    };

    const Batch::Sampler samplers[] = {
      { Batch::Sampler::FILTERING_NEAREST, 1, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_ALWAYS, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
    };

    shadowed_batch = shadowed_technique->CreateBatch("spark_shadowed_batch",
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) }
    );
  }

  void Spark::CreateShadowedMesh()
  {
    auto shadowed_screen_quad_vertices = screen_quad_vertices->CreateView("spark_shadowed_screen_quad_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      shadowed_screen_quad_vertices,
    };

    auto shadowed_screen_quad_triangles = screen_quad_triangles->CreateView("spark_shadowed_screen_quad_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      shadowed_screen_quad_triangles,
    };

    const Mesh::Subset subsets[] = {
      {nullptr, { 0u, 4u }, { 0u, 6u }, { 0u, 1u }}
    };

    shadowed_mesh = shadowed_batch->CreateMesh("spark_shadowed_mesh",
      { subsets, uint32_t(std::size(subsets)) },
      { va_views, uint32_t(std::size(va_views)) },
      { ia_views, uint32_t(std::size(ia_views)) }
    );
  }


  void Spark::CreateSWTracedPass()
  {
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();
    const auto extent_z = 0u;

    auto sw_traced_color_target = color_target->CreateView("spark_sw_traced_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      sw_traced_color_target, std::nullopt,
    };

    sw_traced_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_sw_traced_pass",
      Pass::TYPE_GRAPHIC,
      extent_x,
      extent_y,
      extent_z,
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      {}
    );
  }


  void Spark::CreateSWTracedTechnique()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_sw_traced.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const Technique::IAState ia_state =
    {
      Technique::TOPOLOGY_TRIANGLELIST,
      Technique::INDEXER_32_BIT,
      {
        { 0, 0, 8, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Technique::RCState rc_state =
    {
      Technique::FILL_SOLID,
      Technique::CULL_BACK,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Technique::DSState ds_state =
    {
      false, //depth_enabled
      false, //depth_write
      Technique::COMPARISON_ALWAYS //depth_comparison
    };

    const Technique::OMState om_state =
    {
      false,
      {
        { true, Technique::OPERAND_ONE, Technique::OPERAND_ONE, Technique::OPERATION_ADD, Technique::OPERAND_ONE, Technique::OPERAND_ONE, Technique::OPERATION_ADD, 0xF },
      }
    };

    sw_traced_technique = sw_traced_pass->CreateTechnique("spark_sw_traced_technique",
      shader_ss.str(),
      Technique::Compilation(Technique::COMPILATION_VS | Technique::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }

  void Spark::CreateSWTracedBatch()
  {
    auto sw_traced_screen_data = screen_data->CreateView("spark_sw_traced_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto sw_traced_camera_data = camera_data->CreateView("spark_sw_traced_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto sw_traced_shadow_data = shadow_data->CreateView("spark_sw_traced_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, sizeof(Frustum) }
    );
    const std::shared_ptr<View> ub_views[] = {
      sw_traced_screen_data,
      sw_traced_camera_data,
      sw_traced_shadow_data,
    };

    auto sw_traced_scene_t_boxes = scene_t_boxes->CreateView("spark_sw_traced_scene_t_boxes",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_scene_b_boxes = scene_b_boxes->CreateView("spark_sw_traced_scene_b_boxes",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_trace_instances = trace_instances->CreateView("spark_sw_traced_trace_instances",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_trace_triangles = trace_triangles->CreateView("spark_sw_traced_trace_triangles",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_trace_vertices = trace_vertices->CreateView("spark_sw_traced_trace_vertices",
      Usage(USAGE_SHADER_RESOURCE)
    );

    const std::shared_ptr<View> rb_views[] = {
      sw_traced_scene_t_boxes,
      sw_traced_scene_b_boxes,
      sw_traced_trace_instances,
      sw_traced_trace_triangles,
      sw_traced_trace_vertices,
    };

    auto sw_traced_gbuffer_0_texture = gbuffer_0_target->CreateView("spark_sw_traced_gbuffer_0_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_gbuffer_1_texture = gbuffer_1_target->CreateView("spark_sw_traced_gbuffer_1_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_depth_texture = depth_target->CreateView("spark_sw_traced_depth_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      sw_traced_gbuffer_0_texture,
      sw_traced_gbuffer_1_texture,
      sw_traced_depth_texture,
    };

    sw_traced_batch = sw_traced_technique->CreateBatch("spark_sw_traced_batch",
      {},
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      { rb_views, uint32_t(std::size(rb_views)) },
      {}
    );
  }


  void Spark::CreateSWTracedMesh()
  {
    auto shadowed_screen_quad_vertices = screen_quad_vertices->CreateView("spark_sw_traced_screen_quad_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      shadowed_screen_quad_vertices,
    };

    auto shadowed_screen_quad_triangles = screen_quad_triangles->CreateView("spark_sw_traced_screen_quad_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      shadowed_screen_quad_triangles,
    };

    const Mesh::Subset subsets[] = {
      {nullptr, { 0u, 4u }, { 0u, 6u }, { 0u, 1u }}
    };

    sw_traced_mesh = sw_traced_batch->CreateMesh("spark_sw_traced_mesh",
      { subsets, uint32_t(std::size(subsets)) },
      { va_views, uint32_t(std::size(va_views)) },
      { ia_views, uint32_t(std::size(ia_views)) }
    );
  }



  void Spark::CreatePresentPass()
  {
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();
    const auto extent_z = 0u;

    present_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_present_pass",
      Pass::TYPE_COMPUTE,
      extent_x,
      extent_y,
      extent_z
    );
  }

  void Spark::CreatePresentTechnique()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_present.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    present_technique = present_pass->CreateTechnique("spark_present_technique",
      shader_ss.str(),
      Technique::COMPILATION_CS,
      {},
      {},
      {},
      {},
      {}
    );
  }

  void Spark::CreatePresentBatch()
  {
    auto present_camera_data = camera_data->CreateView("spark_present_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    const std::shared_ptr<View> ub_views[] = {
      present_camera_data,
    };

    auto present_color_target = color_target->CreateView("spark_present_color_target",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      present_color_target,
    };

    const std::shared_ptr<View> wi_views[] = {
      backbuffer_uav,
    };

    present_batch = present_technique->CreateBatch("spark_present_batch",
      {},
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      { wi_views, uint32_t(std::size(wi_views)) },
      {},
      {}
    );
  }

  void Spark::CreatePresentMesh()
  {
    auto present_compute_arguments = compute_arguments->CreateView("spark_present_compute_arguments",
      Usage(USAGE_ARGUMENT_INDIRECT)
    );

    const Mesh::Subset subsets[] = {
      {present_compute_arguments}
    };

    present_mesh = present_batch->CreateMesh("spark_present_mesh",
      { subsets, uint32_t(std::size(subsets)) }
    );
  }



  void Spark::Initialize()
  {
  }

  void Spark::DestroyColorTarget()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(color_target);
    color_target.reset();
  }

  void Spark::DestroyDepthTarget()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(depth_target);
    depth_target.reset();
  }

  void Spark::DestroyShadowMap()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(shadow_map);
    shadow_map.reset();
  }

  void Spark::DestroyGBufferTarget0()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(gbuffer_0_target);
    gbuffer_0_target.reset();
  }

  void Spark::DestroyGBufferTarget1()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(gbuffer_1_target);
    gbuffer_1_target.reset();
  }

  void Spark::DestroyScreenData()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(screen_data);
    screen_data.reset();
  }

  void Spark::DestroyCameraData()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(camera_data);
    camera_data.reset();
  }

  void Spark::DestroyShadowData()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(shadow_data);
    shadow_data.reset();
  }

  void Spark::DestroySceneInstances()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_instances);
    scene_instances.reset();
  }

  void Spark::DestroySceneTriangles()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_triangles);
    scene_triangles.reset();
  }

  void Spark::DestroySceneVertices()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_vertices);
    scene_vertices.reset();
  }

  void Spark::DestroySceneTBoxes()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_t_boxes);
    scene_t_boxes.reset();
  }

  void Spark::DestroySceneBBoxes()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_b_boxes);
    scene_b_boxes.reset();
  }

  void Spark::DestroyTraceInstances()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_instances);
    trace_instances.reset();
  }

  void Spark::DestroyTraceTriangles()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_triangles);
    trace_triangles.reset();
  }

  void Spark::DestroyTraceVertices()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_vertices);
    trace_vertices.reset();
  }
  
  void Spark::DestroySceneTextures0()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures0);
    scene_textures0.reset();
  }

  void Spark::DestroySceneTextures1()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures1);
    scene_textures1.reset();
  }

  void Spark::DestroySceneTextures2()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures2);
    scene_textures2.reset();
  }

  void Spark::DestroySceneTextures3()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures3);
    scene_textures3.reset();
  }

  void Spark::DestroyLightMaps()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(light_maps);
    light_maps.reset();
  }

  void Spark::DestroyScreenQuadVertices()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(screen_quad_vertices);
    screen_quad_vertices.reset();
  }

  void Spark::DestroyScreenQuadTriangles()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(screen_quad_triangles);
    screen_quad_triangles.reset();
  }

  void Spark::DestroySkyboxTexture()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(skybox_texture);
    skybox_texture.reset();
  }

  void Spark::DestroyGraphicArguments()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(graphic_arguments);
    graphic_arguments.reset();
  }

  void Spark::DestroyComputeArguments()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(compute_arguments);
    compute_arguments.reset();
  }

  void Spark::DestroyGeometryPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(geometry_pass);
    geometry_pass.reset();
  }

  void Spark::DestroyGeometryBatch()
  {
    geometry_technique->DestroyBatch(geometry_batch);
    geometry_batch.reset();
  }

  void Spark::DestroyGeometryTechnique()
  {
    geometry_pass->DestroyTechnique(geometry_technique);
    geometry_technique.reset();
  }

  void Spark::DestroyGeometryMesh()
  {
    geometry_batch->DestroyMesh(geometry_mesh);
    geometry_mesh.reset();
  }

  void Spark::DestroyShadowmapBatch(uint32_t index)
  {
    shadowmap_techniques[index]->DestroyBatch(shadowmap_batches[index]);
    shadowmap_batches[index].reset();
  }

  void Spark::DestroyShadowmapTechnique(uint32_t index)
  {
    shadowmap_passes[index]->DestroyTechnique(shadowmap_techniques[index]);
    shadowmap_techniques[index].reset();
  }

  void Spark::DestroyShadowmapPass(uint32_t index)
  {
    wrap.GetCore()->GetDevice()->DestroyPass(shadowmap_passes[index]);
    shadowmap_passes[index].reset();
  }

  void Spark::DestroyShadowmapMesh(uint32_t index)
  {
    shadowmap_batches[index]->DestroyMesh(shadowmap_meshes[index]);
    shadowmap_meshes[index].reset();
  }

  void Spark::DestroyShadowedBatch()
  {
    shadowed_technique->DestroyBatch(shadowed_batch);
    shadowed_batch.reset();
  }

  void Spark::DestroyShadowedTechnique()
  {
    shadowed_pass->DestroyTechnique(shadowed_technique);
    shadowed_technique.reset();
  }

  void Spark::DestroyShadowedPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(shadowed_pass);
    shadowed_pass.reset();
  }

  void Spark::DestroyShadowedMesh()
  {
    shadowed_batch->DestroyMesh(shadowed_mesh);
    shadowed_mesh.reset();
  }

  void Spark::DestroySWTracedBatch()
  {
    sw_traced_technique->DestroyBatch(sw_traced_batch);
    sw_traced_batch.reset();
  }

  void Spark::DestroySWTracedTechnique()
  {
    sw_traced_pass->DestroyTechnique(sw_traced_technique);
    sw_traced_technique.reset();
  }

  void Spark::DestroySWTracedPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(sw_traced_pass);
    sw_traced_pass.reset();
  }

  void Spark::DestroySWTracedMesh()
  {
    sw_traced_batch->DestroyMesh(sw_traced_mesh);
    sw_traced_mesh.reset();
  }

  void Spark::DestroyHWTracedBatch()
  {
    hw_traced_technique->DestroyBatch(hw_traced_batch);
    hw_traced_batch.reset();
  }

  void Spark::DestroyHWTracedTechnique()
  {
    hw_traced_pass->DestroyTechnique(hw_traced_technique);
    hw_traced_technique.reset();
  }

  void Spark::DestroyHWTracedPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(hw_traced_pass);
    hw_traced_pass.reset();
  }

  void Spark::DestroyHWTracedMesh()
  {
    hw_traced_batch->DestroyMesh(hw_traced_mesh);
    hw_traced_mesh.reset();
  }

  void Spark::DestroyUnshadowedBatch()
  {
    unshadowed_technique->DestroyBatch(unshadowed_batch);
    unshadowed_batch.reset();
  }

  void Spark::DestroyUnshadowedTechnique()
  {
    unshadowed_pass->DestroyTechnique(unshadowed_technique);
    unshadowed_technique.reset();
  }

  void Spark::DestroyUnshadowedPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(unshadowed_pass);
    unshadowed_pass.reset();
  }

  void Spark::DestroyUnshadowedMesh()
  {
    unshadowed_batch->DestroyMesh(unshadowed_mesh);
    unshadowed_mesh.reset();
  }

  void Spark::DestroySkyboxBatch()
  {
    skybox_technique->DestroyBatch(skybox_batch);
    skybox_batch.reset();
  }

  void Spark::DestroySkyboxTechnique()
  {
    geometry_pass->DestroyTechnique(skybox_technique);
    skybox_technique.reset();
  }

  void Spark::DestroySkyboxMesh()
  {
    skybox_batch->DestroyMesh(skybox_mesh);
    skybox_mesh.reset();
  }

  void Spark::DestroyPresentBatch()
  {
    present_technique->DestroyBatch(present_batch);
    present_batch.reset();
  }

  void Spark::DestroyPresentTechnique()
  {
    present_pass->DestroyTechnique(present_technique);
    present_technique.reset();
  }

  void Spark::DestroyPresentPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(present_pass);
    present_pass.reset();
  }

  void Spark::DestroyPresentMesh()
  {
    present_batch->DestroyMesh(present_mesh);
    present_mesh.reset();
  }

  void Spark::Use()
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
      auto graphic_arg = reinterpret_cast<Mesh::Graphic*>(graphic_arguments->Map());

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

      auto compute_arg = reinterpret_cast<Mesh::Compute*>(compute_arguments->Map());
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

  void Spark::Discard()
  {
  }

  Spark::Spark(Wrap& wrap)
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

      prop_instances = prop_scene->GetObjectItem("instances");
      prop_triangles = prop_scene->GetObjectItem("triangles");

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

    for(auto i = 0u; i < 6u; ++i)
    {

      CreateShadowmapPass(i);
      CreateShadowmapTechnique(i);
      CreateShadowmapBatch(i);
      CreateShadowmapMesh(i);
    }

    CreateGeometryPass();
    CreateGeometryTechnique();
    CreateGeometryBatch();
    CreateGeometryMesh();
    CreateSkyboxTechnique();
    CreateSkyboxBatch();
    CreateSkyboxMesh();
  
    CreateShadowedPass();
    CreateShadowedTechnique();
    CreateShadowedBatch();
    CreateShadowedMesh();

    CreateSWTracedPass();
    CreateSWTracedTechnique();
    CreateSWTracedBatch();
    CreateSWTracedMesh();

    CreateHWTracedPass();
    CreateHWTracedTechnique();
    CreateHWTracedBatch();
    CreateHWTracedMesh();
    
    CreateUnshadowedPass();
    CreateUnshadowedTechnique();
    CreateUnshadowedBatch();
    CreateUnshadowedMesh();

    CreatePresentPass();
    CreatePresentTechnique();
    CreatePresentBatch();
    CreatePresentMesh();
  }

  Spark::~Spark()
  {
    auto* device = wrap.GetCore()->GetDevice().get();

    DestroyPresentMesh();
    DestroyPresentBatch();
    DestroyPresentTechnique();
    DestroyPresentPass();
    
    DestroyUnshadowedMesh();
    DestroyUnshadowedBatch(); 
    DestroyUnshadowedTechnique();
    DestroyUnshadowedPass();

    DestroyHWTracedMesh();
    DestroyHWTracedBatch();
    DestroyHWTracedTechnique();
    DestroyHWTracedPass();

    DestroySWTracedMesh();
    DestroySWTracedBatch();
    DestroySWTracedTechnique();
    DestroySWTracedPass();
    
    DestroyShadowedMesh();
    DestroyShadowedBatch();
    DestroyShadowedTechnique();
    DestroyShadowedPass();

    for (auto i = 0u; i < 6u; ++i)
    {
      DestroyShadowmapMesh(i);
      DestroyShadowmapBatch(i);
      DestroyShadowmapTechnique(i);
      DestroyShadowmapPass(i);
    }

    DestroySkyboxMesh();
    DestroySkyboxBatch();
    DestroySkyboxTechnique();
    DestroyGeometryMesh();
    DestroyGeometryBatch();
    DestroyGeometryTechnique();
    DestroyGeometryPass();

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