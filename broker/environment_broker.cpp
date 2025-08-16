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
#include "environment_broker.h"

namespace RayGene3D
{
  const uint32_t EnvironmentBroker::levels;

  void EnvironmentBroker::Initialize()
  {
    const auto& prop_tree = wrap.GetUtil()->GetStorage()->GetTree();
    const auto& prop_environment = prop_tree->GetObjectItem("environment");

    path = prop_environment->GetObjectItem("path")->GetString();
    quality = prop_environment->GetObjectItem("quality")->GetUint();

    const auto mipmap = std::max(quality, levels);
    const auto extent = 1u << int32_t(quality) - 1;

    auto pano_texture = TextureArrayHDR(FORMAT_R32G32B32A32_FLOAT, 2 * extent, extent, 1);
    pano_texture.Create(0, glm::zero<glm::f32vec4>());
    pano_texture.Load(0, path.c_str());

    enum CUBEMAP_FACE
    {
      FACE_POSITIVE_X,
      FACE_NEGATIVE_X,
      FACE_POSITIVE_Y,
      FACE_NEGATIVE_Y,
      FACE_POSITIVE_Z,
      FACE_NEGATIVE_Z
    };

    const auto xyz_from_cube = [](const glm::f32vec2& uv, CUBEMAP_FACE face)
    {
      switch (face)
      {
      case FACE_POSITIVE_X:
        return glm::normalize(glm::f32vec3{ 1.0f,-(2.0f * uv[1] - 1.0f),-(2.0f * uv[0] - 1.0f) });
      case FACE_NEGATIVE_X:
        return glm::normalize(glm::f32vec3{-1.0f,-(2.0f * uv[1] - 1.0f), (2.0f * uv[0] - 1.0f) });
      case FACE_POSITIVE_Y:
        return glm::normalize(glm::f32vec3{ (2.0f * uv[0] - 1.0f), 1.0f, (2.0f * uv[1] - 1.0f) });
      case FACE_NEGATIVE_Y:
        return glm::normalize(glm::f32vec3{ (2.0f * uv[0] - 1.0f),-1.0f,-(2.0f * uv[1] - 1.0f) });
      case FACE_POSITIVE_Z:
        return glm::normalize(glm::f32vec3{ (2.0f * uv[0] - 1.0f),-(2.0f * uv[1] - 1.0f), 1.0f });
      case FACE_NEGATIVE_Z:
        return glm::normalize(glm::f32vec3{-(2.0f * uv[0] - 1.0f),-(2.0f * uv[1] - 1.0f),-1.0f });
      }

      return glm::f32vec3();
    };

    const auto pano_from_xyz = [](const glm::f32vec3& xyz)
    {
      const auto theta = std::atan2(xyz.x, xyz.z);
      const auto phi = std::asin(xyz.y);

      const auto u = 0.5f * (theta + glm::pi<float>()) / glm::pi<float>();
      const auto v = (glm::half_pi<float>() - phi) / glm::pi<float>();

      return glm::f32vec2(u, v);
    };

    auto cube_texture = TextureArrayHDR(FORMAT_R32G32B32A32_FLOAT, extent, extent, 6);   
    for (auto k = 0u; k < 6; ++k)
    {
      cube_texture.Create(k);

      for (auto i = 0u; i < cube_texture.Count(k); ++i)
      {
        const auto cube_texel = glm::i32vec2(i % extent, i / extent);
        const auto cube_uv = glm::f32vec2((cube_texel.x + 0.5f) / extent, (cube_texel.y + 0.5f) / extent);
        const auto pano_uv = pano_from_xyz(xyz_from_cube(cube_uv, CUBEMAP_FACE(k)));
        const auto pano_texel = glm::i32vec2(pano_uv * glm::f32vec2(2 * extent, extent));

        cube_texture.Set(k, i, pano_texture.Get(0, 2ull * extent * pano_texel.y + pano_texel.x));
      }
    }

    prop_tree->GetObjectItem("environment")->SetObjectItem("skybox_cubemap", cube_texture.Export());

    CreateSkyboxCubemap();
    CreateReflectionMap();

    CreateVtxArray();
    CreateIdxArray();

    for (auto i = 0u; i < levels; ++i)
    {
      CreateConstantData(i);
      CreateArgumentList(i);
    }


    for (auto i = 0u; i < levels; ++i)
    {
      CreatePass(i);
      CreateConfig(i);
      CreateBatch(i);
    }
  }

  void EnvironmentBroker::Use()
  {
    for (auto i = 0u; i < levels; ++i)
    {
      passes[i]->SetEnabled(true);
    }

    for (auto i = 0u; i < levels; ++i)
    {
      auto graphic_arg = reinterpret_cast<Batch::Graphic*>(argument_list[i]->Map());

      graphic_arg[0].idx_count = 4u;
      graphic_arg[0].ins_count = 6u;
      graphic_arg[0].idx_offset = 0u;
      graphic_arg[0].vtx_offset = 0u;
      graphic_arg[0].ins_offset = 0u;

      argument_list[i]->Unmap();
    }

    core->GetDevice()->Use();

    for (auto i = 0u; i < levels; ++i)
    {
      passes[i]->SetEnabled(false);
    }
  }

  void EnvironmentBroker::Discard()
  {
    for (auto i = 0u; i < levels; ++i)
    {
      DestroyBatch(i);
      DestroyConfig(i);
      DestroyPass(i);
    }

    for (auto i = 0u; i < levels; ++i)
    {
      DestroyConstantData(i);
      DestroyArgumentList(i);
    }

    DestroyVtxArray();
    DestroyIdxArray();

    //DestroySkyboxCubemap();
    //DestroyReflectionMap();
  }

  void EnvironmentBroker::CreateVtxArray()
  {
    static const std::array<glm::f32vec4, 4> quad_vtx = {
      glm::f32vec4(-1.0f, 1.0f, 0.0f, 0.0f),
      glm::f32vec4( 1.0f, 1.0f, 1.0f, 0.0f),
      glm::f32vec4(-1.0f,-1.0f, 0.0f, 1.0f),
      glm::f32vec4( 1.0f,-1.0f, 1.0f, 1.0f),
    };

    vtx_array = core->GetDevice()->CreateResource("environment_vtx_array",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        sizeof(glm::f32vec4),
        quad_vtx.size(),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(quad_vtx.data()), quad_vtx.size() * sizeof(glm::f32vec4) }
    );
  }

  void EnvironmentBroker::CreateIdxArray()
  {
    static const std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    idx_array = core->GetDevice()->CreateResource("environment_idx_array",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        sizeof(glm::u32vec3),
        quad_idx.size(),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(quad_idx.data()), quad_idx.size() * sizeof(glm::u32vec3) }
    );
  }

  void EnvironmentBroker::CreateReflectionMap()
  {
    const auto format = FORMAT_R16G16B16A16_FLOAT;
    const auto layers = 6u;
    const auto mipmap = levels;
    const auto size_x = 1u << int32_t(levels) - 1;
    const auto size_y = 1u << int32_t(levels) - 1;

    reflection_map = core->GetDevice()->CreateResource("environment_reflection_map",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE),
        mipmap,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE)
    );
  }

  void EnvironmentBroker::CreateSkyboxCubemap()
  {
    const auto& prop_tree = wrap.GetUtil()->GetStorage()->GetTree();
    const auto& prop_environment = prop_tree->GetObjectItem("environment");
    const auto& prop_skybox = prop_environment->GetObjectItem("skybox_cubemap");

    const auto format = Format(prop_skybox->GetObjectItem("format")->GetUint());
    const auto layers = prop_skybox->GetObjectItem("layers")->GetUint();
    const auto mipmap = prop_skybox->GetObjectItem("mipmap")->GetUint();
    const auto size_x = prop_skybox->GetObjectItem("size_x")->GetUint();
    const auto size_y = prop_skybox->GetObjectItem("size_y")->GetUint();
    const auto bytes = prop_skybox->GetObjectItem("raw")->GetRawBytes();

    skybox_cubemap = core->GetDevice()->CreateResource("environment_skybox_cubemap",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE),
      bytes
    );
  }

  void EnvironmentBroker::CreateConstantData(uint32_t level)
  {
    const auto constant = glm::u32vec4{ level, 1u << int32_t(levels) - (1 + level), 0u, 0u };
    
    const auto data = reinterpret_cast<const uint8_t*>(&constant);
    const auto count = 1u;
    const auto stride = sizeof(glm::u32vec4);
    const auto bytes = std::pair{ reinterpret_cast<const uint8_t*>(&constant), count * stride };

    std::pair<const uint8_t*, size_t> interops[] = {
      { data, count * stride },
    };

    constant_data[level] = core->GetDevice()->CreateResource("environment_constant_data_" + std::to_string(level),
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        stride,
        count,
      },
      Resource::Hint(Resource::HINT_UNKNOWN),
      bytes
    );
  }


  void EnvironmentBroker::CreateArgumentList(uint32_t level)
  {
    argument_list[level] = core->GetDevice()->CreateResource("environment_argument_list_" + std::to_string(level),
      Resource::BufferDesc
      {
        Usage(USAGE_ARGUMENT_LIST),
        sizeof(Batch::Graphic),
        1u,
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );
  }


  void EnvironmentBroker::CreatePass(uint32_t level)
  {
    auto environment_reflection_target = reflection_map->CreateView("environment_reflection_target_" + std::to_string(level),
      Usage(USAGE_RENDER_TARGET),
      { level, 1u },
      { 0u, 6u },
      View::BIND_CUBEMAP_LAYER
    );

    const Pass::RTAttachment rt_attachments[] = {
      { environment_reflection_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
    };

    const auto size_x = 1u << int32_t(levels) - (1 + level);
    const auto size_y = 1u << int32_t(levels) - (1 + level);
    const auto layers = 6u;

    passes[level] = core->GetDevice()->CreatePass("environment_reflection_pass_" + std::to_string(level),
      Pass::TYPE_GRAPHIC,
      size_x,
      size_y,
      layers,
      { rt_attachments, std::size(rt_attachments) },
      {}
    );
  }


  void EnvironmentBroker::CreateConfig(uint32_t level)
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_reflection_probe.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const Config::IAState ia_config =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0, 0, 16, FORMAT_R32G32_FLOAT, false },
        { 0, 8, 16, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Config::RCState rc_config =
    {
      Config::FILL_SOLID,
      Config::CULL_NONE,
      {
        { 0.0f, 0.0f, float(1u << int32_t(levels) - (1 + level)), float(1u << int32_t(levels) - (1 + level)), 0.0f, 1.0f },
        { 0.0f, 0.0f, float(1u << int32_t(levels) - (1 + level)), float(1u << int32_t(levels) - (1 + level)), 0.0f, 1.0f },
        { 0.0f, 0.0f, float(1u << int32_t(levels) - (1 + level)), float(1u << int32_t(levels) - (1 + level)), 0.0f, 1.0f },
        { 0.0f, 0.0f, float(1u << int32_t(levels) - (1 + level)), float(1u << int32_t(levels) - (1 + level)), 0.0f, 1.0f },
        { 0.0f, 0.0f, float(1u << int32_t(levels) - (1 + level)), float(1u << int32_t(levels) - (1 + level)), 0.0f, 1.0f },
        { 0.0f, 0.0f, float(1u << int32_t(levels) - (1 + level)), float(1u << int32_t(levels) - (1 + level)), 0.0f, 1.0f },
      },
    };

    const Config::DSState ds_config =
    {
      false, //depth_enabled
      false, //depth_write
      Config::COMPARISON_ALWAYS //depth_comparison
    };

    const Config::OMState om_config =
    {
      false,
      {
        { false, Config::OPERAND_SRC_ALPHA, Config::OPERAND_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::OPERAND_INV_SRC_ALPHA, Config::OPERAND_ZERO, Config::OPERATION_ADD, 0xF },
      }
    };

    configs[level] = passes[level]->CreateConfig("environment_reflection_config_" + std::to_string(level),
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_GS | Config::COMPILATION_PS),
      { defines.data(), defines.size() },
      ia_config,
      rc_config,
      ds_config,
      om_config
    );
  }


  void EnvironmentBroker::CreateBatch(uint32_t level)
  {
    const auto vtx_view = vtx_array->CreateView("environment_vtx_view_" + std::to_string(level),
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      vtx_view,
    };

    const auto idx_view = idx_array->CreateView("environment_idx_view_" + std::to_string(level),
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      idx_view,
    };

    const auto argument_view = argument_list[level]->CreateView("environment_argument_view_" + std::to_string(level),
      Usage(USAGE_ARGUMENT_LIST)
    );

    const auto& ins_range = View::Range{ 0u, 6u };
    const auto& vtx_range = View::Range{ 0u, 4u };
    const auto& idx_range = View::Range{ 0u, 6u };
    const auto& sb_offset = std::nullopt; // std::array<uint32_t, 4>{ 0u, 0u, 0u, 0u };
    const auto& push_data = std::nullopt;

    const auto entity = Batch::Entity{
      { va_views, va_views + std::size(va_views) },
      { ia_views, ia_views + std::size(ia_views) },
      nullptr, //argument_view,
      ins_range,
      vtx_range,
      idx_range,
      sb_offset,
      push_data
    };

    const Batch::Sampler samplers[] = {
      { Batch::Sampler::FILTERING_LINEAR, 0, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };

    const auto constant_view = constant_data[level]->CreateView("environment_argument_view_" + std::to_string(level),
      Usage(USAGE_CONSTANT_DATA)
    );

    const std::shared_ptr<View> ub_views[] = {
      constant_view,
    };

    const auto skybox_view = skybox_cubemap->CreateView("environment_skybox_view_" + std::to_string(level),
      Usage(USAGE_SHADER_RESOURCE),
      { uint32_t(std::min(0, int32_t(quality) - int32_t(levels))), levels},
      { 0u, 6u },
      View::BIND_CUBEMAP_LAYER
    );


    const std::shared_ptr<View> ri_views[] = {
      skybox_view,
    };

    batches[level] = configs[level]->CreateBatch("environment_reflection_batch_" + std::to_string(level),
      { &entity, 1u },
      { samplers, std::size(samplers) },
      { ub_views, std::size(ub_views) },
      {},
      { ri_views, std::size(ri_views) },
      {},
      {},
      {}
    );
  }

  void EnvironmentBroker::DestroyPass(uint32_t level)
  {
    core->GetDevice()->DestroyPass(passes[level]);
    passes[level].reset();
  }

  void EnvironmentBroker::DestroyConfig(uint32_t level)
  {
    passes[level]->DestroyConfig(configs[level]);
    configs[level].reset();
  }

  void EnvironmentBroker::DestroyBatch(uint32_t level)
  {
    configs[level]->DestroyBatch(batches[level]);
    batches[level].reset();
  }

  void EnvironmentBroker::DestroyVtxArray()
  {
    core->GetDevice()->DestroyResource(vtx_array);
    vtx_array.reset();
  }

  void EnvironmentBroker::DestroyIdxArray()
  {
    core->GetDevice()->DestroyResource(idx_array);
    idx_array.reset();
  }

  void EnvironmentBroker::DestroySkyboxCubemap()
  {
    core->GetDevice()->DestroyResource(skybox_cubemap);
    skybox_cubemap.reset();
  }

  void EnvironmentBroker::DestroyReflectionMap()
  {
    core->GetDevice()->DestroyResource(reflection_map);
    reflection_map.reset();
  }

  void EnvironmentBroker::DestroyConstantData(uint32_t level)
  {
    core->GetDevice()->DestroyResource(constant_data[level]);
    constant_data[level].reset();
  }

  void EnvironmentBroker::DestroyArgumentList(uint32_t level)
  {
    core->GetDevice()->DestroyResource(argument_list[level]);
    argument_list[level].reset();
  }

  EnvironmentBroker::EnvironmentBroker(Wrap& wrap)
    : Broker("environment_broker", wrap)
    , core(wrap.GetCore())
    , util(wrap.GetUtil())
  {}

  EnvironmentBroker::~EnvironmentBroker()
  {}
}