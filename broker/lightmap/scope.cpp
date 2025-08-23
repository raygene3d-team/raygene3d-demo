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


#include "scope.h"

namespace RayGene3D
{
  namespace Lightmap
  {
    void RasterizeTriangle(const glm::f32vec2& p0, const glm::f32vec2& p1, const glm::f32vec2& p2,
      uint32_t inst_id, uint32_t prim_id, uint32_t size_x, uint32_t size_y, uint32_t layer, Raw& raw)
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

          raw.SetItem<glm::u32vec4>(value, size_t(size_x * size_y) * layer + index);
        }
      }
    }

    void Scope::CreateScreenData()
    {
      screen_data = core->GetDevice()->CreateResource("lightmap_screen_data",
        Resource::BufferDesc
        {
          Usage(USAGE_CONSTANT_DATA),
          uint32_t(sizeof(Screen)),
          1,
        },
        Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
        );
    }

    void Scope::CreateSceneBufferInst()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_buffer_inst", scene_buffer_inst));
    }

    void Scope::CreateSceneBufferTrng()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_buffer_trng", scene_buffer_trng));
    }

    void Scope::CreateSceneBufferVert()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_buffer_vert", scene_buffer_vert));
    }

    void Scope::CreateTraceBufferTBox()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_tbox", trace_buffer_tbox));
    }

    void Scope::CreateTraceBufferBBox()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_bbox", trace_buffer_bbox));
    }

    void Scope::CreateTraceBufferInst()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_inst", trace_buffer_inst));
    }

    void Scope::CreateTraceBufferTrng()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_trng", trace_buffer_trng));
    }

    void Scope::CreateTraceBufferVert()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_vert", trace_buffer_vert));
    }

    void Scope::CreateSceneArrayAAAM()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_array_aaam", scene_array_aaam));
    }

    void Scope::CreateSceneArraySNNO()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_array_snno", scene_array_snno));
    }

    void Scope::CreateSceneArrayEEET()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_array_eeet", scene_array_eeet));
    }

    void Scope::CreateLightmapsInput()
    {
      const auto format = FORMAT_R32G32B32A32_UINT;
      const auto size_x = prop_atlas_size_x->GetUint();
      const auto size_y = prop_atlas_size_y->GetUint();
      const auto layers = prop_atlas_layers->GetUint();
      const auto mipmap = 1u;

      auto raw = Raw(size_x * size_y * layers, glm::u32vec4{ uint32_t(-1), uint32_t(-1), 0, 0 });
      {
        const auto [ins_array, ins_count] = prop_inst->GetObjectItem("binary")->GetRawItems<Instance>(0);
        const auto [trg_array, trg_count] = prop_trng->GetObjectItem("binary")->GetRawItems<Triangle>(0);
        const auto [vrt_array, vrt_count] = prop_vert->GetObjectItem("binary")->GetRawItems<Vertex>(0);

        for (uint32_t i = 0; i < ins_count; ++i)
        {
          const auto& ins = ins_array[i];

          for (uint32_t j = 0; j < ins.trng_count; ++j)
          {
            const auto& trg = trg_array[ins.trng_offset + j];

            const auto& vtx0 = vrt_array[ins.vert_offset + trg.idx[0]];
            const auto& vtx1 = vrt_array[ins.vert_offset + trg.idx[1]];
            const auto& vtx2 = vrt_array[ins.vert_offset + trg.idx[2]];

            const auto& layer = vtx0.msk;
            const auto& color = vtx0.col;

            const auto p0 = vtx0.tc1 * glm::f32vec2(size_x, size_y);
            const auto p1 = vtx1.tc1 * glm::f32vec2(size_x, size_y);
            const auto p2 = vtx2.tc1 * glm::f32vec2(size_x, size_y);

            //RasterizeTriangle(p0, p1, p2, i, j, size_x, size_y, layer, raw);
          }
        }
      }

      lightmaps_input = core->GetDevice()->CreateResource("lightmaps_input",
        Resource::Tex2DDesc
        {
          Usage(USAGE_SHADER_RESOURCE),
          mipmap,
          layers,
          format,
          size_x,
          size_y,
        },
        Resource::Hint(Resource::HINT_LAYERED_IMAGE),
        raw.GetBytes());
    }

    void Scope::CreateLightmapsAccum()
    {
      const auto format = FORMAT_R32G32B32A32_FLOAT;
      const auto size_x = prop_atlas_size_x->GetUint();
      const auto size_y = prop_atlas_size_y->GetUint();
      const auto layers = prop_atlas_layers->GetUint();
      const auto mipmap = 1u;

      const auto raw = Raw(size_x * size_y * layers, glm::zero<glm::f32vec4>());

      lightmaps_accum = core->GetDevice()->CreateResource("lightmaps_accum",
        Resource::Tex2DDesc
        {
          Usage(USAGE_UNORDERED_ACCESS | USAGE_SHADER_RESOURCE),
          mipmap,
          layers,
          format,
          size_x,
          size_y,
        },
        Resource::Hint(Resource::HINT_LAYERED_IMAGE),
        raw.GetBytes());
    }

    void Scope::CreateLightmapsFinal()
    {
      const auto format = FORMAT_R32G32B32A32_FLOAT;
      const auto size_x = prop_atlas_size_x->GetUint();
      const auto size_y = prop_atlas_size_y->GetUint();
      const auto layers = prop_atlas_layers->GetUint();
      const auto mipmap = 1u;

      const auto raw = Raw(size_x * size_y * layers, glm::zero<glm::f32vec4>());

      lightmaps_final = core->GetDevice()->CreateResource("lightmaps_final",
        Resource::Tex2DDesc
        {
          Usage(USAGE_UNORDERED_ACCESS | USAGE_SHADER_RESOURCE),
          mipmap,
          layers,
          format,
          size_x,
          size_y,
        },
        Resource::Hint(Resource::HINT_LAYERED_IMAGE),
        raw.GetBytes());
    }

    void Scope::DestroyScreenData()
    {
      core->GetDevice()->DestroyResource(screen_data);
      screen_data.reset();
    }

    void Scope::DestroySceneBufferInst()
    {
      core->GetDevice()->DestroyResource(scene_buffer_inst);
      scene_buffer_inst.reset();
    }

    void Scope::DestroySceneBufferTrng()
    {
      core->GetDevice()->DestroyResource(scene_buffer_trng);
      scene_buffer_trng.reset();
    }

    void Scope::DestroySceneBufferVert()
    {
      core->GetDevice()->DestroyResource(scene_buffer_vert);
      scene_buffer_vert.reset();
    }

    void Scope::DestroyTraceBufferTBox()
    {
      core->GetDevice()->DestroyResource(trace_buffer_tbox);
      trace_buffer_tbox.reset();
    }

    void Scope::DestroyTraceBufferBBox()
    {
      core->GetDevice()->DestroyResource(trace_buffer_bbox);
      trace_buffer_bbox.reset();
    }

    void Scope::DestroyTraceBufferInst()
    {
      core->GetDevice()->DestroyResource(trace_buffer_inst);
      trace_buffer_inst.reset();
    }

    void Scope::DestroyTraceBufferTrng()
    {
      core->GetDevice()->DestroyResource(trace_buffer_trng);
      trace_buffer_trng.reset();
    }

    void Scope::DestroyTraceBufferVert()
    {
      core->GetDevice()->DestroyResource(trace_buffer_vert);
      trace_buffer_vert.reset();
    }

    void Scope::DestroySceneArrayAAAM()
    {
      core->GetDevice()->DestroyResource(scene_array_aaam);
      scene_array_aaam.reset();
    }

    void Scope::DestroySceneArraySNNO()
    {
      core->GetDevice()->DestroyResource(scene_array_snno);
      scene_array_snno.reset();
    }

    void Scope::DestroySceneArrayEEET()
    {
      core->GetDevice()->DestroyResource(scene_array_eeet);
      scene_array_eeet.reset();
    }

    void Scope::DestroyLightmapsInput()
    {
      core->GetDevice()->DestroyResource(lightmaps_input);
      lightmaps_input.reset();
    }

    void Scope::DestroyLightmapsAccum()
    {
      core->GetDevice()->DestroyResource(lightmaps_accum);
      lightmaps_accum.reset();
    }

    void Scope::DestroyLightmapsFinal()
    {
      core->GetDevice()->DestroyResource(lightmaps_final);
      lightmaps_final.reset();
    }

    Scope::Scope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util)
      : core(core)
      , util(util)
    {
      prop_scene = util->GetStorage()->GetTree()->GetObjectItem("scene");
      {
        prop_inst = prop_scene->GetObjectItem("buffer_inst");
        prop_trng = prop_scene->GetObjectItem("buffer_trng");
        prop_vert = prop_scene->GetObjectItem("buffer_vert");
      }

      prop_camera = util->GetStorage()->GetTree()->GetObjectItem("camera");
      {
        prop_counter = prop_camera->GetObjectItem("counter");
      }

      prop_lighting = util->GetStorage()->GetTree()->GetObjectItem("lighting");

      prop_illumination = util->GetStorage()->GetTree()->GetObjectItem("illumination");
      {
        prop_atlas_size_x = prop_illumination->GetObjectItem("atlas_size_x");
        prop_atlas_size_y = prop_illumination->GetObjectItem("atlas_size_y");
        prop_atlas_layers = prop_illumination->GetObjectItem("atlas_layers");
      }

      CreateScreenData();

      CreateLightmapsInput();
      CreateLightmapsAccum();
      CreateLightmapsFinal();

      CreateSceneBufferInst();
      CreateSceneBufferTrng();
      CreateSceneBufferVert();

      CreateTraceBufferTBox();
      CreateTraceBufferBBox();

      CreateTraceBufferInst();
      CreateTraceBufferTrng();
      CreateTraceBufferVert();

      CreateSceneArrayAAAM();
      CreateSceneArraySNNO();
      CreateSceneArrayEEET();
    }

    Scope::~Scope()
    {
      //DestroySceneTextures0();
      //DestroySceneTextures1();
      //DestroySceneTextures2();
      //DestroySceneTextures3();
      //DestroySceneTextures4();
      //DestroySceneTextures5();
      //DestroySceneTextures6();
      //DestroySceneTextures7();

      //DestroyTraceInstances();
      //DestroyTraceTriangles();
      //DestroyTraceVertices();

      //DestroyTraceTBoxes();
      //DestroyTraceBBoxes();

      //DestroySceneInstances();
      //DestroySceneTriangles();
      //DestroySceneVertices();

      //DestroyLightmapsInput();
      //DestroyLightmapsAccum();
      //DestroyLightmapsFinal();

      DestroyScreenData();
    }
  }
}
