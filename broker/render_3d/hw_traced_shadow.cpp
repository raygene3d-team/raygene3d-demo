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


#include "hw_traced_shadow.h"

namespace RayGene3D
{
  void HWTracedShadow::CreateHWTracedPass()
  {
    const auto size_x = scope.prop_extent_x->GetUint();
    const auto size_y = scope.prop_extent_y->GetUint();
    const auto layers = 1u;

    hw_traced_pass = scope.core->GetDevice()->CreatePass("spark_hw_traced_pass",
      Pass::TYPE_RAYTRACING,
      size_x,
      size_y,
      layers,
      {},
      {}
    );
  }


  void HWTracedShadow::CreateHWTracedTechnique()
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

  void HWTracedShadow::CreateHWTracedBatch()
  {
    const auto [data, count] = scope.prop_instances->GetTypedBytes<Instance>(0);
    auto entities = std::vector<Batch::Entity>(count);
    for (auto i = 0u; i < count; ++i)
    {
      auto hw_traced_trace_vertices = scope.trace_vertices->CreateView("spark_hw_traced_trace_vertices_" + std::to_string(i),
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> va_views[] = {
        hw_traced_trace_vertices,
      };

      auto hw_traced_trace_triangles = scope.trace_triangles->CreateView("spark_hw_traced_trace_triangles_" + std::to_string(i),
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> ia_views[] = {
        hw_traced_trace_triangles,
      };

      const auto& ins_range = View::Range{ 1u,  0u };
      const auto& vtx_range = View::Range{ data[i].vert_offset * 1, data[i].vert_count * 1 };
      const auto& idx_range = View::Range{ data[i].prim_offset * 3, data[i].prim_count * 3 };
      const auto& sb_offset = std::nullopt;
      const auto& push_data = std::nullopt;

      entities[i] = {
        { va_views, va_views + uint32_t(std::size(va_views)) },
        { ia_views, ia_views + uint32_t(std::size(ia_views)) },
        nullptr,
        ins_range,
        vtx_range,
        idx_range,
        sb_offset,
        push_data
      };
    }

    const Batch::Sampler samplers[] = {
      { Batch::Sampler::FILTERING_NEAREST, 1, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };

    auto hw_traced_screen_data = scope.screen_data->CreateView("spark_hw_traced_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto hw_traced_camera_data = scope.camera_data->CreateView("spark_hw_traced_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto hw_traced_shadow_data = scope.shadow_data->CreateView("spark_hw_traced_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, sizeof(Frustum) }
    );
    const std::shared_ptr<View> ub_views[] = {
      hw_traced_screen_data,
      hw_traced_camera_data,
      hw_traced_shadow_data,
    };

    auto hw_traced_gbuffer_0_texture = scope.gbuffer_0_target->CreateView("spark_hw_traced_gbuffer_0_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto hw_traced_gbuffer_1_texture = scope.gbuffer_1_target->CreateView("spark_hw_traced_gbuffer_1_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    auto hw_traced_depth_texture = scope.depth_target->CreateView("spark_hw_traced_depth_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      hw_traced_gbuffer_0_texture,
      hw_traced_gbuffer_1_texture,
      hw_traced_depth_texture,
    };

    auto hw_traced_color_texture = scope.color_target->CreateView("spark_hw_traced_color_texture",
      Usage(USAGE_UNORDERED_ACCESS)
    );
    const std::shared_ptr<View> wi_views[] = {
      hw_traced_color_texture,
    };

    hw_traced_batch = hw_traced_technique->CreateBatch("spark_hw_traced_batch",
      { entities.data(), uint32_t(entities.size()) },
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      { wi_views, uint32_t(std::size(wi_views)) },
      {},
      {}
    );
  }

  void HWTracedShadow::DestroyHWTracedBatch()
  {
    hw_traced_technique->DestroyBatch(hw_traced_batch);
    hw_traced_batch.reset();
  }

  void HWTracedShadow::DestroyHWTracedTechnique()
  {
    hw_traced_pass->DestroyTechnique(hw_traced_technique);
    hw_traced_technique.reset();
  }

  void HWTracedShadow::DestroyHWTracedPass()
  {
    scope.core->GetDevice()->DestroyPass(hw_traced_pass);
    hw_traced_pass.reset();
  }

  void HWTracedShadow::Enable()
  {
    geometry_pass->SetEnabled(true);
    hw_traced_pass->SetEnabled(true);
    present_pass->SetEnabled(true);
  }

  void HWTracedShadow::Disable()
  {
    geometry_pass->SetEnabled(false);
    hw_traced_pass->SetEnabled(false);
    present_pass->SetEnabled(false);
  }

  HWTracedShadow::HWTracedShadow(const Render3DScope& scope)
    : Render3DMode(scope)
  {
    CreateGeometryPass();
    CreateGeometryTechnique();
    CreateGeometryBatch();

    CreateSkyboxTechnique();
    CreateSkyboxBatch();

    CreateHWTracedPass();
    CreateHWTracedTechnique();
    CreateHWTracedBatch();

    CreatePresentPass();
    CreatePresentTechnique();
    CreatePresentBatch();
  }

  HWTracedShadow::~HWTracedShadow()
  {
    DestroyPresentBatch();
    DestroyPresentTechnique();
    DestroyPresentPass();

    DestroyHWTracedBatch();
    DestroyHWTracedTechnique();
    DestroyHWTracedPass();

    DestroySkyboxBatch();
    DestroySkyboxTechnique();

    DestroyGeometryBatch();
    DestroyGeometryTechnique();
    DestroyGeometryPass();
  }
    
}