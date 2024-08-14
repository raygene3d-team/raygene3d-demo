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


#include "render_3d_mode.h"

namespace RayGene3D
{
  void Render3DMode::CreateGeometryPass()
  {
    const auto size_x = scope.prop_extent_x->GetUint();
    const auto size_y = scope.prop_extent_y->GetUint();
    const auto layers = 1u;

    auto geometry_color_target = scope.color_target->CreateView("spark_geometry_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    auto geometry_gbuffer_0_target = scope.gbuffer_0_target->CreateView("spark_geometry_gbuffer_0_target",
      Usage(USAGE_RENDER_TARGET)
    );
    auto geometry_gbuffer_1_target = scope.gbuffer_1_target->CreateView("spark_geometry_gbuffer_1_target",
      Usage(USAGE_RENDER_TARGET)
    );

    const Pass::RTAttachment rt_attachments[] = {
      { geometry_color_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
      { geometry_gbuffer_0_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
      { geometry_gbuffer_1_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
    };

    auto geometry_depth_target = scope.depth_target->CreateView("spark_geometry_depth_target",
      Usage(USAGE_DEPTH_STENCIL)
    );
    const Pass::DSAttachment ds_attachments[] = {
      geometry_depth_target, { 1.0f, std::nullopt },
    };

    geometry_pass = scope.core->GetDevice()->CreatePass("spark_geometry_pass",
      Pass::TYPE_GRAPHIC,
      size_x,
      size_y,
      layers,
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Render3DMode::CreateGeometryConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_geometry.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const Config::IAState ia_Config =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 12, 64, FORMAT_R8G8B8A8_UNORM, false },
        { 0, 16, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 28, 64, FORMAT_R32_UINT, false },
        { 0, 32, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 44, 64, FORMAT_R32_FLOAT, false },
        { 0, 48, 64, FORMAT_R32G32_FLOAT, false },
        { 0, 56, 64, FORMAT_R32G32_FLOAT, false },
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
      true, //depth_enabled
      true, //depth_write
      Config::COMPARISON_LESS //depth_comparison
    };

    const Config::OMState om_Config =
    {
      false,
      {
        { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
        { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
        { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
      }
    };

    geometry_Config = geometry_pass->CreateConfig("spark_geometry_Config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_Config,
      rc_Config,
      ds_Config,
      om_Config
    );
  }


  void Render3DMode::CreateGeometryBatch()
  {
    const auto [data, count] = scope.prop_instances->GetTypedBytes<Instance>(0);
    auto entities = std::vector<Batch::Entity>(count);
    for (auto i = 0u; i < count; ++i)
    {
      auto geometry_vertices = scope.scene_vertices->CreateView("spark_geometry_vertices_" + std::to_string(i),
        Usage(USAGE_VERTEX_ARRAY)
      );
      const std::shared_ptr<View> va_views[] = {
        geometry_vertices,
      };

      auto geometry_triangles = scope.scene_triangles->CreateView("spark_geometry_triangles_" + std::to_string(i),
        Usage(USAGE_INDEX_ARRAY)
      );
      const std::shared_ptr<View> ia_views[] = {
        geometry_triangles,
      };

      const auto geometry_graphic_arguments = scope.graphic_arguments->CreateView("spark_geometry_graphic_argument_" + std::to_string(i),
        Usage(USAGE_ARGUMENT_LIST),
        { uint32_t(sizeof(Batch::Graphic)) * i, uint32_t(sizeof(Batch::Graphic)) }
      );

      const auto& ins_range = View::Range{ 0u,  1u };
      const auto& vtx_range = View::Range{ data[i].vert_offset * 1, data[i].vert_count * 1 };
      const auto& idx_range = View::Range{ data[i].prim_offset * 3, data[i].prim_count * 3 };
      const auto& sb_offset = std::array<uint32_t, 4>{ uint32_t(sizeof(Frustum))* i, 0u, 0u, 0u };
      const auto& push_data = std::nullopt;

      entities[i] = {
        { va_views, va_views + uint32_t(std::size(va_views)) },
        { ia_views, ia_views + uint32_t(std::size(ia_views)) },
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
      { Batch::Sampler::FILTERING_LINEAR, 1, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f } };

    auto geometry_screen_data = scope.screen_data->CreateView("spark_geometry_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );

    auto geometry_camera_data = scope.camera_data->CreateView("spark_geometry_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );

    auto geometry_shadow_data = scope.shadow_data->CreateView("spark_geometry_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Frustum)) }
    );

    const std::shared_ptr<View> ub_views[] = {
      geometry_screen_data,
      geometry_camera_data,
      geometry_shadow_data,
    };


    auto geometry_scene_instances = scope.scene_instances->CreateView("spark_geometry_scene_instances",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Instance)) }
    );

    const std::shared_ptr<View> sb_views[] = {
      geometry_scene_instances
    };


    auto geometry_scene_textures0 = scope.scene_textures0->CreateView("spark_geometry_scene_textures0",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_scene_textures1 = scope.scene_textures1->CreateView("spark_geometry_scene_textures1",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_scene_textures2 = scope.scene_textures2->CreateView("spark_geometry_scene_textures2",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_scene_textures3 = scope.scene_textures3->CreateView("spark_geometry_scene_textures3",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_scene_lightmaps = scope.scene_lightmaps->CreateView("spark_geometry_scene_lightmaps",
      Usage(USAGE_SHADER_RESOURCE)
    );

    auto geometry_reflection_map = scope.reflection_map->CreateView("spark_geometry_reflection_map",
      Usage(USAGE_SHADER_RESOURCE),
      { 0u, uint32_t(-1) },
      { 0u, uint32_t(-1) },
      View::BIND_CUBEMAP_LAYER
    );

    const std::shared_ptr<View> ri_views[] = {
      geometry_scene_textures0,
      geometry_scene_textures1,
      geometry_scene_textures2,
      geometry_scene_textures3,
      geometry_scene_lightmaps,
      geometry_reflection_map,
    };

    geometry_batch = geometry_Config->CreateBatch("spark_geometry_batch",
      { entities.data(), uint32_t(entities.size()) },
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      { sb_views, uint32_t(std::size(sb_views)) },
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {}
    );
  }


  void Render3DMode::CreateSkyboxConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_environment.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::pair<std::string, std::string> defines[] =
    {
      { "TEST", "1" },
    };

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
      true, //depth_enabled
      false, //depth_write
      Config::COMPARISON_EQUAL //depth_comparison
    };

    const Config::OMState om_Config =
    {
      false,
      {
        { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
        { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
        { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
      }
    };

    skybox_Config = geometry_pass->CreateConfig("spark_skybox_Config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines, uint32_t(std::size(defines)) },
      ia_Config,
      rc_Config,
      ds_Config,
      om_Config
    );
  }


  void Render3DMode::CreateSkyboxBatch()
  {
    auto skybox_screen_quad_vertices = scope.screen_quad_vertices->CreateView("spark_skybox_screen_quad_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    auto skybox_screen_quad_triangles = scope.screen_quad_triangles->CreateView("spark_skybox_screen_quad_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const Batch::Entity entities[] = {
      {{skybox_screen_quad_vertices}, {skybox_screen_quad_triangles}, nullptr, { 0u, 1u }, { 0u, 4u }, { 0u, 6u }}
    };

    auto skybox_screen_data = scope.screen_data->CreateView("spark_skybox_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    auto skybox_camera_data = scope.camera_data->CreateView("spark_skybox_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    const std::shared_ptr<View> ub_views[] = {
      skybox_screen_data,
      skybox_camera_data,
    };

    auto skybox_skybox_cubemap = scope.skybox_cubemap->CreateView("spark_skybox_skybox_cubemap",
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

    skybox_batch = skybox_Config->CreateBatch("spark_skybox_batch",
      { entities, uint32_t(std::size(entities)) },
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {}
    );
  }


  void Render3DMode::CreatePresentPass()
  {
    const auto size_x = scope.prop_extent_x->GetUint() / 8u;
    const auto size_y = scope.prop_extent_y->GetUint() / 8u;
    const auto layers = 1u;

    present_pass = scope.core->GetDevice()->CreatePass("spark_present_pass",
      Pass::TYPE_COMPUTE,
      size_x,
      size_y,
      layers,
      {},
      {}
    );
  }

  void Render3DMode::CreatePresentConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_present.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    present_Config = present_pass->CreateConfig("spark_present_Config",
      shader_ss.str(),
      Config::COMPILATION_CS,
      {},
      {},
      {},
      {},
      {}
    );
  }

  void Render3DMode::CreatePresentBatch()
  {
    auto present_compute_arguments = scope.compute_arguments->CreateView("spark_present_compute_arguments",
      Usage(USAGE_ARGUMENT_LIST)
    );
    const Batch::Entity entities[] = {
      {{}, {}, present_compute_arguments}
    };

    auto present_camera_data = scope.camera_data->CreateView("spark_present_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    const std::shared_ptr<View> ub_views[] = {
      present_camera_data,
    };

    auto present_color_target = scope.color_target->CreateView("spark_present_color_target",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      present_color_target,
    };

    const std::shared_ptr<View> wi_views[] = {
      scope.backbuffer_uav,
    };

    present_batch = present_Config->CreateBatch("spark_present_batch",
      { entities, uint32_t(std::size(entities)) },
      {},
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      { wi_views, uint32_t(std::size(wi_views)) },
      {},
      {}
    );
  }

  void Render3DMode::DestroyGeometryPass()
  {
    scope.core->GetDevice()->DestroyPass(geometry_pass);
    geometry_pass.reset();
  }

  void Render3DMode::DestroyGeometryBatch()
  {
    geometry_Config->DestroyBatch(geometry_batch);
    geometry_batch.reset();
  }

  void Render3DMode::DestroyGeometryConfig()
  {
    geometry_pass->DestroyConfig(geometry_Config);
    geometry_Config.reset();
  }

  void Render3DMode::DestroySkyboxBatch()
  {
    skybox_Config->DestroyBatch(skybox_batch);
    skybox_batch.reset();
  }

  void Render3DMode::DestroySkyboxConfig()
  {
    geometry_pass->DestroyConfig(skybox_Config);
    skybox_Config.reset();
  }

  void Render3DMode::DestroyPresentBatch()
  {
    present_Config->DestroyBatch(present_batch);
    present_batch.reset();
  }

  void Render3DMode::DestroyPresentConfig()
  {
    present_pass->DestroyConfig(present_Config);
    present_Config.reset();
  }

  void Render3DMode::DestroyPresentPass()
  {
    scope.core->GetDevice()->DestroyPass(present_pass);
    present_pass.reset();
  }

  Render3DMode::Render3DMode(const Render3DScope& scope)
    : scope(scope)
  {
  }
  
  Render3DMode::~Render3DMode()
  {
  }
}