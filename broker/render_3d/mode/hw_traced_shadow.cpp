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
  namespace Render3D
  {
    void HWTracedShadow::CreateHWTracedPass()
    {
      const auto size_x = scope.prop_extent_x->GetUint();
      const auto size_y = scope.prop_extent_y->GetUint();
      const auto layers = 1u;

      hw_traced_pass = scope.core->GetDevice()->CreatePass("spark_hw_traced_pass",
        Pass::TYPE_TRACING,
        size_x,
        size_y,
        layers,
        {},
        {}
      );
    }


    void HWTracedShadow::CreateHWTracedConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_hw_traced.glsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();

      std::vector<std::pair<std::string, std::string>> defines;
      //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

      hw_traced_config = hw_traced_pass->CreateConfig("spark_hw_traced_config",
        shader_ss.str(),
        Config::Compilation(Config::COMPILATION_RGEN | Config::COMPILATION_MISS),
        { defines.data(), defines.size() },
        {},
        {},
        {},
        {}
      );
    }

    void HWTracedShadow::CreateHWTracedBatch()
    {
      const auto [items, count] = scope.prop_inst->GetObjectItem("binary")->GetRawItems<Instance>(0);
      auto entities = std::vector<Batch::Entity>(count);
      for (auto i = 0u; i < count; ++i)
      {
        const auto& hw_traced_trace_vertices = scope.trace_buffer_vert->CreateView("spark_hw_traced_trace_vertices_" + std::to_string(i),
          Usage(USAGE_SHADER_RESOURCE)
        );
        const std::shared_ptr<View> va_views[] = {
          hw_traced_trace_vertices,
        };

        const auto& hw_traced_trace_triangles = scope.trace_buffer_trng->CreateView("spark_hw_traced_trace_triangles_" + std::to_string(i),
          Usage(USAGE_SHADER_RESOURCE)
        );
        const std::shared_ptr<View> ia_views[] = {
          hw_traced_trace_triangles,
        };

        const auto& ins_range = Range{ 1u,  0u };
        const auto& vtx_range = Range{ items[i].vert_offset * 1, items[i].vert_count * 1 };
        const auto& idx_range = Range{ items[i].trng_offset * 3, items[i].trng_count * 3 };
        const auto& sb_offset = std::nullopt;
        const auto& push_data = std::nullopt;

        entities[i] = {
          { va_views, va_views + std::size(va_views) },
          { ia_views, ia_views + std::size(ia_views) },
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

      hw_traced_batch = hw_traced_config->CreateBatch("spark_hw_traced_batch",
        { entities.data(), entities.size() },
        { samplers, std::size(samplers) },
        { ub_views, std::size(ub_views) },
        {},
        { ri_views, std::size(ri_views) },
        { wi_views, std::size(wi_views) },
        {},
        {}
      );
    }

    void HWTracedShadow::DestroyHWTracedBatch()
    {
      hw_traced_config->DestroyBatch(hw_traced_batch);
      hw_traced_batch.reset();
    }

    void HWTracedShadow::DestroyHWTracedConfig()
    {
      hw_traced_pass->DestroyConfig(hw_traced_config);
      hw_traced_config.reset();
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

    HWTracedShadow::HWTracedShadow(Scope& scope)
      : Mode(scope)
    {
      CreateGeometryPass();
      CreateGeometryConfig();
      CreateGeometryBatch();

      CreateSkyboxConfig();
      CreateSkyboxBatch();

      CreateHWTracedPass();
      CreateHWTracedConfig();
      CreateHWTracedBatch();

      CreatePresentPass();
      CreatePresentConfig();
      CreatePresentBatch();
    }

    HWTracedShadow::~HWTracedShadow()
    {
      DestroyPresentBatch();
      DestroyPresentConfig();
      DestroyPresentPass();

      DestroyHWTracedBatch();
      DestroyHWTracedConfig();
      DestroyHWTracedPass();

      DestroySkyboxBatch();
      DestroySkyboxConfig();

      DestroyGeometryBatch();
      DestroyGeometryConfig();
      DestroyGeometryPass();
    }
  }
}