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


#include "mode.h"

namespace RayGene3D
{
  namespace Lightmap
  {
    void Mode::CreateAveragePass()
    {
      const auto size_x = scope.prop_atlas_size_x->GetUint();
      const auto size_y = scope.prop_atlas_size_x->GetUint();
      const auto layers = scope.prop_atlas_layers->GetUint();

      average_pass = scope.core->GetDevice()->CreatePass("lightmap_average_pass",
        Pass::TYPE_COMPUTE,
        size_x / 8,
        size_y / 8,
        layers,
        {},
        {}
      );
    }

    void Mode::CreateAverageConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/lightmap_average.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      shader_fs.close();

      average_config = average_pass->CreateConfig("lightmap_average_config",
        shader_ss.str(),
        Config::COMPILATION_COMP,
        {},
        {},
        {},
        {},
        {}
      );
    }

    void Mode::CreateAverageBatch()
    {
      const auto size_x = scope.prop_atlas_size_x->GetUint();
      const auto size_y = scope.prop_atlas_size_x->GetUint();
      const auto layers = scope.prop_atlas_layers->GetUint();

      const Batch::Entity entities[] = {
        {{}, {}, nullptr, { 0u, size_x / 8 }, { 0u, size_y / 8 }, { 0u, layers }}
      };

      auto average_accum = scope.lightmaps_accum->CreateView("lightmap_average_accum",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> ri_views[] = {
        average_accum,
      };

      auto average_final = scope.lightmaps_final->CreateView("lightmap_average_final",
        Usage(USAGE_UNORDERED_ACCESS)
      );
      const std::shared_ptr<View> wi_views[] = {
        average_final,
      };

      average_batch = average_config->CreateBatch("spark_present_batch",
        { entities, std::size(entities) },
        {},
        {},
        {},
        { ri_views, std::size(ri_views) },
        { wi_views, std::size(wi_views) },
        {},
        {}
      );
    }

    void Mode::DestroyAverageBatch()
    {
      average_config->DestroyBatch(average_batch);
      average_batch.reset();
    }

    void Mode::DestroyAverageConfig()
    {
      average_pass->DestroyConfig(average_config);
      average_config.reset();
    }

    void Mode::DestroyAveragePass()
    {
      scope.core->GetDevice()->DestroyPass(average_pass);
      average_pass.reset();
    }

    Mode::Mode(const Scope& scope)
      : scope(scope)
    {
    }

    Mode::~Mode()
    {
    }
  }
}