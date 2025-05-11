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


#include "hw_traced_atlas.h"

namespace RayGene3D
{
  namespace Lightmap
  {
    void HWTracedAtlas::CreateHWTracedPass()
    {
      const auto size_x = scope.prop_atlas_size_x->GetUint();
      const auto size_y = scope.prop_atlas_size_x->GetUint();
      const auto layers = scope.prop_atlas_layers->GetUint();

      hw_traced_pass = scope.core->GetDevice()->CreatePass("lightmap_hw_traced_pass",
        Pass::TYPE_TRACING,
        size_x,
        size_y,
        layers,
        {},
        {}
      );
    }


    void HWTracedAtlas::CreateHWTracedConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/lightmap_hw_traced.glsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();

      std::vector<std::pair<std::string, std::string>> defines;
      //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

      hw_traced_config = hw_traced_pass->CreateConfig("lightmap_hw_traced_config",
        shader_ss.str(),
        Config::Compilation(Config::COMPILATION_RGEN | Config::COMPILATION_MISS),
        { defines.data(), defines.size() },
        {},
        {},
        {},
        {}
      );
    }

    void HWTracedAtlas::CreateHWTracedBatch()
    {
      //const auto [data, count] = scope.prop_instances->GetTypedBytes<Instance>(0);
      //auto entities = std::vector<Batch::Entity>(count);
      //for (auto i = 0u; i < count; ++i)
      //{
      //  auto hw_traced_trace_vertices = scope.trace_vertices->CreateView("lightmap_hw_traced_trace_vertices_" + std::to_string(i),
      //    Usage(USAGE_SHADER_RESOURCE) //USAGE_RAYTRACING_INPUT
      //  );
      //  const std::shared_ptr<View> va_views[] = {
      //    hw_traced_trace_vertices,
      //  };

      //  auto hw_traced_trace_triangles = scope.trace_triangles->CreateView("lightmap_hw_traced_trace_triangles_" + std::to_string(i),
      //    Usage(USAGE_SHADER_RESOURCE) //USAGE_RAYTRACING_INPUT
      //  );
      //  const std::shared_ptr<View> ia_views[] = {
      //    hw_traced_trace_triangles,
      //  };

      //  const auto& ins_range = View::Range{ 1u,  0u };
      //  const auto& vtx_range = View::Range{ data[i].vert_offset * 1, data[i].vert_count * 1 };
      //  const auto& idx_range = View::Range{ data[i].prim_offset * 3, data[i].prim_count * 3 };
      //  const auto& sb_offset = std::nullopt;
      //  const auto& push_data = std::nullopt;

      //  entities[i] = {
      //    { va_views, va_views + uint32_t(std::size(va_views)) },
      //    { ia_views, ia_views + uint32_t(std::size(ia_views)) },
      //    nullptr,
      //    ins_range,
      //    vtx_range,
      //    idx_range,
      //    sb_offset,
      //    push_data
      //  };
      //}

      //const Batch::Sampler samplers[] = {
      //  { Batch::Sampler::FILTERING_NEAREST, 1, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      //};

      //auto hw_traced_screen_data = scope.screen_data->CreateView("lightmap_hw_traced_screen_data",
      //  Usage(USAGE_CONSTANT_DATA)
      //);
      //const std::shared_ptr<View> ub_views[] = {
      //  hw_traced_screen_data,
      //};

      //auto hw_traced_input = scope.lightmaps_input->CreateView("lightmap_hw_traced_input",
      //  Usage(USAGE_SHADER_RESOURCE)
      //);
      //const std::shared_ptr<View> ri_views[] = {
      //  hw_traced_input,
      //};

      //auto hw_traced_accum = scope.lightmaps_accum->CreateView("lightmap_hw_traced_accum",
      //  Usage(USAGE_UNORDERED_ACCESS)
      //);
      //const std::shared_ptr<View> wi_views[] = {
      //  hw_traced_accum,
      //};

      //hw_traced_batch = hw_traced_config->CreateBatch("lightmap_hw_traced_batch",
      //  { entities.data(), uint32_t(entities.size()) },
      //  { samplers, uint32_t(std::size(samplers)) },
      //  { ub_views, uint32_t(std::size(ub_views)) },
      //  {},
      //  { ri_views, uint32_t(std::size(ri_views)) },
      //  { wi_views, uint32_t(std::size(wi_views)) },
      //  {},
      //  {}
      //);
    }

    void HWTracedAtlas::DestroyHWTracedBatch()
    {
      hw_traced_config->DestroyBatch(hw_traced_batch);
      hw_traced_batch.reset();
    }

    void HWTracedAtlas::DestroyHWTracedConfig()
    {
      hw_traced_pass->DestroyConfig(hw_traced_config);
      hw_traced_config.reset();
    }

    void HWTracedAtlas::DestroyHWTracedPass()
    {
      scope.core->GetDevice()->DestroyPass(hw_traced_pass);
      hw_traced_pass.reset();
    }

    void HWTracedAtlas::Enable()
    {
      hw_traced_pass->SetEnabled(true);
    }

    void HWTracedAtlas::Disable()
    {
      hw_traced_pass->SetEnabled(false);
    }

    HWTracedAtlas::HWTracedAtlas(const Scope& scope)
      : Mode(scope)
    {
      CreateHWTracedPass();
      CreateHWTracedConfig();
      CreateHWTracedBatch();
    }

    HWTracedAtlas::~HWTracedAtlas()
    {
      DestroyHWTracedBatch();
      DestroyHWTracedConfig();
      DestroyHWTracedPass();
    }
  }
}