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
  }

  void XAtlasBroker::Use()
  {
    //if (prop_maps->GetBool() == false) return;

    const auto [ins_array, ins_count] = prop_instances->GetTypedBytes<Instance>(0);
    const auto [trg_array, trg_count] = prop_triangles->GetTypedBytes<Triangle>(0);
    const auto [vrt_array, vrt_count] = prop_vertices->GetTypedBytes<Vertex>(0);

    const auto atlas = xatlas::Create();

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

        auto updated_vrt = vrt_array[ins_array[i].vert_offset + mesh.vertexArray[j].xref];
        updated_vrt.msk = vertex.atlasIndex;
        updated_vrt.tc1 = { vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height };
        updated_prop_vrt->SetTypedBytes<Vertex>({ &updated_vrt, 1 }, updated_vrt_offset + j);
      }

      for (uint32_t j = 0; j < mesh.indexCount / 3; j++)
      {
        const auto idx0 = mesh.indexArray[j * 3 + 0];
        const auto idx1 = mesh.indexArray[j * 3 + 1];
        const auto idx2 = mesh.indexArray[j * 3 + 2];

        auto updated_trg = trg_array[ins_array[i].prim_offset + j];
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

      auto updated_ins = ins_array[i];
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

    prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);
    prop_triangles = prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->GetArrayItem(0);
    prop_vertices = prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->GetArrayItem(0);

    prop_atlas_size_x->SetUint(atlas->width);
    prop_atlas_size_y->SetUint(atlas->height);
    prop_atlas_layers->SetUint(atlas->atlasCount);

    xatlas::Destroy(atlas);

    prop_illumination->SetObjectItem("atlas_size_x", prop_atlas_size_x);
    prop_illumination->SetObjectItem("atlas_size_y", prop_atlas_size_y);
    prop_illumination->SetObjectItem("atlas_layers", prop_atlas_layers);
  }

  void XAtlasBroker::Discard()
  {
  }

  XAtlasBroker::XAtlasBroker(Wrap& wrap)
    : Broker("xatlas_broker", wrap)
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

    prop_atlas_size_x = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
    prop_atlas_size_y = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
    prop_atlas_layers = std::shared_ptr<RayGene3D::Property>(new RayGene3D::Property(RayGene3D::Property::TYPE_UINT));
  }

  XAtlasBroker::~XAtlasBroker()
  {
    prop_vertices.reset();
    prop_triangles.reset();
    prop_instances.reset();

    prop_scene.reset();

    prop_maps.reset();
    prop_quality.reset();
    prop_density.reset();

    prop_illumination.reset();
  }
}