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
#include "xatlas_broker.h"

#include <xatlas/xatlas.h>


namespace RayGene3D
{
  void XAtlasBroker::Initialize()
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
      prop_maps = prop_illumination->GetObjectItem("maps");
      prop_quality = prop_illumination->GetObjectItem("quality");
      prop_density = prop_illumination->GetObjectItem("density");
    }
  }

  void XAtlasBroker::Use()
  {
    //if (prop_maps->GetBool() == false) return;

    const auto [ins_array, ins_count] = prop_instances->GetTypedBytes<Instance>(0);
    const auto [trg_array, trg_count] = prop_triangles->GetTypedBytes<Triangle>(0);
    const auto [vrt_array, vrt_count] = prop_vertices->GetTypedBytes<Vertex>(0);

    auto atlas = xatlas::Create();

    for (uint32_t i = 0; i < ins_count; ++i)
    {
      const auto trg_offset = ins_array[i].prim_offset;
      const auto trg_items = trg_array + trg_offset;
      const auto trg_count = ins_array[i].prim_count;

      const auto vrt_offset = ins_array[i].vert_offset;
      const auto vrt_items = vrt_array + vrt_offset;
      const auto vrt_count = ins_array[i].vert_count;

      xatlas::MeshDecl mesh_decl;
      mesh_decl.vertexCount = vrt_count;
      mesh_decl.vertexPositionData = reinterpret_cast<const uint8_t*>(vrt_items) + 0u;
      mesh_decl.vertexPositionStride = uint32_t(sizeof(Vertex));
      mesh_decl.vertexNormalData = reinterpret_cast<const uint8_t*>(vrt_items) + 16u;
      mesh_decl.vertexNormalStride = uint32_t(sizeof(Vertex));
      mesh_decl.vertexUvData = reinterpret_cast<const uint8_t*>(vrt_items) + 48u;
      mesh_decl.vertexUvStride = uint32_t(sizeof(Vertex));
      mesh_decl.indexCount = trg_count * 3u;
      mesh_decl.indexData = trg_items;
      mesh_decl.indexFormat = xatlas::IndexFormat::UInt32;
      BLAST_ASSERT(xatlas::AddMeshError::Success == xatlas::AddMesh(atlas, mesh_decl));

      //xatlas::UvMeshDecl uv_mesh_decl;
      //uv_mesh_decl.vertexCount = vertex_count;
      //uv_mesh_decl.vertexStride = vertex_stride;
      //uv_mesh_decl.vertexUvData = vertex_data + 48;
      //uv_mesh_decl.indexCount = index_count;
      //uv_mesh_decl.indexData = index_data;
      //uv_mesh_decl.indexFormat = index_format;
      //uv_mesh_decl.indexOffset = 0;
      //BLAST_ASSERT(xatlas::AddMeshError::Success == xatlas::AddUvMesh(atlas, uv_mesh_decl));
    }

    xatlas::ChartOptions chartOptions;
    chartOptions.useInputMeshUvs = true;
    xatlas::ComputeCharts(atlas, chartOptions);

    xatlas::PackOptions packOptions;
    packOptions.resolution = prop_quality->GetUint();
    packOptions.texelsPerUnit = prop_density->GetReal();
    xatlas::PackCharts(atlas, packOptions);

    BLAST_ASSERT(atlas->meshCount == ins_count);

    auto updated_vrt_count = 0u;
    auto updated_trg_count = 0u;
    auto updated_ins_count = 0u;
    for (uint32_t i = 0; i < atlas->meshCount; i++)
    {
      const auto& mesh = atlas->meshes[i];

      updated_vrt_count += mesh.vertexCount;
      updated_trg_count += mesh.indexCount / 3;
      updated_ins_count += 1u;
    }

    const auto updated_prop_vrt = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
    updated_prop_vrt->RawAllocate(updated_vrt_count * uint32_t(sizeof(Vertex)));

    const auto updated_prop_trg = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
    updated_prop_trg->RawAllocate(updated_trg_count * uint32_t(sizeof(Triangle)));

    const auto updated_prop_ins = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
    updated_prop_ins->RawAllocate(updated_ins_count * uint32_t(sizeof(Instance)));

    auto updated_vrt_offset = 0u;
    auto updated_trg_offset = 0u;
    for (uint32_t i = 0; i < atlas->meshCount; ++i)
    {
      const auto& mesh = atlas->meshes[i];

      for (uint32_t j = 0; j < mesh.vertexCount; j++)
      {
        const auto& vertex = mesh.vertexArray[j];

        Vertex updated_vrt = vrt_array[ins_array[i].vert_offset + mesh.vertexArray[j].xref];
        updated_vrt.msk = vertex.atlasIndex;
        updated_vrt.tc1 = { vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height };
        updated_prop_vrt->SetTypedBytes<Vertex>({ &updated_vrt, 1 }, updated_vrt_offset + j);
      }

      for (uint32_t j = 0; j < mesh.indexCount / 3; j++)
      {
        const auto idx0 = mesh.indexArray[j * 3 + 0];
        const auto idx1 = mesh.indexArray[j * 3 + 1];
        const auto idx2 = mesh.indexArray[j * 3 + 2];

        Triangle updated_trg = trg_array[ins_array[i].prim_offset + j];
        updated_trg.idx = { idx0, idx1, idx2 };
        updated_prop_trg->SetTypedBytes<Triangle>({ &updated_trg, 1 }, updated_trg_offset + j);
      }

      for (uint32_t j = 0; j < mesh.chartCount; ++j)
      {
        const auto& chart = mesh.chartArray[j];

        const auto color = glm::u8vec4
        {
          rand() % 64 + 63,
          rand() % 64 + 63,
          rand() % 64 + 63,
          255
        };

        for (uint32_t k = 0; k < chart.faceCount; ++k)
        {
          const auto face = chart.faceArray[k];

          const auto idx_0 = mesh.indexArray[3 * face + 0];
          auto vtx_0 = *updated_prop_vrt->GetTypedBytes<Vertex>(updated_vrt_offset + idx_0).first;
          vtx_0.col = color;
          updated_prop_vrt->SetTypedBytes<Vertex>({ &vtx_0, 1 }, updated_vrt_offset + idx_0);

          const auto idx_1 = mesh.indexArray[3 * face + 1];
          auto vtx_1 = *updated_prop_vrt->GetTypedBytes<Vertex>(updated_vrt_offset + idx_1).first;
          vtx_1.col = color;
          updated_prop_vrt->SetTypedBytes<Vertex>({ &vtx_1, 1 }, updated_vrt_offset + idx_1);

          const auto idx_2 = mesh.indexArray[3 * face + 2];
          auto vtx_2 = *updated_prop_vrt->GetTypedBytes<Vertex>(updated_vrt_offset + idx_2).first;
          vtx_2.col = color;
          updated_prop_vrt->SetTypedBytes<Vertex>({ &vtx_2, 1 }, updated_vrt_offset + idx_2);
        }
      }



      Instance updated_ins = ins_array[i];
      updated_ins.vert_count = mesh.vertexCount;
      updated_ins.vert_offset = updated_vrt_offset;
      updated_ins.prim_count = mesh.indexCount / 3;
      updated_ins.prim_offset = updated_trg_offset;
      updated_prop_ins->SetTypedBytes<Instance>({ &updated_ins, 1 }, i);

      updated_vrt_offset += mesh.vertexCount;
      updated_trg_offset += mesh.indexCount / 3;
    }

    prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->SetArrayItem(0, updated_prop_ins);
    prop_scene->GetObjectItem("instances")->GetObjectItem("count")->SetUint(updated_ins_count);

    prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->SetArrayItem(0, updated_prop_trg);
    prop_scene->GetObjectItem("triangles")->GetObjectItem("count")->SetUint(updated_trg_count);

    prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->SetArrayItem(0, updated_prop_vrt);
    prop_scene->GetObjectItem("vertices")->GetObjectItem("count")->SetUint(updated_vrt_count);

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


    extent_x = atlas->width;
    extent_y = atlas->height;
    layers = atlas->atlasCount;

    auto raws = std::vector<Raw>(layers);
    for (auto& raw : raws)
    {
      //raw.Allocate(extent_x* extent_y* uint32_t(sizeof(glm::u32vec4)));
      raw.Allocate(extent_x * extent_y * uint32_t(sizeof(glm::u8vec4)));

      for (auto i = 0u; i < extent_x * extent_y; ++i)
      {
        //raw.SetElement(glm::zero<glm::u32vec4>(), i);
        raw.SetElement(glm::zero<glm::u8vec4>(), i);
      }
    }

    for (uint32_t i = 0; i < atlas->meshCount; ++i)
    {
      const auto& mesh = atlas->meshes[i];

      for (uint32_t j = 0; j < mesh.chartCount; ++j)
      {
        const auto& chart = mesh.chartArray[j];

        const auto color = glm::u8vec4
        {
          rand() % 64 + 63,
          rand() % 64 + 63,
          rand() % 64 + 63,
          255
        };

        for (uint32_t k = 0; k < chart.faceCount; ++k)
        {
          const auto face = chart.faceArray[k];

          auto& vtx0 = mesh.vertexArray[mesh.indexArray[3 * face + 0]];
          auto& vtx1 = mesh.vertexArray[mesh.indexArray[3 * face + 1]];
          auto& vtx2 = mesh.vertexArray[mesh.indexArray[3 * face + 2]];

          const auto p0 = glm::f32vec2(vtx0.uv[0], vtx0.uv[1]);
          const auto p1 = glm::f32vec2(vtx1.uv[0], vtx1.uv[1]);
          const auto p2 = glm::f32vec2(vtx2.uv[0], vtx2.uv[1]);

          const auto bb_min = glm::ivec2(glm::floor(glm::min(p0, glm::min(p1, p2))));
          const auto bb_max = glm::ivec2(glm::ceil(glm::max(p0, glm::max(p1, p2))));

          for (auto n = bb_min.y; n < bb_max.y; ++n)
          {
            for (auto m = bb_min.x; m < bb_max.x; ++m)
            {
              const auto barycentric_00 = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(0.0f, 0.0f), p0, p1, p2);
              const auto covered_00 = glm::all(glm::greaterThanEqual(barycentric_00, glm::zero<glm::f32vec3>()))
                && glm::all(glm::lessThanEqual(barycentric_00, glm::one<glm::f32vec3>()));

              const auto barycentric_01 = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(0.0f, 1.0f), p0, p1, p2);
              const auto covered_01 = glm::all(glm::greaterThanEqual(barycentric_01, glm::zero<glm::f32vec3>()))
                && glm::all(glm::lessThanEqual(barycentric_01, glm::one<glm::f32vec3>()));

              const auto barycentric_10 = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(1.0f, 0.0f), p0, p1, p2);
              const auto covered_10 = glm::all(glm::greaterThanEqual(barycentric_10, glm::zero<glm::f32vec3>()))
                && glm::all(glm::lessThanEqual(barycentric_10, glm::one<glm::f32vec3>()));

              const auto barycentric_11 = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(1.0f, 1.0f), p0, p1, p2);
              const auto covered_11 = glm::all(glm::greaterThanEqual(barycentric_11, glm::zero<glm::f32vec3>()))
                && glm::all(glm::lessThanEqual(barycentric_11, glm::one<glm::f32vec3>()));

              if (!covered_00 && !covered_01 && !covered_10 && !covered_11) continue;

              const auto barycentric = eval_barycentric_fn(glm::f32vec2(m, n) + glm::f32vec2(0.5f, 0.5f), p0, p1, p2);

              const auto u = *reinterpret_cast<const uint32_t*>(&barycentric[0]);
              const auto v = *reinterpret_cast<const uint32_t*>(&barycentric[1]);

              const auto value = glm::u32vec4{ i, face, u, v };
              const auto index = n * extent_x + m;

              //raws[chart.atlasIndex].SetElement<glm::u32vec4>(value, index);
              raws[chart.atlasIndex].SetElement<glm::u8vec4>(color, index);
            }
          }
        }
      }
    }

    xatlas::Destroy(atlas);

    const auto blueprint_prop = CreateTextureProperty({ raws.data(), uint32_t(raws.size()) }, extent_x, extent_y, 1u, layers);
    prop_scene->SetObjectItem("blueprint", blueprint_prop);
  }

  void XAtlasBroker::Discard()
  {
    prop_vertices.reset();
    prop_triangles.reset();
    prop_instances.reset();

    prop_scene.reset();
  }

  XAtlasBroker::XAtlasBroker(Wrap& wrap)
    : Broker("xatlas_broker", wrap)
  {
  }

  XAtlasBroker::~XAtlasBroker()
  {
  }
}