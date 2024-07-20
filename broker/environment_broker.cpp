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
    const auto mipmap = 3u;
    const auto extent = 1u << int32_t(mipmap) - 1;
    

    //auto texture_property = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
    //texture_property->SetArraySize(cubemap_layer_counter);

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
      //const auto r = std::hypot(xyz.x, xyz.y);
      //const auto theta = std::atan2(xyz.y, xyz.x);
      //const auto phi = std::atan2(xyz.z, r);

      const auto theta = std::atan2(xyz.x, xyz.z);
      const auto phi = std::asin(xyz.y);

      const auto u = 0.5f * (theta + glm::pi<float>()) / glm::pi<float>();
      const auto v = (glm::half_pi<float>() - phi) / glm::pi<float>(); //(glm::half_pi<float>() - phi) / glm::pi<float>();

      return glm::f32vec2(u, v);
    };

    const auto i = 255u;
    const auto j = 255u;
    const auto k = FACE_NEGATIVE_Z;
    const auto cube_uv = glm::f32vec2{ (i + 0.5f) / extent, (j + 0.5f) / extent };
    const auto xyz = xyz_from_cube(cube_uv, k);
    const auto pano_uv = pano_from_xyz(xyz);

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


          //glm::f32vec2 texels;
          //const auto weights = glm::modf(pano_uv * glm::f32vec2(pano_extent_x, pano_extent_y), texels);
          //
          //const auto texels_00 = (glm::i32vec2(texels) + glm::i32vec2(0, 0)) % glm::i32vec2(pano_extent_x, pano_extent_y);
          //const auto texels_10 = (glm::i32vec2(texels) + glm::i32vec2(1, 0)) % glm::i32vec2(pano_extent_x, pano_extent_y);
          //const auto texels_01 = (glm::i32vec2(texels) + glm::i32vec2(0, 1)) % glm::i32vec2(pano_extent_x, pano_extent_y);
          //const auto texels_11 = (glm::i32vec2(texels) + glm::i32vec2(1, 1)) % glm::i32vec2(pano_extent_x, pano_extent_y);
          //
          //const auto& values_00 = pano_raw.GetElement<glm::f32vec4>(texels_00.y * pano_extent_x + texels_00.x);
          //const auto& values_10 = pano_raw.GetElement<glm::f32vec4>(texels_10.y * pano_extent_x + texels_10.x);
          //const auto& values_01 = pano_raw.GetElement<glm::f32vec4>(texels_01.y * pano_extent_x + texels_01.x);
          //const auto& values_11 = pano_raw.GetElement<glm::f32vec4>(texels_11.y * pano_extent_x + texels_11.x);
          //
          //const auto result = glm::mix(glm::mix(values_00, values_01, weights.x), glm::mix(values_10, values_01, weights.x), weights.y);

          raw.SetElement(value, j * extent + i);
        }
      }

      //const auto texture = std::make_tuple(std::move(raw), extent, extent);
      //auto [levels, extent_x, extent_y] = MipmapTextureHDR(mipmap, texture);
      //std::move(levels.begin(), levels.end(), std::back_inserter(raws));

      raws.push_back(std::move(raw));
    }

    const auto prop_skybox = CreateTextureProperty({ raws.data(), uint32_t(raws.size()) }, extent, extent, 1u, layers);

    const auto prop_scene = tree->GetObjectItem("scene_property");
    prop_scene->SetObjectItem("skybox", prop_skybox);
  }

  void EnvironmentBroker::Discard()
  {}

  EnvironmentBroker::EnvironmentBroker(Wrap& wrap)
    : Broker("environment_broker", wrap)
  {}

  EnvironmentBroker::~EnvironmentBroker()
  {}
}