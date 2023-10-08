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
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE),
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

  void Spark::CreateSkyboxVertices()
  {
    static const std::array<glm::f32vec4, 4> quad_vtx = {
      glm::f32vec4(-1.0f, 1.0f, 0.0f, 0.0f),
      glm::f32vec4(1.0f, 1.0f, 1.0f, 0.0f),
      glm::f32vec4(-1.0f,-1.0f, 0.0f, 1.0f),
      glm::f32vec4(1.0f,-1.0f, 1.0f, 1.0f),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_vtx.data(), uint32_t(quad_vtx.size() * sizeof(glm::f32vec4)) },
    };

    skybox_vertices = wrap.GetCore()->GetDevice()->CreateResource("spark_skybox_vertices",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        uint32_t(sizeof(glm::f32vec4)),
        uint32_t(quad_vtx.size()),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Spark::CreateSkyboxTriangles()
  {
    static const std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_idx.data(), uint32_t(quad_idx.size() * sizeof(glm::u32vec3)) },
    };

    skybox_triangles = wrap.GetCore()->GetDevice()->CreateResource("spark_skybox_triangles",
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
        Usage(USAGE_COMMAND_INDIRECT),
        uint32_t(sizeof(Pass::Argument)),
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
        Usage(USAGE_COMMAND_INDIRECT),
        uint32_t(sizeof(Pass::Argument)),
        1u,
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );
  }

  void Spark::CreateShadowmapLayout()
  {
    auto shadowmap_camera_data = shadow_data->CreateView("spark_shadowmap_shadow_data",
      USAGE_CONSTANT_DATA,
      { 0, sizeof(Frustum) }
    );

    const std::shared_ptr<View> sb_views[] = {
      shadowmap_camera_data,
    };

    shadowmap_layout = wrap.GetCore()->GetDevice()->CreateLayout("spark_shadowmap_layout",
      {},
      { sb_views, uint32_t(std::size(sb_views)) },
      {},
      {},
      {},
      {},
      {},
      {}
    );
  }

  void Spark::CreateShadowmapConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadow.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    const Config::IAState ia_state =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false }
      }
    };

    const Config::RCState rc_state =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(shadow_resolution), float(shadow_resolution), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      Config::COMPARISON_LESS //depth_comparison
    };

    const Config::OMState om_state =
    {
      false,
      {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    shadowmap_config = wrap.GetCore()->GetDevice()->CreateConfig("spark_shadowmap_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS),
      {},
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }

  void Spark::CreateShadowmapPass(uint32_t index)
  {
    const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    auto shadowmap_graphic_arguments = std::vector<std::shared_ptr<View>>(instance_count);
    for (uint32_t j = 0; j < uint32_t(shadowmap_graphic_arguments.size()); ++j)
    {
      shadowmap_graphic_arguments[j] = graphic_arguments->CreateView("spark_shadowmap_graphic_argument_" + std::to_string(index) + "_" + std::to_string(j),
        Usage(USAGE_COMMAND_INDIRECT),
        { j * uint32_t(sizeof(Pass::Argument)), uint32_t(sizeof(Pass::Argument)) }
      );
    }
    std::vector<Pass::Command> commands(instance_count);
    for (uint32_t j = 0; j < uint32_t(commands.size()); ++j)
    {
      commands[j].view = shadowmap_graphic_arguments[j];
      commands[j].offsets = { index * uint32_t(sizeof(Frustum)) };
    }

    auto shadowmap_scene_vertices = scene_vertices->CreateView("spark_shadowmap_scene_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      shadowmap_scene_vertices,
    };

    auto shadowmap_scene_triangles = scene_triangles->CreateView("spark_shadowmap_scene_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      shadowmap_scene_triangles,
    };

    const Pass::Subpass subpasses[] = {
      { shadowmap_config, shadowmap_layout, std::move(commands), {va_views, va_views + std::size(va_views)}, {ia_views, ia_views + std::size(ia_views)} }
    };

    auto shadowmap_shadow_map = shadow_map->CreateView("spark_shadowmap_shadow_map_" + std::to_string(index),
      Usage(USAGE_DEPTH_STENCIL),
      { index, 1 }
    );

    const Pass::DSAttachment ds_attachments[] = {
      { shadowmap_shadow_map, { 1.0f, std::nullopt }},
    };

    shadowmap_passes[index] = wrap.GetCore()->GetDevice()->CreatePass("spark_shadowmap_pass_" + std::to_string(index),
      Pass::TYPE_GRAPHIC,
      { subpasses, uint32_t(std::size(subpasses)) },
      {},
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Spark::CreateUnshadowedLayout()
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
      unshadowed_shadow_data,
    };


    auto unshadowed_scene_instances = scene_instances->CreateView("spark_unshadowed_scene_instances",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Instance)) }
    );

    const std::shared_ptr<View> ue_views[] = {
      unshadowed_scene_instances
    };


    auto unshadowed_scene_textures0 = scene_textures0->CreateView("spark_unshadowed_scene_textures0",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto unshadowed_scene_textures1 = scene_textures1->CreateView("spark_unshadowed_scene_textures1",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto unshadowed_scene_textures2 = scene_textures2->CreateView("spark_unshadowed_scene_textures2",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto unshadowed_scene_textures3 = scene_textures3->CreateView("spark_unshadowed_scene_textures3",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto unshadowed_light_maps = light_maps->CreateView("spark_unshadowed_light_maps",
      Usage(USAGE_SHADER_RESOURCE)
    );

    const std::shared_ptr<View> ri_views[] = {
      unshadowed_scene_textures0,
      unshadowed_scene_textures1,
      unshadowed_scene_textures2,
      unshadowed_scene_textures3,
      unshadowed_light_maps,
    };

    const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };

    unshadowed_layout = wrap.GetCore()->GetDevice()->CreateLayout("spark_unshadowed_layout",
      { ub_views, uint32_t(std::size(ub_views)) },
      { ue_views, uint32_t(std::size(ue_views)) },
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {},
      { samplers, uint32_t(std::size(samplers)) },
      {}
    );
  }

  void Spark::CreateUnshadowedConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_simple.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::pair<std::string, std::string> defines[] =
    {
      {"TEST", "1"},
    };

    const Config::IAState ia_state =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
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
    
    const Config::RCState rc_state =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      Config::COMPARISON_LESS //depth_comparison
    };

    const Config::OMState om_state =
    {
      false,
      {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    unshadowed_config = wrap.GetCore()->GetDevice()->CreateConfig("spark_unshadowed_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines, uint32_t(std::size(defines)) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }

  void Spark::CreateUnshadowedPass()
  {
    Pass::Subpass subpasses[ShadingSubpass::SUBPASS_MAX_COUNT] = {};
    {
      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      auto unshadowed_graphic_arguments = std::vector<std::shared_ptr<View>>(instance_count);
      for (uint32_t j = 0; j < uint32_t(unshadowed_graphic_arguments.size()); ++j)
      {
        unshadowed_graphic_arguments[j] = graphic_arguments->CreateView("spark_unshadowed_graphic_argument_" + std::to_string(j),
          Usage(USAGE_COMMAND_INDIRECT),
          { j * uint32_t(sizeof(Pass::Argument)), uint32_t(sizeof(Pass::Argument)) }
        );
      }
      std::vector<Pass::Command> unshadowed_commands(instance_count);
      for (uint32_t j = 0; j < uint32_t(unshadowed_commands.size()); ++j)
      {
        unshadowed_commands[j].view = unshadowed_graphic_arguments[j];
        unshadowed_commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
      }

      auto unshadowed_scene_vertices = scene_vertices->CreateView("spark_unshadowed_scene_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      std::vector<std::shared_ptr<View>> unshadowed_va_views = {
        unshadowed_scene_vertices,
      };

      auto unshadowed_scene_triangles = scene_triangles->CreateView("spark_unshadowed_scene_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      std::vector<std::shared_ptr<View>> unshadowed_ia_views = {
        unshadowed_scene_triangles,
      };

      subpasses[ShadingSubpass::SUBPASS_OPAQUE] =
      {
        unshadowed_config, unshadowed_layout,
        std::move(unshadowed_commands),
        std::move(unshadowed_va_views),
        std::move(unshadowed_ia_views)
      };
    }
    {
      const Pass::Command skybox_commands[] = {
        {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
      };

      auto skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      const std::shared_ptr<View> skybox_va_views[] = {
        skybox_skybox_vertices,
      };

      auto skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      const std::shared_ptr<View> skybox_ia_views[] = {
        skybox_skybox_triangles,
      };

      subpasses[ShadingSubpass::SUBPASS_SKYBOX] =
      {
        skybox_config, skybox_layout,
        { skybox_commands, skybox_commands + std::size(skybox_commands) },
        { skybox_va_views, skybox_va_views + std::size(skybox_va_views) },
        { skybox_ia_views, skybox_ia_views + std::size(skybox_ia_views) }
      };
    }

    auto unshadowed_color_target = color_target->CreateView("spark_unshadowed_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      unshadowed_color_target, std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    };

    auto unshadowed_depth_target = depth_target->CreateView("spark_unshadowed_depth_target",
      Usage(USAGE_DEPTH_STENCIL)
    );
    const Pass::DSAttachment ds_attachments[] = {
      unshadowed_depth_target, { 1.0f, std::nullopt },
    };

    unshadowed_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_unshadowed_pass",
      Pass::TYPE_GRAPHIC,
      { subpasses, uint32_t(std::size(subpasses)) },
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Spark::CreateShadowedLayout()
  {
    auto shadowed_screen_data = screen_data->CreateView("spark_shadowed_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto shadowed_camera_data = camera_data->CreateView("spark_shadowed_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto shadowed_shadow_data = shadow_data->CreateView("spark_shadowed_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, sizeof(Frustum) }
    );
    const std::shared_ptr<View> ub_views[] = {
      shadowed_screen_data,
      shadowed_camera_data,
      shadowed_shadow_data,
    };
    //shadowed_layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

    auto shadowed_scene_instances = scene_instances->CreateView("spark_shadowed_scene_instances",
      Usage(USAGE_CONSTANT_DATA),
      { 0, sizeof(Instance) }
    );
    const std::shared_ptr<View> ue_views[] = {
      shadowed_scene_instances
    };
    //shadowed_layout->UpdateSBViews({ ue_views, uint32_t(std::size(ue_views)) });

    auto shadowed_scene_textures0 = scene_textures0->CreateView("spark_shadowed_scene_textures0",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_scene_textures1 = scene_textures1->CreateView("spark_shadowed_scene_textures1",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_scene_textures2 = scene_textures2->CreateView("spark_shadowed_scene_textures2",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_scene_textures3 = scene_textures3->CreateView("spark_shadowed_scene_textures3",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_shadow_map = shadow_map->CreateView("spark_shadowed_shadow_map",
      Usage(USAGE_SHADER_RESOURCE),
      View::Bind(View::BIND_CUBEMAP_LAYER)
    );
    const std::shared_ptr<View> ri_views[] = {
      shadowed_scene_textures0,
      shadowed_scene_textures1,
      shadowed_scene_textures2,
      shadowed_scene_textures3,
      shadowed_shadow_map,
    };

    const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      { Layout::Sampler::FILTERING_NEAREST, 1, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_ALWAYS, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
    };

    shadowed_layout = wrap.GetCore()->GetDevice()->CreateLayout("spark_shadowed_layout",
      { ub_views, uint32_t(std::size(ub_views)) },
      { ue_views, uint32_t(std::size(ue_views)) },
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {},
      { samplers, uint32_t(std::size(samplers)) },
      {}
    );
  }


  void Spark::CreateShadowedConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_advanced.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::pair<std::string, std::string> defines[] =
    {
      {"TEST", "1"},
    };

    const Config::IAState ia_state =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
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

    const Config::RCState rc_state =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      Config::COMPARISON_LESS //depth_comparison
    };

    const Config::OMState om_state =
    {
      false,
      {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    shadowed_config = wrap.GetCore()->GetDevice()->CreateConfig("spark_shadowed_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines, uint32_t(std::size(defines)) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }


  void Spark::CreateShadowedPass()
  {
    Pass::Subpass subpasses[ShadingSubpass::SUBPASS_MAX_COUNT] = {};
    {
      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      auto shadowed_graphic_arguments = std::vector<std::shared_ptr<View>>(instance_count);
      for (uint32_t j = 0; j < uint32_t(shadowed_graphic_arguments.size()); ++j)
      {
        shadowed_graphic_arguments[j] = graphic_arguments->CreateView("spark_shadowed_graphic_argument_" + std::to_string(j),
          Usage(USAGE_COMMAND_INDIRECT),
          { j * uint32_t(sizeof(Pass::Argument)), uint32_t(sizeof(Pass::Argument)) }
        );
      }
      std::vector<Pass::Command> shadowed_commands(instance_count);
      for (uint32_t j = 0; j < uint32_t(shadowed_commands.size()); ++j)
      {
        shadowed_commands[j].view = shadowed_graphic_arguments[j];
        shadowed_commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
      }

      auto shadowed_scene_vertices = scene_vertices->CreateView("spark_shadowed_scene_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      std::vector<std::shared_ptr<View>> shadowed_va_views = {
        shadowed_scene_vertices,
      };

      auto shadowed_scene_triangles = scene_triangles->CreateView("spark_shadowed_scene_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      std::vector<std::shared_ptr<View>> shadowed_ia_views = {
        shadowed_scene_triangles,
      };

      subpasses[ShadingSubpass::SUBPASS_OPAQUE] =
      {
        shadowed_config, shadowed_layout,
        std::move(shadowed_commands),
        std::move(shadowed_va_views),
        std::move(shadowed_ia_views)
      };
    }
    {
      const Pass::Command skybox_commands[] = {
        {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
      };

      auto skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      const std::shared_ptr<View> skybox_va_views[] = {
        skybox_skybox_vertices,
      };

      auto skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      const std::shared_ptr<View> skybox_ia_views[] = {
        skybox_skybox_triangles,
      };

      subpasses[ShadingSubpass::SUBPASS_SKYBOX] =
      {
        skybox_config, skybox_layout,
        { skybox_commands, skybox_commands + std::size(skybox_commands) },
        { skybox_va_views, skybox_va_views + std::size(skybox_va_views) },
        { skybox_ia_views, skybox_ia_views + std::size(skybox_ia_views) }
      };
    }

    auto shadowed_color_target = color_target->CreateView("spark_shadowed_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      shadowed_color_target, std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    };

    auto shadowed_depth_target = depth_target->CreateView("spark_shadowed_depth_target",
      Usage(USAGE_DEPTH_STENCIL)
    );
    const Pass::DSAttachment ds_attachments[] = {
      shadowed_depth_target, { 1.0f, std::nullopt },
    };

    shadowed_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_shadowed_pass",
      Pass::TYPE_GRAPHIC,
      { subpasses, uint32_t(std::size(subpasses)) },
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Spark::CreateSkyboxLayout()
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
    //skybox_layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

    auto skybox_skybox_texture = skybox_texture->CreateView("spark_skybox_skybox_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      skybox_skybox_texture,
    };
    //skybox_layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };
    //skybox_layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });

    skybox_layout = wrap.GetCore()->GetDevice()->CreateLayout("spark_skybox_layout",
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {},
      { samplers, uint32_t(std::size(samplers)) },
      {}
    );
  }

  void Spark::CreateSkyboxConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_environment.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::pair<std::string, std::string> defines[] =
    {
      { "TEST", "1" },
    };

    const Config::IAState ia_state =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0, 0, 16, FORMAT_R32G32_FLOAT, false },
        { 0, 8, 16, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Config::RCState rc_state =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_state =
    {
      true, //depth_enabled
      false, //depth_write
      Config::COMPARISON_EQUAL //depth_comparison
    };

    const Config::OMState om_state =
    {
      false,
      {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    skybox_config = wrap.GetCore()->GetDevice()->CreateConfig("spark_skybox_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines, uint32_t(std::size(defines)) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }

  void Spark::CreateSkyboxPass()
  {
    const Pass::Command skybox_commands[] = {
      {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
    };

    auto skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> skybox_va_views[] = {
      skybox_skybox_vertices,
    };

    auto skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> skybox_ia_views[] = {
      skybox_skybox_triangles,
    };

    Pass::Subpass subpasses[] =
    {
      skybox_config, skybox_layout,
      { skybox_commands, skybox_commands + std::size(skybox_commands) },
      { skybox_va_views, skybox_va_views + std::size(skybox_va_views) },
      { skybox_ia_views, skybox_ia_views + std::size(skybox_ia_views) }
    };

    auto skybox_color_target = color_target->CreateView("spark_skybox_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      skybox_color_target, std::nullopt,
    };

    auto skybox_depth_target = depth_target->CreateView("spark_skybox_depth_target",
      Usage(USAGE_DEPTH_STENCIL)
    );
    const Pass::DSAttachment ds_attachments[] = {
      skybox_depth_target, { std::nullopt, std::nullopt },
    };

    skybox_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_skybox_pass",
      Pass::TYPE_GRAPHIC,
      { subpasses, uint32_t(std::size(subpasses)) },
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Spark::CreatePresentLayout()
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

    present_layout = wrap.GetCore()->GetDevice()->CreateLayout("spark_present_layout",
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      { wi_views, uint32_t(std::size(wi_views)) },
      {},
      {},
      {},
      {}
    );
  }

  void Spark::CreatePresentConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_present.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    present_config = wrap.GetCore()->GetDevice()->CreateConfig("spark_present_config",
      shader_ss.str(),
      Config::COMPILATION_CS,
      {},
      {},
      {},
      {},
      {}
    );
  }

  void Spark::CreatePresentPass()
  {
    auto present_compute_arguments = compute_arguments->CreateView("spark_present_compute_arguments",
      Usage(USAGE_COMMAND_INDIRECT)
    );
    Pass::Command present_commands[] = {
      { present_compute_arguments },
    };
    //present_pass->UpdateSubpassCommands(0, { commands, uint32_t(std::size(commands)) });

    //present_pass->SetSubpassLayout(0, present_layout);
    //present_pass->SetSubpassConfig(0, present_config);

    Pass::Subpass subpasses[] =
    {
      present_config, present_layout,
      { present_commands, present_commands + std::size(present_commands) },
      {},
      {}
    };

    present_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_present_pass",
      Pass::TYPE_COMPUTE,
      { subpasses, uint32_t(std::size(subpasses)) },
      {},
      {}
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

  void Spark::DestroySkyboxVertices()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(skybox_vertices);
    skybox_vertices.reset();
  }

  void Spark::DestroySkyboxTriangles()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(skybox_triangles);
    skybox_triangles.reset();
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

  void Spark::DestroyShadowmapLayout()
  {
    wrap.GetCore()->GetDevice()->DestroyLayout(shadowmap_layout);
    shadowmap_layout.reset();
  }

  void Spark::DestroyShadowmapConfig()
  {
    wrap.GetCore()->GetDevice()->DestroyConfig(shadowmap_config);
    shadowmap_config.reset();
  }

  void Spark::DestroyShadowmapPass(uint32_t index)
  {
    wrap.GetCore()->GetDevice()->DestroyPass(shadowmap_passes[index]);
    shadowmap_passes[index].reset();
  }

  void Spark::DestroyShadowedLayout()
  {
    wrap.GetCore()->GetDevice()->DestroyLayout(shadowed_layout);
    shadowed_layout.reset();
  }

  void Spark::DestroyShadowedConfig()
  {
    wrap.GetCore()->GetDevice()->DestroyConfig(shadowed_config);
    shadowed_config.reset();
  }

  void Spark::DestroyShadowedPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(shadowed_pass);
    shadowed_pass.reset();
  }

  void Spark::DestroyUnshadowedLayout()
  {
    wrap.GetCore()->GetDevice()->DestroyLayout(unshadowed_layout);
    unshadowed_layout.reset();
  }

  void Spark::DestroyUnshadowedConfig()
  {
    wrap.GetCore()->GetDevice()->DestroyConfig(unshadowed_config);
    unshadowed_config.reset();
  }

  void Spark::DestroyUnshadowedPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(unshadowed_pass);
    unshadowed_pass.reset();
  }

  void Spark::DestroySkyboxLayout()
  {
    wrap.GetCore()->GetDevice()->DestroyLayout(skybox_layout);
    skybox_layout.reset();
  }

  void Spark::DestroySkyboxConfig()
  {
    wrap.GetCore()->GetDevice()->DestroyConfig(skybox_config);
    skybox_config.reset();
  }

  void Spark::DestroySkyboxPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(skybox_pass);
    skybox_pass.reset();
  }

  void Spark::DestroyPresentLayout()
  {
    wrap.GetCore()->GetDevice()->DestroyLayout(present_layout);
    present_layout.reset();
  }

  void Spark::DestroyPresentConfig()
  {
    wrap.GetCore()->GetDevice()->DestroyConfig(present_config);
    present_config.reset();
  }

  void Spark::DestroyPresentPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(present_pass);
    present_pass.reset();
  }

  void Spark::Use()
  {
    switch (shadows)
    {
    case NO_SHADOWS:
    {
      shadowmap_passes[0]->SetEnabled(false);
      shadowmap_passes[1]->SetEnabled(false);
      shadowmap_passes[2]->SetEnabled(false);
      shadowmap_passes[3]->SetEnabled(false);
      shadowmap_passes[4]->SetEnabled(false);
      shadowmap_passes[5]->SetEnabled(false);
      unshadowed_pass->SetEnabled(true);
      shadowed_pass->SetEnabled(false);
      //skybox_pass->SetEnabled(true);
      present_pass->SetEnabled(true);
      break;
    }
    case POINT_SHADOWS:
    {
      shadowmap_passes[0]->SetEnabled(true);
      shadowmap_passes[1]->SetEnabled(true);
      shadowmap_passes[2]->SetEnabled(true);
      shadowmap_passes[3]->SetEnabled(true);
      shadowmap_passes[4]->SetEnabled(true);
      shadowmap_passes[5]->SetEnabled(true);
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(true);
      //skybox_pass->SetEnabled(true);
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
      //skybox_pass->SetEnabled(false);
      present_pass->SetEnabled(false);
      break;
    }
    }


    {
      auto graphic_arg = reinterpret_cast<Pass::Argument*>(graphic_arguments->Map());

      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      for (uint32_t i = 0; i < instance_count; ++i)
      {
        graphic_arg[i].idx_count = instance_data[i].prim_count * 3;
        graphic_arg[i].ins_count = 1;
        graphic_arg[i].idx_offset = instance_data[i].prim_offset * 3;
        graphic_arg[i].vtx_offset = instance_data[i].vert_offset * 1;
        graphic_arg[i].ins_offset = 0;
      }

      graphic_arguments->Unmap();
    }

    {
      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();

      auto compute_arg = reinterpret_cast<Pass::Argument*>(compute_arguments->Map());
      {
        compute_arg->grid_x = extent_x / 8;
        compute_arg->grid_y = extent_y / 8;
        compute_arg->grid_z = 1;
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


    CreateColorTarget();
    CreateDepthTarget();
    CreateShadowMap();

    CreateScreenData();
    CreateCameraData();
    CreateShadowData();

    CreateSceneInstances();
    CreateSceneTriangles();
    CreateSceneVertices();

    CreateSceneTextures0();
    CreateSceneTextures1();
    CreateSceneTextures2();
    CreateSceneTextures3();

    CreateLightMaps();

    CreateSkyboxVertices();
    CreateSkyboxTriangles();
    CreateSkyboxTexture();

    CreateGraphicArguments();
    CreateComputeArguments();

    CreateShadowmapLayout();
    CreateUnshadowedLayout();
    CreateShadowedLayout();
    CreateSkyboxLayout();
    CreatePresentLayout();

    CreateShadowmapConfig();
    CreateUnshadowedConfig();
    CreateShadowedConfig();
    CreateSkyboxConfig();
    CreatePresentConfig();

    CreateShadowmapPass(0);
    CreateShadowmapPass(1);
    CreateShadowmapPass(2);
    CreateShadowmapPass(3);
    CreateShadowmapPass(4);
    CreateShadowmapPass(5);
    CreateUnshadowedPass();
    CreateShadowedPass();
    CreatePresentPass();
  }

  Spark::~Spark()
  {
    DestroyPresentPass();
    DestroyPresentLayout();
    DestroyPresentConfig();
    
    DestroySkyboxPass();
    DestroySkyboxLayout();
    DestroySkyboxConfig();
    
    DestroyUnshadowedPass();
    DestroyUnshadowedLayout();
    DestroyUnshadowedConfig();

    DestroyShadowedPass();
    DestroyShadowedLayout();
    DestroyShadowedConfig();

    DestroyShadowmapPass(5);
    DestroyShadowmapPass(4);
    DestroyShadowmapPass(3);
    DestroyShadowmapPass(2);
    DestroyShadowmapPass(1);
    DestroyShadowmapPass(0);
    DestroyShadowmapLayout();
    DestroyShadowmapConfig();

    DestroyGraphicArguments();
    DestroyComputeArguments();

    DestroySkyboxVertices();
    DestroySkyboxTriangles();
    DestroySkyboxTexture();

    DestroyLightMaps();

    DestroySceneTextures0();
    DestroySceneTextures1();
    DestroySceneTextures2();
    DestroySceneTextures3();

    DestroySceneInstances();
    DestroySceneTriangles();
    DestroySceneVertices();

    DestroyScreenData();
    DestroyCameraData();
    DestroyShadowData();

    DestroyColorTarget();
    DestroyDepthTarget();
    DestroyShadowMap();
  }
}