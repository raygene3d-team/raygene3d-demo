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
  void EnvironmentBroker::Initialize()
  {}

  void EnvironmentBroker::Use()
  {
    const auto tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_environment = tree->GetObjectItem("environment_property");

    const auto pano_extent_x = prop_environment->GetObjectItem("extent_x")->GetUint();
    const auto pano_extent_y = prop_environment->GetObjectItem("extent_y")->GetUint();
    const auto pano_mipmap = prop_environment->GetObjectItem("mipmap")->GetUint();
    const auto pano_layers = prop_environment->GetObjectItem("layers")->GetUint();

    const auto& pano_raw = prop_environment->GetObjectItem("raws")->GetArrayItem(0)->GetRaw();

    const auto layers = 6u;
    const auto extent = 1u << int32_t(mipmap) - 1;

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

    auto raws = std::vector<Raw>();

    for (uint32_t k = 0; k < layers; ++k)
    {
      auto raw = Raw(extent * extent * uint32_t(sizeof(glm::f32vec4)));

      for (uint32_t j = 0; j < extent; ++j)
      {
        for (uint32_t i = 0; i < extent; ++i)
        {
          const auto cube_uv = glm::f32vec2{ (i + 0.5f) / extent, (j + 0.5f) / extent };
          const auto pano_uv = pano_from_xyz(xyz_from_cube(cube_uv, CUBEMAP_FACE(k)));

          const auto texel = glm::i32vec2(pano_uv * glm::f32vec2(pano_extent_x, pano_extent_y));
          const auto& value = pano_raw.GetElement<glm::f32vec4>(texel.y * pano_extent_x + texel.x);

          raw.SetElement(value, j * extent + i);
        }
      }

      raws.push_back(std::move(raw));
    }

    prop_environment = CreateTextureProperty({ raws.data(), uint32_t(raws.size()) }, extent, extent, 1u, layers);

    const auto prop_scene = tree->GetObjectItem("scene_property");
    prop_scene->SetObjectItem("skybox", prop_environment);
  }

  void EnvironmentBroker::Discard()
  {}

  void EnvironmentBroker::CreateVtxArray()
  {
    static const std::array<glm::f32vec2, 4> quad_vtx = {
      glm::f32vec2(-1.0f, 1.0f),
      glm::f32vec2( 1.0f, 1.0f),
      glm::f32vec2(-1.0f,-1.0f),
      glm::f32vec2( 1.0f,-1.0f),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_vtx.data(), uint32_t(quad_vtx.size() * sizeof(glm::f32vec2)) },
    };

    vtx_array = core->GetDevice()->CreateResource("environment_vtx_array",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        uint32_t(sizeof(glm::f32vec2)),
        uint32_t(quad_vtx.size()),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void EnvironmentBroker::CreateIdxArray()
  {
    static const std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_idx.data(), uint32_t(quad_idx.size() * sizeof(glm::u32vec3)) },
    };

    idx_array = core->GetDevice()->CreateResource("environment_idx_array",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        uint32_t(sizeof(glm::u32vec3)),
        uint32_t(quad_idx.size()),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void EnvironmentBroker::CreateReflectionMap()
  {
    const auto layers = 6u;
    const auto extent = 1u << int32_t(mipmap) - 1;

    reflection_map = core->GetDevice()->CreateResource("environment_reflection_map",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET),
        mipmap,
        layers,
        FORMAT_R16G16B16A16_FLOAT,
        extent,
        extent,
      },
      Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE)
    );
  }

  void EnvironmentBroker::CreateSkyboxCubemap()
  {
    const auto layers = 6u;
    const auto extent = 1u << int32_t(mipmap) - 1;
    const auto& raws = prop_environment->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
    }

    skybox_cubemap = core->GetDevice()->CreateResource("environment_skybox_cubemap",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap,
        layers,
        FORMAT_R32G32B32A32_FLOAT,
        extent,
        extent,
      },
      Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void EnvironmentBroker::CreateConstantData()
  {
    ReflectionProbeLevel array[mipmap];
    for (auto i = 0u; i < mipmap; ++i)
    {
      array[i] = {i, 1u << int32_t(mipmap) - (i + 1)};
    }
    const auto count = uint32_t(std::size(array));
    const auto stride = uint32_t(sizeof(ReflectionProbeLevel));

    std::pair<const void*, uint32_t> interops[] = {
      { array, count * stride },
    };

    constant_data = core->GetDevice()->CreateResource("environment_constant_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        stride,
        count,
      },
      Resource::Hint(Resource::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
      );
  }


  void EnvironmentBroker::CreateArgumentList()
  {
    argument_list = core->GetDevice()->CreateResource("environment_argument_list",
      Resource::BufferDesc
      {
        Usage(USAGE_ARGUMENT_LIST),
        uint32_t(sizeof(Batch::Graphic)),
        1u,
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );
  }


  void EnvironmentBroker::CreatePass(uint32_t mipmap)
  {
    //const auto extent_x = scope.prop_extent_x->GetUint();
    //const auto extent_y = scope.prop_extent_y->GetUint();
    //const auto extent_z = 1u;

    auto environment_reflection_target = reflection_map->CreateView("environment_reflection_target_" + std::to_string(mipmap),
      Usage(USAGE_RENDER_TARGET),
      { mipmap, 1u },
      { 0u, 6u },
      View::BIND_CUBEMAP_LAYER
    );

    const Pass::RTAttachment rt_attachments[] = {
      { environment_reflection_target, std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } },
    };

    const Pass::DSAttachment ds_attachments[] = {
      {}
    };

    pass = core->GetDevice()->CreatePass("environment_reflection_pass_" + std::to_string(mipmap),
      Pass::TYPE_GRAPHIC,
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }


  void EnvironmentBroker::CreateTechnique(uint32_t mipmap)
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_reflection_probe.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::vector<std::pair<std::string, std::string>> defines;
    //defines.push_back({ "NORMAL_ENCODING_ALGORITHM", normal_encoding_method });

    const Technique::IAState ia_technique =
    {
      Technique::TOPOLOGY_TRIANGLELIST,
      Technique::INDEXER_32_BIT,
      {
        { 0, 0, 16, FORMAT_R32G32_FLOAT, false },
        { 0, 8, 16, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Technique::RCState rc_technique =
    {
      Technique::FILL_SOLID,
      Technique::CULL_NONE,
      {
        { 0.0f, 0.0f, float(1u << int32_t(mipmap) - 1), float(1u << int32_t(mipmap) - 1), 0.0f, 1.0f }
      },
    };

    const Technique::DSState ds_technique =
    {
      false, //depth_enabled
      false, //depth_write
      Technique::COMPARISON_ALWAYS //depth_comparison
    };

    const Technique::OMState om_technique =
    {
      false,
      {
        { false, Technique::OPERAND_SRC_ALPHA, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERATION_ADD, Technique::OPERAND_INV_SRC_ALPHA, Technique::OPERAND_ZERO, Technique::OPERATION_ADD, 0xF },
      }
    };

    technique = pass->CreateTechnique("environment_reflection_technique_" + std::to_string(mipmap),
      shader_ss.str(),
      Technique::Compilation(Technique::COMPILATION_VS | Technique::COMPILATION_GS | Technique::COMPILATION_PS),
      { defines.data(), uint32_t(defines.size()) },
      ia_technique,
      rc_technique,
      ds_technique,
      om_technique
    );
  }


  void EnvironmentBroker::CreateBatch(uint32_t mipmap)
  {
    const auto vtx_view = vtx_array->CreateView("environment_vtx_view_" + std::to_string(mipmap),
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      vtx_view,
    };

    const auto idx_view = idx_array->CreateView("environment_idx_view_" + std::to_string(mipmap),
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      idx_view,
    };

    const auto argument_view = argument_list->CreateView("environment_argument_view_" + std::to_string(mipmap),
      Usage(USAGE_ARGUMENT_LIST)
    );

    const auto& ins_range = View::Range{ 1u, 0u };
    const auto& vtx_range = View::Range{ 4u, 0u };
    const auto& idx_range = View::Range{ 6u, 0u };
    const auto& sb_offset = std::array<uint32_t, 4>{ uint32_t(sizeof(ReflectionProbeLevel))* mipmap, 0u, 0u, 0u };
    const auto& push_data = std::nullopt;

    const auto entity = Batch::Entity{
      { va_views, va_views + uint32_t(std::size(va_views)) },
      { ia_views, ia_views + uint32_t(std::size(ia_views)) },
      nullptr, // geometry_graphic_arguments,
      ins_range,
      vtx_range,
      idx_range,
      sb_offset,
      push_data
    };

    const Batch::Sampler samplers[] = {
      { Batch::Sampler::FILTERING_ANISOTROPIC, 16, Batch::Sampler::ADDRESSING_REPEAT, Batch::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };


    const std::shared_ptr<View> ub_views[] = {
      {},
    };


    const auto constant_view = constant_data->CreateView("environment_argument_view_" + std::to_string(mipmap),
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(ReflectionProbeLevel)) }
    );

    const std::shared_ptr<View> sb_views[] = {
      constant_view
    };


    const auto skybox_view = skybox_cubemap->CreateView("environment_skybox_view_" + std::to_string(mipmap),
      Usage(USAGE_SHADER_RESOURCE)
    );


    const std::shared_ptr<View> ri_views[] = {
      skybox_view,
    };

    batch = technique->CreateBatch("environment_reflection_batch_" + std::to_string(mipmap),
      { &entity, 1u },
      { samplers, uint32_t(std::size(samplers)) },
      { ub_views, uint32_t(std::size(ub_views)) },
      { sb_views, uint32_t(std::size(sb_views)) },
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {}
    );
  }

  EnvironmentBroker::EnvironmentBroker(Wrap& wrap)
    : Broker("environment_broker", wrap)
    , core(wrap.GetCore())
    , util(wrap.GetUtil())
  {}

  EnvironmentBroker::~EnvironmentBroker()
  {}
}