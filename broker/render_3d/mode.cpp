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
  namespace Render3D
  {
    void Mode::CreateGeometryPass()
    {
      const auto size_x = scope.prop_extent_x->GetUint();
      const auto size_y = scope.prop_extent_y->GetUint();
      const auto layers = 1u;

      const auto& geometry_color_target = scope.color_target->CreateView("render_3d_geometry_color_target",
        Usage(USAGE_RENDER_TARGET)
      );
      const auto& geometry_gbuffer_0_target = scope.gbuffer_0_target->CreateView("render_3d_geometry_gbuffer_0_target",
        Usage(USAGE_RENDER_TARGET)
      );
      const auto& geometry_gbuffer_1_target = scope.gbuffer_1_target->CreateView("render_3d_geometry_gbuffer_1_target",
        Usage(USAGE_RENDER_TARGET)
      );

      const Pass::RTAttachment rt_attachments[] = {
        { geometry_color_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
        { geometry_gbuffer_0_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
        { geometry_gbuffer_1_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
      };

      const auto& geometry_depth_target = scope.depth_target->CreateView("render_3d_geometry_depth_target",
        Usage(USAGE_DEPTH_STENCIL)
      );
      const Pass::DSAttachment ds_attachments[] = {
        geometry_depth_target, { 1.0f, std::nullopt },
      };

      geometry_pass = scope.core->GetDevice()->CreatePass("render_3d_geometry_pass",
        Pass::TYPE_GRAPHIC,
        size_x,
        size_y,
        layers,
        { rt_attachments, std::size(rt_attachments) },
        { ds_attachments, std::size(ds_attachments) }
      );
    }

    void Mode::CreateGeometryConfig()
    {
      std::fstream shader_fs;
      //shader_fs.open("./asset/shaders/spark_geom_raster.hlsl", std::fstream::in);
      shader_fs.open("./asset/shaders/spark_geom_meshlet.glsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      shader_fs.close();

      std::vector<std::pair<std::string, std::string>> defines;
      //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

      const Config::IAState ia_config =
      {
        //Config::TOPOLOGY_TRIANGLELIST,
        //Config::INDEXER_32_BIT,
        //{
        //  { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false },
        //  { 0, 12, 64, FORMAT_R8G8B8A8_UNORM, false },
        //  { 0, 16, 64, FORMAT_R32G32B32_FLOAT, false },
        //  { 0, 28, 64, FORMAT_R32_UINT, false },
        //  { 0, 32, 64, FORMAT_R32G32B32_FLOAT, false },
        //  { 0, 44, 64, FORMAT_R32_FLOAT, false },
        //  { 0, 48, 64, FORMAT_R32G32_FLOAT, false },
        //  { 0, 56, 64, FORMAT_R32G32_FLOAT, false },
        //}
      };

      const Config::RCState rc_config =
      {
        Config::FILL_SOLID,
        Config::CULL_BACK,
        {
          { 0.0f, 0.0f, float(scope.prop_extent_x->GetUint()), float(scope.prop_extent_y->GetUint()), 0.0f, 1.0f }
        },
      };

      const Config::DSState ds_config =
      {
        true, //depth_enabled
        true, //depth_write
        Config::COMPARISON_LESS //depth_comparison
      };

      const Config::OMState om_config =
      {
        false,
        {
          { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
          { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
          { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
        }
      };

      geometry_config = geometry_pass->CreateConfig("render_3d_geometry_config",
        shader_ss.str(),
        //Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
        Config::Compilation(Config::COMPILATION_MESH), // | Config::COMPILATION_PS),
        { defines.data(), defines.size() },
        ia_config,
        rc_config,
        ds_config,
        om_config
      );
    }


    void Mode::CreateGeometryBatch()
    {
      const auto [data, count] = scope.prop_inst->GetObjectItem("binary")->GetRawItems<Instance>(0);
      auto entities = std::vector<Batch::Entity>(count);
      for (auto i = 0u; i < count; ++i)
      {
        //const auto& geometry_vertices = scope.scene_buffer_vert->CreateView("render_3d_geometry_vertices_" + std::to_string(i),
        //  Usage(USAGE_VERTEX_ARRAY)
        //);
        //const std::shared_ptr<View> va_views[] = {
        //  geometry_vertices,
        //};

        //const auto& geometry_triangles = scope.scene_buffer_trng->CreateView("render_3d_geometry_triangles_" + std::to_string(i),
        //  Usage(USAGE_INDEX_ARRAY)
        //);
        //const std::shared_ptr<View> ia_views[] = {
        //  geometry_triangles,
        //};

        //const auto& geometry_graphic_arguments = scope.graphic_arguments->CreateView("render_3d_geometry_graphic_argument_" + std::to_string(i),
        //  Usage(USAGE_ARGUMENT_LIST),
        //  { sizeof(Batch::Graphic) * i, sizeof(Batch::Graphic) }
        //);

        //const auto& ins_range = Range{ 0u,  1u };
        //const auto& vtx_range = Range{ data[i].vert_offset * 1, data[i].vert_count * 1 };
        //const auto& idx_range = Range{ data[i].trng_offset * 3, data[i].trng_count * 3 };
        const auto& ins_range = Range{ 0u, data[i].mlet_count };
        const auto& vtx_range = Range{ 0u, 1u };
        const auto& idx_range = Range{ 0u, 1u };
        const auto& sb_offset = std::array<uint32_t, 4>{ uint32_t(sizeof(Instance)) * i, 0u, 0u, 0u };
        const auto& push_data = std::nullopt;

        entities[i] = {
          {}, //{ va_views, va_views + std::size(va_views) },
          {}, //{ ia_views, ia_views + std::size(ia_views) },
          nullptr, // geometry_graphic_arguments,
          ins_range,
          vtx_range,
          idx_range,
          sb_offset,
          push_data
        };
      }

      const Batch::Sampler samplers[] = {
        { Batch::Sampler::FILTERING_ANISOTROPIC, 16, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
        { Batch::Sampler::FILTERING_NEAREST, 1, Batch::Sampler::ADDRESSING_MIRROR, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f } };

      const auto& geometry_screen_data = scope.screen_data->CreateView("render_3d_geometry_screen_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const auto& geometry_camera_data = scope.camera_data->CreateView("render_3d_geometry_camera_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const auto& geometry_shadow_data = scope.shadow_data->CreateView("render_3d_geometry_shadow_data",
        Usage(USAGE_CONSTANT_DATA),
        { 0, sizeof(Frustum) }
      );
      const std::shared_ptr<View> ub_views[] = {
        geometry_screen_data,
        geometry_camera_data,
        geometry_shadow_data,
      };

      const auto& geometry_scene_instances = scope.scene_buffer_inst->CreateView("render_3d_geometry_scene_instances",
        Usage(USAGE_CONSTANT_DATA),
        { 0, sizeof(Instance) }
      );
      const std::shared_ptr<View> sb_views[] = {
        geometry_scene_instances
      };

      const auto& geometry_scene_array_aaam = scope.scene_array_aaam->CreateView("render_3d_geometry_scene_array_aaam",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& geometry_scene_array_snno = scope.scene_array_snno->CreateView("render_3d_geometry_scene_array_snno",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& geometry_scene_array_eeet = scope.scene_array_eeet->CreateView("render_3d_geometry_scene_array_eeet",
        Usage(USAGE_SHADER_RESOURCE)
      );
      //const auto& geometry_scene_textures3 = scope.scene_textures3->CreateView("render_3d_geometry_scene_textures3",
      //  Usage(USAGE_SHADER_RESOURCE)
      //);
      //const auto& geometry_scene_textures4 = scope.scene_textures4->CreateView("render_3d_geometry_scene_textures4",
      //  Usage(USAGE_SHADER_RESOURCE)
      //);
      //const auto& geometry_scene_textures5 = scope.scene_textures5->CreateView("render_3d_geometry_scene_textures5",
      //  Usage(USAGE_SHADER_RESOURCE)
      //);
      //const auto& geometry_scene_textures6 = scope.scene_textures6->CreateView("render_3d_geometry_scene_textures6",
      //  Usage(USAGE_SHADER_RESOURCE)
      //);
      //const auto& geometry_scene_textures7 = scope.scene_textures7->CreateView("render_3d_geometry_scene_textures7",
      //  Usage(USAGE_SHADER_RESOURCE)
      //);
      //const auto& geometry_scene_lightmaps = scope.lightmaps_final->CreateView("render_3d_geometry_scene_lightmaps",
      //  Usage(USAGE_SHADER_RESOURCE)
      //);
      const auto& geometry_reflection_map = scope.reflection_map->CreateView("render_3d_geometry_reflection_map",
        Usage(USAGE_SHADER_RESOURCE),
        { 0u, size_t(-1) },
        { 0u, size_t(-1) },
        View::BIND_CUBEMAP_LAYER
      );
      const std::shared_ptr<View> ri_views[] = {
        geometry_scene_array_aaam,
        geometry_scene_array_snno,
        geometry_scene_array_eeet,
        //geometry_scene_array_eeet,
        //geometry_scene_lightmaps,
        //geometry_reflection_map,
      };

      const auto& geometry_trace_meshlets = scope.trace_buffer_mlet->CreateView("render_3d_geometry_trace_meshlets",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& geometry_trace_v_indices = scope.trace_buffer_vidx->CreateView("render_3d_geometry_trace_v_indices",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const auto& geometry_trace_vertices = scope.trace_buffer_vert->CreateView("render_3d_geometry_trace_vertices",
        Usage(USAGE_SHADER_RESOURCE)
      );      
      const auto& geometry_trace_t_indices = scope.trace_buffer_tidx->CreateView("render_3d_geometry_trace_t_indices",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> rb_views[] = {
        geometry_trace_meshlets,
        geometry_trace_v_indices,
        geometry_trace_vertices,
        geometry_trace_t_indices,
      };

      geometry_batch = geometry_config->CreateBatch("render_3d_geometry_batch",
        { entities.data(), entities.size() },
        { samplers, std::size(samplers) },
        { ub_views, std::size(ub_views) },
        { sb_views, std::size(sb_views) },
        { ri_views, std::size(ri_views) },
        {},
        { rb_views, std::size(rb_views) },
        {}
      );
    }


    void Mode::CreateSkyboxConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_environment.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      shader_fs.close();

      std::pair<std::string, std::string> defines[] =
      {
        { "TEST", "1" },
      };

      const Config::IAState ia_config =
      {
        Config::TOPOLOGY_TRIANGLELIST,
        Config::INDEXER_32_BIT,
        {
          { 0, 0, 8, FORMAT_R32G32_FLOAT, false },
        }
      };

      const Config::RCState rc_config =
      {
        Config::FILL_SOLID,
        Config::CULL_BACK,
        {
          { 0.0f, 0.0f, float(scope.prop_extent_x->GetUint()), float(scope.prop_extent_y->GetUint()), 0.0f, 1.0f }
        },
      };

      const Config::DSState ds_config =
      {
        true, //depth_enabled
        false, //depth_write
        Config::COMPARISON_EQUAL //depth_comparison
      };

      const Config::OMState om_config =
      {
        false,
        {
          { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
          { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
          { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
        }
      };

      skybox_config = geometry_pass->CreateConfig("render_3d_skybox_config",
        shader_ss.str(),
        Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
        { defines, std::size(defines) },
        ia_config,
        rc_config,
        ds_config,
        om_config
      );
    }


    void Mode::CreateSkyboxBatch()
    {
      const auto& skybox_screen_quad_vertices = scope.screen_quad_vertices->CreateView("render_3d_skybox_screen_quad_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      const auto& skybox_screen_quad_triangles = scope.screen_quad_triangles->CreateView("render_3d_skybox_screen_quad_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      const Batch::Entity entities[] = {
        {{skybox_screen_quad_vertices}, {skybox_screen_quad_triangles}, nullptr, { 0u, 1u }, { 0u, 4u }, { 0u, 6u }}
      };

      const auto& skybox_screen_data = scope.screen_data->CreateView("render_3d_skybox_screen_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const auto& skybox_camera_data = scope.camera_data->CreateView("render_3d_skybox_camera_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const std::shared_ptr<View> ub_views[] = {
        skybox_screen_data,
        skybox_camera_data,
      };

      const auto& skybox_skybox_cubemap = scope.skybox_cubemap->CreateView("render_3d_skybox_skybox_cubemap",
        Usage(USAGE_SHADER_RESOURCE),
        { 0u, 1u },
        { 0u, 6u },
        View::BIND_CUBEMAP_LAYER
      );
      const std::shared_ptr<View> ri_views[] = {
        skybox_skybox_cubemap,
      };

      const Batch::Sampler samplers[] = {
        { Batch::Sampler::FILTERING_NEAREST, 1, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      };

      skybox_batch = skybox_config->CreateBatch("render_3d_skybox_batch",
        { entities, std::size(entities) },
        { samplers, std::size(samplers) },
        { ub_views, std::size(ub_views) },
        {},
        { ri_views, std::size(ri_views) },
        {},
        {},
        {}
      );
    }


    void Mode::CreatePresentPass()
    {
      const auto size_x = scope.prop_extent_x->GetUint() / 8u;
      const auto size_y = scope.prop_extent_y->GetUint() / 8u;
      const auto layers = 1u;

      present_pass = scope.core->GetDevice()->CreatePass("render_3d_present_pass",
        Pass::TYPE_COMPUTE,
        size_x,
        size_y,
        layers,
        {},
        {}
      );
    }

    void Mode::CreatePresentConfig()
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_present.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      shader_fs.close();

      present_config = present_pass->CreateConfig("render_3d_present_config",
        shader_ss.str(),
        Config::COMPILATION_CS,
        {},
        {},
        {},
        {},
        {}
      );
    }

    void Mode::CreatePresentBatch()
    {
      const auto& present_compute_arguments = scope.compute_arguments->CreateView("render_3d_present_compute_arguments",
        Usage(USAGE_ARGUMENT_LIST)
      );
      const Batch::Entity entities[] = {
        {{}, {}, present_compute_arguments}
      };

      const auto& present_camera_data = scope.camera_data->CreateView("render_3d_present_camera_data",
        Usage(USAGE_CONSTANT_DATA)
      );
      const std::shared_ptr<View> ub_views[] = {
        present_camera_data,
      };

      const auto& present_color_target = scope.color_target->CreateView("render_3d_present_color_target",
        Usage(USAGE_SHADER_RESOURCE)
      );
      const std::shared_ptr<View> ri_views[] = {
        present_color_target,
      };

      const std::shared_ptr<View> wi_views[] = {
        scope.backbuffer_uav,
      };

      present_batch = present_config->CreateBatch("render_3d_present_batch",
        { entities, std::size(entities) },
        {},
        { ub_views, std::size(ub_views) },
        {},
        { ri_views, std::size(ri_views) },
        { wi_views, std::size(wi_views) },
        {},
        {}
      );
    }

    void Mode::DestroyGeometryPass()
    {
      scope.core->GetDevice()->DestroyPass(geometry_pass);
      geometry_pass.reset();
    }

    void Mode::DestroyGeometryBatch()
    {
      geometry_config->DestroyBatch(geometry_batch);
      geometry_batch.reset();
    }

    void Mode::DestroyGeometryConfig()
    {
      geometry_pass->DestroyConfig(geometry_config);
      geometry_config.reset();
    }

    void Mode::DestroySkyboxBatch()
    {
      skybox_config->DestroyBatch(skybox_batch);
      skybox_batch.reset();
    }

    void Mode::DestroySkyboxConfig()
    {
      geometry_pass->DestroyConfig(skybox_config);
      skybox_config.reset();
    }

    void Mode::DestroyPresentBatch()
    {
      present_config->DestroyBatch(present_batch);
      present_batch.reset();
    }

    void Mode::DestroyPresentConfig()
    {
      present_pass->DestroyConfig(present_config);
      present_config.reset();
    }

    void Mode::DestroyPresentPass()
    {
      scope.core->GetDevice()->DestroyPass(present_pass);
      present_pass.reset();
    }

    Mode::Mode(Scope& scope)
      : scope(scope)
    {
    }

    Mode::~Mode()
    {
    }
  }
}