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
  namespace Render3D
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
        { rt_attachments, std::size(rt_attachments) },
        {}
      );
    }

    void CubemapShadow::CreateShadowedConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_shadowed.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      shader_fs.close();

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

      shadowed_config = shadowed_pass->CreateConfig("spark_shadowed_config",
        shader_ss.str(),
        Config::Compilation(Config::COMPILATION_VERT | Config::COMPILATION_FRAG),
        { defines.data(), defines.size() },
        ia_Config,
        rc_Config,
        ds_Config,
        om_Config
      );
    }


    void CubemapShadow::CreateShadowedBatch()
    {
      const auto& shadowed_screen_quad_vertices = scope.screen_quad_vertices->CreateView("spark_shadowed_screen_quad_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      const auto& shadowed_screen_quad_triangles = scope.screen_quad_triangles->CreateView("spark_shadowed_screen_quad_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      const Batch::Entity entities[] = {
        {{shadowed_screen_quad_vertices}, {shadowed_screen_quad_triangles}, nullptr, { 0u, 1u }, { 0u, 4u }, { 0u, 6u } }
      };

      const auto& shadowed_screen_data = scope.screen_data->CreateView("spark_shadowed_screen_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const auto& shadowed_camera_data = scope.camera_data->CreateView("spark_shadowed_camera_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const auto& shadowed_shadow_data = scope.shadow_data->CreateView("spark_shadowed_shadow_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const std::shared_ptr<View> ub_views[] = {
        shadowed_screen_data,
        shadowed_camera_data,
        shadowed_shadow_data
      };

      const auto& shadowed_gbuffer_0_texture = scope.gbuffer_0_target->CreateView("spark_shadowed_gbuffer_0_texture",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& shadowed_gbuffer_1_texture = scope.gbuffer_1_target->CreateView("spark_shadowed_gbuffer_1_texture",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& shadowed_depth_texture = scope.depth_target->CreateView("spark_shadowed_depth_texture",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& shadowed_shadow_map = scope.shadow_map->CreateView("spark_shadowed_shadow_map",
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

      shadowed_batch = shadowed_config->CreateBatch("spark_shadowed_batch",
        { entities, std::size(entities) },
        { samplers, std::size(samplers) },
        { ub_views, std::size(ub_views) },
        {},
        { ri_views, std::size(ri_views) }
      );
    }

    void CubemapShadow::CreateShadowmapPass()
    {
      const auto size_x = scope.shadow_resolution;
      const auto size_y = scope.shadow_resolution;
      const auto layers = 1u;

      const auto& shadowmap_shadow_map = scope.shadow_map->CreateView("spark_shadowmap_shadow_map",
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
        { ds_attachments, std::size(ds_attachments) }
      );
    }


    void CubemapShadow::CreateShadowmapConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_shadowmap.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      shader_fs.close();

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

      shadowmap_config = shadowmap_pass->CreateConfig("spark_shadowmap_config",
        shader_ss.str(),
        Config::Compilation(Config::COMPILATION_VERT),
        {},
        ia_Config,
        rc_Config,
        ds_Config,
        om_Config
      );
    }

    void CubemapShadow::CreateShadowmapBatch()
    {
      const auto [data, count] = scope.prop_inst->GetObjectItem("binary")->GetRawItems<Instance>(0);
      auto entities = std::vector<Batch::Entity>(count);
      for (size_t i = 0u; i < count; ++i)
      {
        const auto& shadowmap_scene_vertices = scope.scene_buffer_vert->CreateView("spark_shadowmap_scene_vertices_" + std::to_string(i),
          Usage(USAGE_VERTEX_ARRAY)
        );

        const auto& shadowmap_scene_triangles = scope.scene_buffer_trng->CreateView("spark_shadowmap_scene_triangles_" + std::to_string(i),
          Usage(USAGE_INDEX_ARRAY)
        );

        const auto& shadowmap_graphic_arguments = scope.graphic_arguments->CreateView("spark_shadowmap_graphic_argument_" + std::to_string(i),
          Usage(USAGE_ARGUMENT_LIST),
          { sizeof(Batch::Graphic) * i, sizeof(Batch::Graphic) }
        );

        const auto& ins_range = Range{ 0u,  1u };
        const auto& vtx_range = Range{ data[i].vert_offset * 1, data[i].vert_count * 1 };
        const auto& idx_range = Range{ data[i].trng_offset * 3, data[i].trng_count * 3 };
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

      const auto& shadowmap_shadow_data = scope.shadow_data->CreateView("spark_shadowmap_shadow_data",
        USAGE_CONSTANT_DATA,
        { 0u, sizeof(Frustum) }
      );

      const std::shared_ptr<View> ub_views[] = {
        shadowmap_shadow_data,
      };

      shadowmap_batch = shadowmap_config->CreateBatch("spark_shadowmap_batch",
        { entities.data(), entities.size() },
        {},
        { ub_views, std::size(ub_views) },
        {},
        {},
        {},
        {},
        {}
      );
    }


    void CubemapShadow::DestroyShadowmapBatch()
    {
      shadowmap_config->DestroyBatch(shadowmap_batch);
      shadowmap_batch.reset();
    }

    void CubemapShadow::DestroyShadowmapConfig()
    {
      shadowmap_pass->DestroyConfig(shadowmap_config);
      shadowmap_config.reset();
    }

    void CubemapShadow::DestroyShadowmapPass()
    {
      scope.core->GetDevice()->DestroyPass(shadowmap_pass);
      shadowmap_pass.reset();
    }

    void CubemapShadow::DestroyShadowedBatch()
    {
      shadowed_config->DestroyBatch(shadowed_batch);
      shadowed_batch.reset();
    }

    void CubemapShadow::DestroyShadowedConfig()
    {
      shadowed_pass->DestroyConfig(shadowed_config);
      shadowed_config.reset();
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

    CubemapShadow::CubemapShadow(Scope& scope)
      : Mode(scope)
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
}