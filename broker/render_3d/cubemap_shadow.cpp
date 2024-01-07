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
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();
    const auto extent_z = 1u;

    auto shadowed_color_target = color_target->CreateView("spark_shadowed_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      shadowed_color_target, std::nullopt,
    };

    shadowed_pass = wrap.GetCore()->GetDevice()->CreatePass("spark_shadowed_pass",
      Pass::TYPE_GRAPHIC,
      { 0u, extent_x },
      { 0u, extent_y },
      { 0u, extent_z },
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      {}
    );
  }

  void CubemapShadow::CreateShadowedState()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadowed.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const State::IAState ia_state =
    {
      State::TOPOLOGY_TRIANGLELIST,
      State::INDEXER_32_BIT,
      {
        { 0, 0, 8, FORMAT_R32G32_FLOAT, false },
      }
    };

    const State::RCState rc_state =
    {
      State::FILL_SOLID,
      State::CULL_BACK,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const State::DSState ds_state =
    {
      false, //depth_enabled
      false, //depth_write
      State::COMPARISON_ALWAYS //depth_comparison
    };

    const State::OMState om_state =
    {
      false,
      {
        { true, State::OPERAND_ONE, State::OPERAND_ONE, State::OPERATION_ADD, State::OPERAND_ONE, State::OPERAND_ONE, State::OPERATION_ADD, 0xF },
      }
    };

    shadowed_state = shadowed_pass->CreateState("spark_shadowed_state",
      shader_ss.str(),
      State::Compilation(State::COMPILATION_VS | State::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }


  void CubemapShadow::CreateShadowedBatch()
  {
    auto shadowed_screen_quad_vertices = screen_quad_vertices->CreateView("spark_shadowed_screen_quad_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    auto shadowed_screen_quad_triangles = screen_quad_triangles->CreateView("spark_shadowed_screen_quad_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const Batch::Entity entities[] = {
      {{shadowed_screen_quad_vertices}, {shadowed_screen_quad_triangles}, nullptr, { 0u, 4u }, { 0u, 6u }, { 0u, 1u } }
    };

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
      { 0u, uint32_t(-1) },
      { 0u, uint32_t(-1) },
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

    shadowed_batch = shadowed_state->CreateBatch("spark_shadowed_batch",
      { entities, uint32_t(std::size(entities)) },
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) }
    );
  }

  void CubemapShadow::CreateShadowmapPass(uint32_t index)
  {
    const auto extent_x = shadow_resolution;
    const auto extent_y = shadow_resolution;
    const auto extent_z = 1u;

    auto shadowmap_shadow_map = shadow_map->CreateView("spark_shadowmap_shadow_map_" + std::to_string(index),
      Usage(USAGE_DEPTH_STENCIL),
      { 0u, uint32_t(-1) },
      { index, 1u }
    );

    const Pass::DSAttachment ds_attachments[] = {
      { shadowmap_shadow_map, { 1.0f, std::nullopt }},
    };

    shadowmap_passes[index] = wrap.GetCore()->GetDevice()->CreatePass("spark_shadowmap_pass_" + std::to_string(index),
      Pass::TYPE_GRAPHIC,
      { 0u, extent_x },
      { 0u, extent_y },
      { 0u, extent_z },
      {},
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }


  void CubemapShadow::CreateShadowmapState(uint32_t index)
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadowmap.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    const State::IAState ia_state =
    {
      State::TOPOLOGY_TRIANGLELIST,
      State::INDEXER_32_BIT,
      {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false }
      }
    };

    const State::RCState rc_state =
    {
      State::FILL_SOLID,
      State::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(shadow_resolution), float(shadow_resolution), 0.0f, 1.0f }
      },
    };

    const State::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      State::COMPARISON_LESS //depth_comparison
    };

    const State::OMState om_state =
    {
      false,
      {
        { false, State::OPERAND_SRC_ALPHA, State::OPERAND_INV_SRC_ALPHA, State::OPERATION_ADD, State::OPERAND_INV_SRC_ALPHA, State::OPERAND_ZERO, State::OPERATION_ADD, 0xF }
      }
    };

    shadowmap_states[index] = shadowmap_passes[index]->CreateState("spark_shadowmap_state" + std::to_string(index),
      shader_ss.str(),
      State::Compilation(State::COMPILATION_VS),
      {},
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }

  void CubemapShadow::CreateShadowmapBatch(uint32_t index)
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    auto entities = std::vector<Batch::Entity>(count);
    for (auto i = 0u; i < count; ++i)
    {
      const auto shadowmap_scene_vertices = scene_vertices->CreateView("spark_shadowmap_scene_vertices_" + std::to_string(index) + "_" + std::to_string(i),
        Usage(USAGE_VERTEX_ARRAY)
      );

      const auto shadowmap_scene_triangles = scene_triangles->CreateView("spark_shadowmap_scene_triangles_" + std::to_string(index) + "_" + std::to_string(i),
        Usage(USAGE_INDEX_ARRAY)
      );

      const auto shadowmap_graphic_arguments = graphic_arguments->CreateView("spark_shadowmap_graphic_argument_" + std::to_string(index) + "_" + std::to_string(i),
        Usage(USAGE_ARGUMENT_INDIRECT),
        { uint32_t(sizeof(Batch::Graphic)) * i, uint32_t(sizeof(Batch::Graphic)) }
      );

      const auto& vtx_range = View::Range{ data[i].vert_offset * 1, data[i].vert_count * 1 };
      const auto& idx_range = View::Range{ data[i].prim_offset * 3, data[i].prim_count * 3 };
      const auto& ins_range = View::Range{ 0u,  1u };
      const auto& sb_offset = std::array<uint32_t, 4>{ uint32_t(sizeof(Frustum))* index, 0u, 0u, 0u };
      const auto& push_data = std::nullopt;

      entities[i] = {
        { shadowmap_scene_vertices },
        { shadowmap_scene_triangles },
        nullptr, //shadowmap_graphic_arguments,
        vtx_range,
        idx_range,
        ins_range,
        sb_offset,
        push_data
      };
    }

    auto shadowmap_shadow_data = shadow_data->CreateView("spark_shadowmap_shadow_data" + std::to_string(index),
      USAGE_CONSTANT_DATA,
      { 0u, uint32_t(sizeof(Frustum)) }
    );

    const std::shared_ptr<View> sb_views[] = {
      shadowmap_shadow_data,
    };

    shadowmap_batches[index] = shadowmap_states[index]->CreateBatch("spark_shadowmap_batch" + std::to_string(index),
      { entities.data(), uint32_t(entities.size()) },
      {},
      {},
      { sb_views, uint32_t(std::size(sb_views)) },
      {},
      {},
      {},
      {}
    );
  }


  void CubemapShadow::DestroyShadowmapBatch(uint32_t index)
  {
    shadowmap_states[index]->DestroyBatch(shadowmap_batches[index]);
    shadowmap_batches[index].reset();
  }

  void CubemapShadow::DestroyShadowmapState(uint32_t index)
  {
    shadowmap_passes[index]->DestroyState(shadowmap_states[index]);
    shadowmap_states[index].reset();
  }

  void CubemapShadow::DestroyShadowmapPass(uint32_t index)
  {
    wrap.GetCore()->GetDevice()->DestroyPass(shadowmap_passes[index]);
    shadowmap_passes[index].reset();
  }

  void CubemapShadow::DestroyShadowedBatch()
  {
    shadowed_state->DestroyBatch(shadowed_batch);
    shadowed_batch.reset();
  }

  void CubemapShadow::DestroyShadowedState()
  {
    shadowed_pass->DestroyState(shadowed_state);
    shadowed_state.reset();
  }

  void CubemapShadow::DestroyShadowedPass()
  {
    wrap.GetCore()->GetDevice()->DestroyPass(shadowed_pass);
    shadowed_pass.reset();
  }

  CubemapShadow::CubemapShadow(const Render3DTechnique& scope)
    : Render3DTechnique(scope)
  {
    for (auto i = 0u; i < 6u; ++i)
    {
      CreateShadowmapPass(i);
      CreateShadowmapState(i);
      CreateShadowmapBatch(i);
    }

    CreateShadowedPass();
    CreateShadowedState();
    CreateShadowedBatch();
  }

  CubemapShadow::~CubemapShadow()
  {
    DestroyShadowedBatch();
    DestroyShadowedState();
    DestroyShadowedPass();

    for (auto i = 0u; i < 6u; ++i)
    {
      DestroyShadowmapBatch(i);
      DestroyShadowmapState(i);
      DestroyShadowmapPass(i);
    }
  }
}