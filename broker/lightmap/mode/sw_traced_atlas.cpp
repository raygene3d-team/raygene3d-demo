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


#include "sw_traced_atlas.h"

namespace RayGene3D
{
  namespace Lightmap
  {
    void SWTracedAtlas::CreateSWTracedPass()
    {
      const auto size_x = scope.prop_atlas_size_x->GetUint();
      const auto size_y = scope.prop_atlas_size_x->GetUint();
      const auto layers = scope.prop_atlas_layers->GetUint();

      sw_traced_pass = scope.core->GetDevice()->CreatePass("lightmap_sw_traced_pass",
        Pass::TYPE_COMPUTE,
        size_x,
        size_y,
        layers,
        {},
        {}
      );
    }


    void SWTracedAtlas::CreateSWTracedConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/lightmap_sw_traced.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();

      std::vector<std::pair<std::string, std::string>> defines;

      sw_traced_config = sw_traced_pass->CreateConfig("lightmap_sw_traced_config",
        shader_ss.str(),
        Config::Compilation(Config::COMPILATION_CS),
        { defines.data(), uint32_t(defines.size()) },
        {},
        {},
        {},
        {}
      );
    }

    void SWTracedAtlas::CreateSWTracedBatch()
    {
      const auto size_x = scope.prop_atlas_size_x->GetUint();
      const auto size_y = scope.prop_atlas_size_x->GetUint();
      const auto layers = scope.prop_atlas_layers->GetUint();

      const Batch::Entity entities[] = {
        {{}, {}, nullptr, { 0u, size_x }, { 0u, size_y }, { 0u, layers }}
      };

      const Batch::Sampler samplers[] = {
       { Batch::Sampler::FILTERING_NEAREST, 1, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      };

      auto sw_traced_screen_data = scope.screen_data->CreateView("lightmap_sw_traced_screen_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const std::shared_ptr<View> ub_views[] = {
        sw_traced_screen_data,
      };

      auto sw_traced_trace_t_boxes = scope.trace_t_boxes->CreateView("lightmap_sw_traced_trace_t_boxes",
        Usage(USAGE_SHADER_RESOURCE)
      );
      auto sw_traced_trace_b_boxes = scope.trace_b_boxes->CreateView("lightmap_sw_traced_trace_b_boxes",
        Usage(USAGE_SHADER_RESOURCE)
      );
      auto sw_traced_trace_instances = scope.trace_instances->CreateView("lightmap_sw_traced_trace_instances",
        Usage(USAGE_SHADER_RESOURCE)
      );
      auto sw_traced_trace_triangles = scope.trace_triangles->CreateView("lightmap_sw_traced_trace_triangles",
        Usage(USAGE_SHADER_RESOURCE)
      );
      auto sw_traced_trace_vertices = scope.trace_vertices->CreateView("lightmap_sw_traced_trace_vertices",
        Usage(USAGE_SHADER_RESOURCE)
      );

      const std::shared_ptr<View> rb_views[] = {
        sw_traced_trace_t_boxes,
        sw_traced_trace_b_boxes,
        sw_traced_trace_instances,
        sw_traced_trace_triangles,
        sw_traced_trace_vertices,
      };

      auto sw_traced_input = scope.lightmaps_input->CreateView("lightmap_sw_traced_input",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> ri_views[] = {
        sw_traced_input,
      };

      auto sw_traced_accum = scope.lightmaps_accum->CreateView("lightmap_sw_traced_accum",
        Usage(USAGE_UNORDERED_ACCESS)
      );
      const std::shared_ptr<View> wi_views[] = {
        sw_traced_accum,
      };

      sw_traced_batch = sw_traced_config->CreateBatch("spark_sw_traced_batch",
        { entities, uint32_t(std::size(entities)) },
        { samplers, uint32_t(std::size(samplers)) },
        { ub_views, uint32_t(std::size(ub_views)) },
        {},
        { ri_views, uint32_t(std::size(ri_views)) },
        { wi_views, uint32_t(std::size(wi_views)) },
        { rb_views, uint32_t(std::size(rb_views)) },
        {}
      );
    }

    void SWTracedAtlas::DestroySWTracedBatch()
    {
      sw_traced_config->DestroyBatch(sw_traced_batch);
      sw_traced_batch.reset();
    }

    void SWTracedAtlas::DestroySWTracedConfig()
    {
      sw_traced_pass->DestroyConfig(sw_traced_config);
      sw_traced_config.reset();
    }

    void SWTracedAtlas::DestroySWTracedPass()
    {
      scope.core->GetDevice()->DestroyPass(sw_traced_pass);
      sw_traced_pass.reset();
    }

    void SWTracedAtlas::Enable()
    {
      sw_traced_pass->SetEnabled(true);
    }

    void SWTracedAtlas::Disable()
    {
      sw_traced_pass->SetEnabled(false);
    }

    SWTracedAtlas::SWTracedAtlas(const Scope& scope)
      : Mode(scope)
    {
      CreateSWTracedPass();
      CreateSWTracedConfig();
      CreateSWTracedBatch();
    }

    SWTracedAtlas::~SWTracedAtlas()
    {
      DestroySWTracedBatch();
      DestroySWTracedConfig();
      DestroySWTracedPass();
    }
  }
}