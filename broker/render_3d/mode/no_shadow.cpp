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


#include "no_shadow.h"

namespace RayGene3D
{
  namespace Render3D
  {
    void NoShadow::CreateUnshadowedPass()
    {
      const auto size_x = scope.prop_extent_x->GetUint();
      const auto size_y = scope.prop_extent_y->GetUint();
      const auto layers = 1u;

      auto unshadowed_color_target = scope.color_target->CreateView("spark_unshadowed_color_target",
        Usage(USAGE_RENDER_TARGET)
      );
      const Pass::RTAttachment rt_attachments[] = {
        unshadowed_color_target, std::nullopt,
      };

      unshadowed_pass = scope.core->GetDevice()->CreatePass("spark_unshadowed_pass",
        Pass::TYPE_GRAPHIC,
        size_x,
        size_y,
        layers,
        { rt_attachments, std::size(rt_attachments) },
        {}
      );
    }

    void NoShadow::CreateUnshadowedConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_unshadowed.hlsl", std::fstream::in);
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

      unshadowed_config = unshadowed_pass->CreateConfig("spark_unshadowed_config",
        shader_ss.str(),
        Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
        { defines.data(), defines.size() },
        ia_Config,
        rc_Config,
        ds_Config,
        om_Config
      );
    }



    void NoShadow::CreateUnshadowedBatch()
    {
      auto unshadowed_screen_quad_vertices = scope.screen_quad_vertices->CreateView("spark_unshadowed_screen_quad_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      auto unshadowed_screen_quad_triangles = scope.screen_quad_triangles->CreateView("spark_unshadowed_screen_quad_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      const Batch::Entity entities[] = {
        {{unshadowed_screen_quad_vertices}, {unshadowed_screen_quad_triangles}, nullptr, { 0u, 1u }, { 0u, 4u }, { 0u, 6u } }
      };

      auto unshadowed_screen_data = scope.screen_data->CreateView("spark_unshadowed_screen_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      auto unshadowed_camera_data = scope.camera_data->CreateView("spark_unshadowed_camera_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      auto unshadowed_shadow_data = scope.shadow_data->CreateView("spark_unshadowed_shadow_data",
        Usage(USAGE_CONSTANT_DATA),
        { 0, sizeof(Frustum) }
      );
      const std::shared_ptr<View> ub_views[] = {
        unshadowed_screen_data,
        unshadowed_camera_data,
        unshadowed_shadow_data
      };

      auto unshadowed_gbuffer_0_texture = scope.gbuffer_0_target->CreateView("spark_unshadowed_gbuffer_0_target",
        Usage(USAGE_SHADER_RESOURCE)
      );
      auto unshadowed_gbuffer_1_texture = scope.gbuffer_1_target->CreateView("spark_unshadowed_gbuffer_1_target",
        Usage(USAGE_SHADER_RESOURCE)
      );
      auto unshadowed_depth_texture = scope.depth_target->CreateView("spark_unshadowed_depth_target",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> ri_views[] = {
        unshadowed_gbuffer_0_texture,
        unshadowed_gbuffer_1_texture,
        unshadowed_depth_texture,
      };

      unshadowed_batch = unshadowed_config->CreateBatch("spark_unshadowed_batch",
        { entities, std::size(entities) },
        {},
        { ub_views, std::size(ub_views) },
        {},
        { ri_views, std::size(ri_views) }
      );
    }

    void NoShadow::DestroyUnshadowedBatch()
    {
      unshadowed_config->DestroyBatch(unshadowed_batch);
      unshadowed_batch.reset();
    }

    void NoShadow::DestroyUnshadowedConfig()
    {
      unshadowed_pass->DestroyConfig(unshadowed_config);
      unshadowed_config.reset();
    }

    void NoShadow::DestroyUnshadowedPass()
    {
      scope.core->GetDevice()->DestroyPass(unshadowed_pass);
      unshadowed_pass.reset();
    }

    void NoShadow::Enable()
    {
      geometry_pass->SetEnabled(true);
      unshadowed_pass->SetEnabled(true);
      present_pass->SetEnabled(true);
    }

    void NoShadow::Disable()
    {
      geometry_pass->SetEnabled(false);
      unshadowed_pass->SetEnabled(false);
      present_pass->SetEnabled(false);
    }

    NoShadow::NoShadow(Scope& scope)
      : Mode(scope)
    {
      CreateGeometryPass();
      CreateGeometryConfig();
      CreateGeometryBatch();

      CreateSkyboxConfig();
      CreateSkyboxBatch();

      CreateUnshadowedPass();
      CreateUnshadowedConfig();
      CreateUnshadowedBatch();

      CreatePresentPass();
      CreatePresentConfig();
      CreatePresentBatch();
    }

    NoShadow::~NoShadow()
    {
      DestroyPresentBatch();
      DestroyPresentConfig();
      DestroyPresentPass();

      DestroyUnshadowedBatch();
      DestroyUnshadowedConfig();
      DestroyUnshadowedPass();

      DestroySkyboxBatch();
      DestroySkyboxConfig();

      DestroyGeometryBatch();
      DestroyGeometryConfig();
      DestroyGeometryPass();
    }
  }
}