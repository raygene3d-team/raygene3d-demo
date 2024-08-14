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


#include "cubemap_shadow.h"

namespace RayGene3D
{
  void CubemapShadow::CreateShadowedPass()
  {
    const auto size_x = scope.prop_extent_x->GetUint();
    const auto size_y = scope.prop_extent_y->GetUint();
    const auto layers = 1u;

    auto shadowed_color_target = scope.color_target->CreateView("spark_shadowed_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      shadowed_color_target, std::nullopt,
    };

    shadowed_pass = scope.core->GetDevice()->CreatePass("spark_shadowed_pass",
      Pass::TYPE_GRAPHIC,
      size_x,
      size_y,
      layers,
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      {}
    );
  }

  void CubemapShadow::CreateShadowedConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadowed.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const Config::IAState ia_Config =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0, 0, 8, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Config::RCState rc_Config =
    {
      Config::FILL_SOLID,
      Config::CULL_BACK,
      {
        { 0.0f, 0.0f, float(scope.prop_extent_x->GetUint()), float(scope.prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_Config =
    {
      false, //depth_enabled
      false, //depth_write
      Config::COMPARISON_ALWAYS //depth_comparison
    };

    const Config::OMState om_Config =
    {
      false,
      {
        { true, Config::OPERAND_ONE, Config::OPERAND_ONE, Config::OPERATION_ADD, Config::OPERAND_ONE, Config::OPERAND_ONE, Config::OPERATION_ADD, 0xF },
      }
    };

    shadowed_Config = shadowed_pass->CreateConfig("spark_shadowed_Config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_Config,
      rc_Config,
      ds_Config,
      om_Config
    );
  }


  void CubemapShadow::CreateShadowedBatch()
  {
    auto shadowed_screen_quad_vertices = scope.screen_quad_vertices->CreateView("spark_shadowed_screen_quad_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    auto shadowed_screen_quad_triangles = scope.screen_quad_triangles->CreateView("spark_shadowed_screen_quad_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const Batch::Entity entities[] = {
      {{shadowed_screen_quad_vertices}, {shadowed_screen_quad_triangles}, nullptr, { 0u, 1u }, { 0u, 4u }, { 0u, 6u } }
    };

    auto shadowed_screen_data = scope.screen_data->CreateView("spark_shadowed_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto shadowed_camera_data = scope.camera_data->CreateView("spark_shadowed_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto shadowed_shadow_data = scope.shadow_data->CreateView("spark_shadowed_shadow_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    const std::shared_ptr<View> ub_views[] = {
      shadowed_screen_data,
      shadowed_camera_data,
      shadowed_shadow_data
    };

    auto shadowed_gbuffer_0_texture = scope.gbuffer_0_target->CreateView("spark_shadowed_gbuffer_0_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_gbuffer_1_texture = scope.gbuffer_1_target->CreateView("spark_shadowed_gbuffer_1_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_depth_texture = scope.depth_target->CreateView("spark_shadowed_depth_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto shadowed_shadow_map = scope.shadow_map->CreateView("spark_shadowed_shadow_map",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      shadowed_gbuffer_0_texture,
      shadowed_gbuffer_1_texture,
      shadowed_depth_texture,
      shadowed_shadow_map,
    };

    const Batch::Sampler samplers[] = {
      { Batch::Sampler::FILTERING_LINEAR, 1, Batch::Sampler::ADDRESSING_CLAMP, Batch::Sampler::COMPARISON_LESS },
    };

    shadowed_batch = shadowed_Config->CreateBatch("spark_shadowed_batch",
      { entities, uint32_t(std::size(entities)) },
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) }
    );
  }

  void CubemapShadow::CreateShadowmapPass()
  {
    const auto size_x = scope.shadow_resolution;
    const auto size_y = scope.shadow_resolution;
    const auto layers = 1u;

    auto shadowmap_shadow_map = scope.shadow_map->CreateView("spark_shadowmap_shadow_map",
      Usage(USAGE_DEPTH_STENCIL),
      { 0u, uint32_t(-1) }
    );

    const Pass::DSAttachment ds_attachments[] = {
      { shadowmap_shadow_map, { 1.0f, std::nullopt }},
    };

    shadowmap_pass = scope.core->GetDevice()->CreatePass("spark_shadowmap_pass",
      Pass::TYPE_GRAPHIC,
      size_x,
      size_y,
      layers,
      {},
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }


  void CubemapShadow::CreateShadowmapConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadowmap.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    const Config::IAState ia_Config =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false }
      }
    };

    const Config::RCState rc_Config =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(scope.shadow_resolution), float(scope.shadow_resolution), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_Config =
    {
      true, //depth_enabled
      true, //depth_write
      Config::COMPARISON_LESS //depth_comparison
    };

    const Config::OMState om_Config =
    {
      false,
      {
        { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    shadowmap_Config = shadowmap_pass->CreateConfig("spark_shadowmap_Config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS),
      {},
      ia_Config,
      rc_Config,
      ds_Config,
      om_Config
    );
  }

  void CubemapShadow::CreateShadowmapBatch()
  {
    const auto [data, count] = scope.prop_instances->GetTypedBytes<Instance>(0);
    auto entities = std::vector<Batch::Entity>(count);
    for (auto i = 0u; i < count; ++i)
    {
      const auto shadowmap_scene_vertices = scope.scene_vertices->CreateView("spark_shadowmap_scene_vertices_" + std::to_string(i),
        Usage(USAGE_VERTEX_ARRAY)
      );

      const auto shadowmap_scene_triangles = scope.scene_triangles->CreateView("spark_shadowmap_scene_triangles_" + std::to_string(i),
        Usage(USAGE_INDEX_ARRAY)
      );

      const auto shadowmap_graphic_arguments = scope.graphic_arguments->CreateView("spark_shadowmap_graphic_argument_" + std::to_string(i),
        Usage(USAGE_ARGUMENT_LIST),
        { uint32_t(sizeof(Batch::Graphic)) * i, uint32_t(sizeof(Batch::Graphic)) }
      );

      const auto& ins_range = View::Range{ 0u,  1u };
      const auto& vtx_range = View::Range{ data[i].vert_offset * 1, data[i].vert_count * 1 };
      const auto& idx_range = View::Range{ data[i].prim_offset * 3, data[i].prim_count * 3 };
      const auto& sb_offset = std::nullopt;
      const auto& push_data = std::nullopt;

      entities[i] = {
        { shadowmap_scene_vertices },
        { shadowmap_scene_triangles },
        nullptr, //shadowmap_graphic_arguments,
        ins_range,
        vtx_range,
        idx_range,
        sb_offset,
        push_data
      };
    }

    auto shadowmap_shadow_data = scope.shadow_data->CreateView("spark_shadowmap_shadow_data",
      USAGE_CONSTANT_DATA,
      { 0u, uint32_t(sizeof(Frustum)) }
    );

    const std::shared_ptr<View> ub_views[] = {
      shadowmap_shadow_data,
    };

    shadowmap_batch = shadowmap_Config->CreateBatch("spark_shadowmap_batch",
      { entities.data(), uint32_t(entities.size()) },
      {},
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      {},
      {},
      {},
      {}
    );
  }


  void CubemapShadow::DestroyShadowmapBatch()
  {
    shadowmap_Config->DestroyBatch(shadowmap_batch);
    shadowmap_batch.reset();
  }

  void CubemapShadow::DestroyShadowmapConfig()
  {
    shadowmap_pass->DestroyConfig(shadowmap_Config);
    shadowmap_Config.reset();
  }

  void CubemapShadow::DestroyShadowmapPass()
  {
    scope.core->GetDevice()->DestroyPass(shadowmap_pass);
    shadowmap_pass.reset();
  }

  void CubemapShadow::DestroyShadowedBatch()
  {
    shadowed_Config->DestroyBatch(shadowed_batch);
    shadowed_batch.reset();
  }

  void CubemapShadow::DestroyShadowedConfig()
  {
    shadowed_pass->DestroyConfig(shadowed_Config);
    shadowed_Config.reset();
  }

  void CubemapShadow::DestroyShadowedPass()
  {
    scope.core->GetDevice()->DestroyPass(shadowed_pass);
    shadowed_pass.reset();
  }

  void CubemapShadow::Enable()
  {
    shadowmap_pass->SetEnabled(true);
    geometry_pass->SetEnabled(true);
    shadowed_pass->SetEnabled(true);
    present_pass->SetEnabled(true);
  }

  void CubemapShadow::Disable()
  {

    shadowmap_pass->SetEnabled(false);
    geometry_pass->SetEnabled(false);
    shadowed_pass->SetEnabled(false);
    present_pass->SetEnabled(false);
  }

  CubemapShadow::CubemapShadow(const Render3DScope& scope)
    : Render3DMode(scope)
  {

    CreateShadowmapPass();
    CreateShadowmapConfig();
    CreateShadowmapBatch();

    CreateGeometryPass();
    CreateGeometryConfig();
    CreateGeometryBatch();

    CreateSkyboxConfig();
    CreateSkyboxBatch();

    CreateShadowedPass();
    CreateShadowedConfig();
    CreateShadowedBatch();

    CreatePresentPass();
    CreatePresentConfig();
    CreatePresentBatch();
  }

  CubemapShadow::~CubemapShadow()
  {
    DestroyPresentBatch();
    DestroyPresentConfig();
    DestroyPresentPass();

    DestroyShadowedBatch();
    DestroyShadowedConfig();
    DestroyShadowedPass();

    DestroySkyboxBatch();
    DestroySkyboxConfig();

    DestroyGeometryBatch();
    DestroyGeometryConfig();
    DestroyGeometryPass();

    DestroyShadowmapBatch();
    DestroyShadowmapConfig();
    DestroyShadowmapPass();
  }
}