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

  void EnvironmentBroker::CreateReflectionMap()
  {
    const auto layers = 6u;
    const auto extent = 1u << int32_t(mipmap) - 1;

    skybox_cubemap = core->GetDevice()->CreateResource("environment_reflection_map",
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

  void EnvironmentBroker::CreateConstantArray()
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

    constant_array = core->GetDevice()->CreateResource("environment_constant_array",
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

  void EnvironmentBroker::CreateBatch(uint32_t mipmap)
  {
    const auto layout = device->CreateLayout(name);

    const Layout::Sampler samplers[] = {
    { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f }
    };
    layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });

    const auto reflection_level_data = reflection_probe_data->CreateView("spark_reflection_level_data");
    {
      reflection_level_data->SetBind(View::BIND_CONSTANT_DATA);
      reflection_level_data->SetByteOffset(0);
      reflection_level_data->SetByteCount(sizeof(ReflectionProbeLevel));
    }

    const std::shared_ptr<View> sb_views[] = {
      reflection_level_data,
    };
    layout->UpdateSBViews({ sb_views, uint32_t(std::size(sb_views)) });

    const auto skybox_skybox_texture = skybox_texture->CreateView("spark_skybox_skybox_texture");
    {
      skybox_skybox_texture->SetBind(View::BIND_SHADER_RESOURCE);
      skybox_skybox_texture->SetUsage(View::USAGE_CUBEMAP_LAYER);
    }

    const std::shared_ptr<View> ri_views[] = {
      skybox_skybox_texture
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    return layout;
  }

  void EnvironmentBroker::CreateTechnique(uint32_t mipmap)
  {
    const auto config = device->CreateConfig(name);

    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_reflection_probe.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    config->SetSource(shader_ss.str());
    config->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_GS | Config::COMPILATION_PS));
    config->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
    config->SetIndexer(Config::INDEXER_32_BIT);

    const Config::Attribute attributes[] = {
      { 0, 0, 16, FORMAT_R32G32_FLOAT, false },
      { 0, 8, 16, FORMAT_R32G32_FLOAT, false },
    };
    config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

    config->SetFillMode(Config::FILL_SOLID);
    config->SetCullMode(Config::CULL_NONE);
    config->SetClipEnabled(false);

    const Config::Blend blends[] = {
      { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
    };
    config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
    config->SetATCEnabled(false);

    config->SetDepthEnabled(false);
    config->SetDepthWrite(false);

    const auto mip_size = reflection_probe_size >> mip_level;

    const Config::Viewport viewports[] = {
      { 0.0f, 0.0f, float(mip_size), float(mip_size), 0.0f, 1.0f },
    };
    config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });

    return config;
  }

  void EnvironmentBroker::CreatePass(uint32_t mipmap)
  {
    const auto pass = device->CreatePass(name);

    pass->SetExtentX(reflection_probe_size >> mipmap);
    pass->SetExtentY(reflection_probe_size >> mipmap);

    pass->SetType(Pass::TYPE_GRAPHIC);
    pass->SetEnabled(true);

    const auto reflection_color_target = reflection_probe->CreateView("spark_reflection_color_target");
    {
      reflection_color_target->SetBind(View::BIND_RENDER_TARGET);
      reflection_color_target->SetLayerOffset(0);
      reflection_color_target->SetLayerCount(6);

      reflection_color_target->SetMipmapCount(1);
      reflection_color_target->SetMipmapOffset(mip_level);
    }

    const std::shared_ptr<View> rt_views[] = {
      reflection_color_target,
    };
    pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

    const Pass::RTValue rt_values[] = {
      std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f },
    };
    pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

    pass->SetSubpassCount(1);


    Pass::Command commands[] = {
      {nullptr, {6, 6, 0, 0, 0, 0, 0, 0}},
    };
    commands[0].offsets = { mip_level * uint32_t(sizeof(ReflectionProbeLevel)) };
    pass->UpdateSubpassCommands(0, { commands, uint32_t(std::size(commands)) });

    const auto reflection_probe_vertices = skybox_vertices->CreateView("spark_reflection_vertices");
    {
      reflection_probe_vertices->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const std::shared_ptr<View> va_views[] = {
      reflection_probe_vertices,
    };
    pass->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });

    const auto reflection_probe_triangles = skybox_triangles->CreateView("spark_reflection_triangles");
    {
      reflection_probe_triangles->SetBind(View::BIND_INDEX_ARRAY);
    }

    const std::shared_ptr<View> ia_views[] = {
      reflection_probe_triangles,
    };
    pass->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });

    pass->SetSubpassLayout(0, reflection_probe_layout);
    pass->SetSubpassConfig(0, reflection_probe_configs[mip_level]);

    return pass;
  }

  EnvironmentBroker::EnvironmentBroker(Wrap& wrap)
    : Broker("environment_broker", wrap)
    , core(wrap.GetCore())
    , util(wrap.GetUtil())
  {}

  EnvironmentBroker::~EnvironmentBroker()
  {}
}