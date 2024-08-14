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


#include "sw_traced_shadow.h"

namespace RayGene3D
{
  void SWTracedShadow::CreateSWTracedPass()
  {
    const auto size_x = scope.prop_extent_x->GetUint();
    const auto size_y = scope.prop_extent_y->GetUint();
    const auto layers = 1u;

    auto sw_traced_color_target = scope.color_target->CreateView("spark_sw_traced_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      sw_traced_color_target, std::nullopt,
    };

    sw_traced_pass = scope.core->GetDevice()->CreatePass("spark_sw_traced_pass",
      Pass::TYPE_GRAPHIC,
      size_x,
      size_y,
      layers,
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      {}
    );
  }


  void SWTracedShadow::CreateSWTracedConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_sw_traced.hlsl", std::fstream::in);
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

    sw_traced_config = sw_traced_pass->CreateConfig("spark_sw_traced_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_Config,
      rc_Config,
      ds_Config,
      om_Config
    );
  }

  void SWTracedShadow::CreateSWTracedBatch()
  {
    auto shadowed_screen_quad_vertices = scope.screen_quad_vertices->CreateView("spark_sw_traced_screen_quad_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    auto shadowed_screen_quad_triangles = scope.screen_quad_triangles->CreateView("spark_sw_traced_screen_quad_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const Batch::Entity entities[] = {
      {{shadowed_screen_quad_vertices}, {shadowed_screen_quad_triangles}, nullptr, { 0u, 1u }, { 0u, 4u }, { 0u, 6u }}
    };

    auto sw_traced_screen_data = scope.screen_data->CreateView("spark_sw_traced_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto sw_traced_camera_data = scope.camera_data->CreateView("spark_sw_traced_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto sw_traced_shadow_data = scope.shadow_data->CreateView("spark_sw_traced_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, sizeof(Frustum) }
    );
    const std::shared_ptr<View> ub_views[] = {
      sw_traced_screen_data,
      sw_traced_camera_data,
      sw_traced_shadow_data,
    };

    auto sw_traced_scene_t_boxes = scope.scene_t_boxes->CreateView("spark_sw_traced_scene_t_boxes",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_scene_b_boxes = scope.scene_b_boxes->CreateView("spark_sw_traced_scene_b_boxes",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_trace_instances = scope.trace_instances->CreateView("spark_sw_traced_trace_instances",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_trace_triangles = scope.trace_triangles->CreateView("spark_sw_traced_trace_triangles",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_trace_vertices = scope.trace_vertices->CreateView("spark_sw_traced_trace_vertices",
      Usage(USAGE_SHADER_RESOURCE)
    );

    const std::shared_ptr<View> rb_views[] = {
      sw_traced_scene_t_boxes,
      sw_traced_scene_b_boxes,
      sw_traced_trace_instances,
      sw_traced_trace_triangles,
      sw_traced_trace_vertices,
    };

    auto sw_traced_gbuffer_0_texture = scope.gbuffer_0_target->CreateView("spark_sw_traced_gbuffer_0_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_gbuffer_1_texture = scope.gbuffer_1_target->CreateView("spark_sw_traced_gbuffer_1_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto sw_traced_depth_texture = scope.depth_target->CreateView("spark_sw_traced_depth_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      sw_traced_gbuffer_0_texture,
      sw_traced_gbuffer_1_texture,
      sw_traced_depth_texture,
    };

    sw_traced_batch = sw_traced_config->CreateBatch("spark_sw_traced_batch",
      { entities, uint32_t(std::size(entities)) },
      {},
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      { rb_views, uint32_t(std::size(rb_views)) },
      {}
    );
  }

  void SWTracedShadow::DestroySWTracedBatch()
  {
    sw_traced_config->DestroyBatch(sw_traced_batch);
    sw_traced_batch.reset();
  }

  void SWTracedShadow::DestroySWTracedConfig()
  {
    sw_traced_pass->DestroyConfig(sw_traced_config);
    sw_traced_config.reset();
  }

  void SWTracedShadow::DestroySWTracedPass()
  {
    scope.core->GetDevice()->DestroyPass(sw_traced_pass);
    sw_traced_pass.reset();
  }

  void SWTracedShadow::Enable()
  {
    geometry_pass->SetEnabled(true);
    sw_traced_pass->SetEnabled(true);
    present_pass->SetEnabled(true);
  }

  void SWTracedShadow::Disable()
  {
    geometry_pass->SetEnabled(false);
    sw_traced_pass->SetEnabled(false);
    present_pass->SetEnabled(false);
  }

  SWTracedShadow::SWTracedShadow(const Render3DScope& scope)
    : Render3DMode(scope)
  {
    CreateGeometryPass();
    CreateGeometryConfig();
    CreateGeometryBatch();

    CreateSkyboxConfig();
    CreateSkyboxBatch();

    CreateSWTracedPass();
    CreateSWTracedConfig();
    CreateSWTracedBatch();

    CreatePresentPass();
    CreatePresentConfig();
    CreatePresentBatch();
  }

  SWTracedShadow::~SWTracedShadow()
  {
    DestroyPresentBatch();
    DestroyPresentConfig();
    DestroyPresentPass();

    DestroySWTracedBatch();
    DestroySWTracedConfig();
    DestroySWTracedPass();

    DestroySkyboxBatch();
    DestroySkyboxConfig();

    DestroyGeometryBatch();
    DestroyGeometryConfig();
    DestroyGeometryPass();
  }
}