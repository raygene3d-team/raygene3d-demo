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
        size_x / 8,
        size_y / 8,
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
      shader_fs.close();

      std::vector<std::pair<std::string, std::string>> defines;

      sw_traced_config = sw_traced_pass->CreateConfig("lightmap_sw_traced_config",
        shader_ss.str(),
        Config::Compilation(Config::COMPILATION_CS),
        { defines.data(), defines.size() },
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
        {{}, {}, nullptr, { 0u, size_x / 8 }, { 0u, size_y / 8 }, { 0u, layers }}
      };

      const Batch::Sampler samplers[] = {
       { Batch::Sampler::FILTERING_NEAREST, 1, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      };

      const auto& screen_data = scope.screen_data->CreateView("lightmap_sw_traced_screen_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const std::shared_ptr<View> ub_views[] = {
        screen_data,
      };

      const auto& trace_buffer_tbox = scope.trace_buffer_tbox->CreateView("lightmap_sw_trace_buffer_tbox",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& trace_buffer_bbox = scope.trace_buffer_bbox->CreateView("lightmap_sw_trace_buffer_bbox",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& trace_buffer_inst = scope.trace_buffer_inst->CreateView("lightmap_sw_trace_buffer_inst",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& trace_buffer_trng = scope.trace_buffer_trng->CreateView("lightmap_sw_trace_buffer_trng",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& trace_buffer_vert = scope.trace_buffer_vert->CreateView("lightmap_sw_trace_buffer_vert",
        Usage(USAGE_SHADER_RESOURCE)
      );

      const std::shared_ptr<View> rb_views[] = {
        trace_buffer_tbox,
        trace_buffer_bbox,
        trace_buffer_inst,
        trace_buffer_trng,
        trace_buffer_vert,
      };

      const auto& scene_array_aaam = scope.scene_array_aaam->CreateView("lightmap_sw_scene_array_aaam",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& scene_array_snno = scope.scene_array_snno->CreateView("lightmap_sw_scene_array_snno",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& scene_array_eeet = scope.scene_array_eeet->CreateView("lightmap_sw_scene_array_eeet",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& sw_traced_input = scope.lightmaps_input->CreateView("lightmap_sw_traced_input",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> ri_views[] = {
        scene_array_aaam,
        scene_array_snno,
        scene_array_eeet,
        sw_traced_input,
      };

      const auto& sw_traced_accum = scope.lightmaps_accum->CreateView("lightmap_sw_traced_accum",
        Usage(USAGE_UNORDERED_ACCESS)
      );
      const std::shared_ptr<View> wi_views[] = {
        sw_traced_accum,
      };

      sw_traced_batch = sw_traced_config->CreateBatch("lightmap_sw_traced_batch",
        { entities, std::size(entities) },
        { samplers, std::size(samplers) },
        { ub_views, std::size(ub_views) },
        {},
        { ri_views, std::size(ri_views) },
        { wi_views, std::size(wi_views) },
        { rb_views, std::size(rb_views) },
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
      average_pass->SetEnabled(true);
    }

    void SWTracedAtlas::Disable()
    {
      sw_traced_pass->SetEnabled(false);
      average_pass->SetEnabled(false);
    }

    SWTracedAtlas::SWTracedAtlas(const Scope& scope)
      : Mode(scope)
    {
      CreateSWTracedPass();
      CreateSWTracedConfig();
      CreateSWTracedBatch();

      CreateAveragePass();
      CreateAverageConfig();
      CreateAverageBatch();
    }

    SWTracedAtlas::~SWTracedAtlas()
    {
      DestroyAverageBatch();
      DestroyAverageConfig();
      DestroyAveragePass();

      DestroySWTracedBatch();
      DestroySWTracedConfig();
      DestroySWTracedPass();
    }
  }
}