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
#include "lightmap_broker.h"


namespace RayGene3D
{
  void RasterizeTriangle(const glm::f32vec2& p0, const glm::f32vec2& p1, const glm::f32vec2& p2,
    uint32_t inst_id, uint32_t prim_id, uint32_t size_x, uint32_t size_y, Raw& raw)
  {
    const auto eval_barycentric_fn = [](const glm::f32vec2& p,
      const glm::f32vec2& a, const glm::f32vec2& b, const glm::f32vec2& c)
      {
        const auto v0 = b - a, v1 = c - a, v2 = p - a;
        const auto d00 = glm::dot(v0, v0);
        const auto d01 = glm::dot(v0, v1);
        const auto d11 = glm::dot(v1, v1);
        const auto d20 = glm::dot(v2, v0);
        const auto d21 = glm::dot(v2, v1);
        const auto denom = d00 * d11 - d01 * d01;
        const auto v = (d11 * d20 - d01 * d21) / denom;
        const auto w = (d00 * d21 - d01 * d20) / denom;
        const auto u = 1.0f - v - w;
        return glm::f32vec3(u, v, w);
      };

    const auto bb_min = glm::ivec2(glm::floor(glm::min(p0, glm::min(p1, p2))));
    const auto bb_max = glm::ivec2(glm::ceil(glm::max(p0, glm::max(p1, p2))));

    for (auto n = bb_min.y; n < bb_max.y; ++n)
    {
      for (auto m = bb_min.x; m < bb_max.x; ++m)
      {
        const auto barycentric = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(0.5f, 0.5f), p0, p1, p2);
        if (glm::any(glm::lessThan(barycentric, glm::zero<glm::f32vec3>()))
          || glm::any(glm::greaterThan(barycentric, glm::one<glm::f32vec3>())))
        {
          const auto barycentric_00 = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(0.0f, 0.0f), p0, p1, p2);
          const auto missed_00 = glm::any(glm::lessThan(barycentric_00, glm::zero<glm::f32vec3>()))
            || glm::any(glm::greaterThan(barycentric_00, glm::one<glm::f32vec3>()));

          const auto barycentric_01 = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(0.0f, 1.0f), p0, p1, p2);
          const auto missed_01 = glm::any(glm::lessThan(barycentric_01, glm::zero<glm::f32vec3>()))
            || glm::any(glm::greaterThan(barycentric_01, glm::one<glm::f32vec3>()));

          const auto barycentric_10 = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(1.0f, 0.0f), p0, p1, p2);
          const auto missed_10 = glm::any(glm::lessThan(barycentric_10, glm::zero<glm::f32vec3>()))
            || glm::any(glm::greaterThan(barycentric_10, glm::one<glm::f32vec3>()));

          const auto barycentric_11 = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(1.0f, 1.0f), p0, p1, p2);
          const auto missed_11 = glm::any(glm::lessThan(barycentric_11, glm::zero<glm::f32vec3>()))
            || glm::any(glm::greaterThan(barycentric_11, glm::one<glm::f32vec3>()));

          if (missed_00 && missed_01 && missed_10 && missed_11) continue;
        }

        const auto u = *reinterpret_cast<const uint32_t*>(&barycentric[0]);
        const auto v = *reinterpret_cast<const uint32_t*>(&barycentric[1]);

        const auto value = glm::u32vec4{ inst_id, prim_id, u, v };
        const auto index = n * size_x + m;

        raw.SetElement<glm::u32vec4>(value, index);
      }
    }
  }

  void LightmapBroker::CreateTraceInstances()
  {
  }

  void LightmapBroker::CreateTraceTriangles()
  {
  }

  void LightmapBroker::CreateTraceVertices()
  {
  }

  void LightmapBroker::CreateLightmapsInput()
  {
    const auto& layers = prop_atlas->GetObjectItem("layers");
    const auto& mipmap = prop_atlas->GetObjectItem("mipmap");
    const auto& extent_x = prop_atlas->GetObjectItem("extent_x");
    const auto& extent_y = prop_atlas->GetObjectItem("extent_y");
    const auto& raws = prop_atlas->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
    }

    lightmaps_input = wrap.GetCore()->GetDevice()->CreateResource("lightmaps_input",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R32G32B32A32_FLOAT,
        extent_x->GetUint(),
        extent_y->GetUint(),
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
      );
  }

  void LightmapBroker::CreateLightmapsAccum()
  {
  }

  void LightmapBroker::CreateLightmapsFinal()
  {
  }

  void LightmapBroker::CreateComputePass()
  {
  }

  void LightmapBroker::CreateComputeConfig()
  {
  }

  void LightmapBroker::CreateComputeBatch()
  {
  }

  void LightmapBroker::CreateComputeArguments()
  {
  }

  void LightmapBroker::DiscardTraceInstances()
  {
  }

  void LightmapBroker::DiscardTraceTriangles()
  {
  }

  void LightmapBroker::DiscardTraceVertices()
  {
  }

  void LightmapBroker::DestroyLightmapsInput()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(lightmaps_input);
    lightmaps_input.reset();
  }

  void LightmapBroker::DestroyLightmapsAccum()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(lightmaps_accum);
    lightmaps_accum.reset();
  }

  void LightmapBroker::DestroyLightmapsFinal()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(lightmaps_final);
    lightmaps_final.reset();
  }

  void LightmapBroker::DiscardComputePass()
  {
  }

  void LightmapBroker::DiscardComputeConfig()
  {
  }

  void LightmapBroker::DiscardComputeBatch()
  {
  }

  void LightmapBroker::DiscardComputeArguments()
  {
  }



  void LightmapBroker::Initialize()
  {
  }

  void LightmapBroker::Use()
  {
  }

  void LightmapBroker::Discard()
  {
    //prop_scene->SetObjectItem("lightmaps", prop_lightmaps);

    prop_vertices.reset();
    prop_triangles.reset();
    prop_instances.reset();

    prop_scene.reset();

    prop_atlas_size_x.reset();
    prop_atlas_size_y.reset();
    prop_atlas_layers.reset();

    prop_illumination.reset();
  }

  LightmapBroker::LightmapBroker(Wrap& wrap)
    : Broker("lightmap_broker", wrap)
  {
    const auto& prop_tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_scene = prop_tree->GetObjectItem("scene");
    {
      prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);
      prop_triangles = prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->GetArrayItem(0);
      prop_vertices = prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->GetArrayItem(0);
    }

    prop_illumination = prop_tree->GetObjectItem("illumination");
    {
      prop_atlas_size_x = prop_illumination->GetObjectItem("atlas_size_x");
      prop_atlas_size_y = prop_illumination->GetObjectItem("atlas_size_y");
      prop_atlas_layers = prop_illumination->GetObjectItem("atlas_layers");
    }

    const auto atlas_size_x = prop_atlas_size_x->GetUint();
    const auto atlas_size_y = prop_atlas_size_y->GetUint();
    const auto atlas_layers = prop_atlas_layers->GetUint();

    auto raws = std::vector<Raw>(atlas_layers);
    for (auto& raw : raws)
    {
      raw.Allocate(atlas_size_x * atlas_size_y * uint32_t(sizeof(glm::u32vec4)));

      for (auto i = 0u; i < atlas_size_x * atlas_size_y; ++i)
      {
        raw.SetElement(glm::u32vec4{ uint32_t(-1), uint32_t(-1), 0, 0 }, i);
      }
    }

    {
      const auto [ins_array, ins_count] = prop_instances->GetTypedBytes<Instance>(0);
      const auto [trg_array, trg_count] = prop_triangles->GetTypedBytes<Triangle>(0);
      const auto [vrt_array, vrt_count] = prop_vertices->GetTypedBytes<Vertex>(0);

      for (uint32_t i = 0; i < ins_count; ++i)
      {
        const auto& ins = ins_array[i];

        for (uint32_t j = 0; j < ins.prim_count; ++j)
        {
          const auto& trg = trg_array[ins.prim_offset + j];

          const auto& vtx0 = vrt_array[ins.vert_offset + trg.idx[0]];
          const auto& vtx1 = vrt_array[ins.vert_offset + trg.idx[1]];
          const auto& vtx2 = vrt_array[ins.vert_offset + trg.idx[2]];

          const auto& layer = vtx0.msk;
          const auto& color = vtx0.col;

          const auto p0 = vtx0.tc1 * glm::f32vec2(atlas_size_x, atlas_size_y);
          const auto p1 = vtx1.tc1 * glm::f32vec2(atlas_size_x, atlas_size_y);
          const auto p2 = vtx2.tc1 * glm::f32vec2(atlas_size_x, atlas_size_y);

          RasterizeTriangle(p0, p1, p2, i, j, atlas_size_x, atlas_size_y, raws[layer]);
        }
      }
    }

    prop_atlas = CreateTextureProperty({ raws.data(), uint32_t(raws.size()) }, atlas_size_x, atlas_size_y, 1u, atlas_layers);

    CreateTraceInstances();
    CreateTraceTriangles();
    CreateTraceVertices();

    CreateLightmapsInput();
    CreateLightmapsAccum();
    CreateLightmapsFinal();

    CreateComputeArguments();

    CreateComputePass();
    CreateComputeConfig();
    CreateComputeBatch();

    //prop_atlas.reset();    
  }

  LightmapBroker::~LightmapBroker()
  {
    DiscardComputeBatch();
    DiscardComputeConfig();
    DiscardComputePass();

    DiscardComputeArguments();

    //DestroyLightmapsInput();
    //DestroyLightmapsAccum();
    //DestroyLightmapsFinal();

    DiscardTraceInstances();
    DiscardTraceTriangles();
    DiscardTraceVertices();
  }
}