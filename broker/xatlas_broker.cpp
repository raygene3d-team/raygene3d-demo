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
  //void UpdateWrap()
  //{
  //  auto atlas = xatlas::Create();

  //  for (uint32_t i = 0; i < uint32_t(instances_array.size()); ++i)
  //  {
  //    const auto& vertices = vertices_arrays[i];
  //    const auto vertex_data = reinterpret_cast<const uint8_t*>(vertices.data());
  //    const auto vertex_count = uint32_t(vertices.size());
  //    const auto vertex_stride = uint32_t(sizeof(Vertex));

  //    const auto& triangles = triangles_arrays[i];
  //    const auto index_data = reinterpret_cast<const uint8_t*>(triangles.data());
  //    const auto index_count = uint32_t(triangles.size()) * 3;
  //    const auto index_format = xatlas::IndexFormat::UInt32;

  //    xatlas::MeshDecl mesh_decl;
  //    mesh_decl.vertexCount = vertex_count;
  //    mesh_decl.vertexPositionData = vertex_data + 0;
  //    mesh_decl.vertexPositionStride = vertex_stride;
  //    mesh_decl.vertexNormalData = vertex_data + 16;
  //    mesh_decl.vertexNormalStride = vertex_stride;
  //    mesh_decl.vertexUvData = vertex_data + 48;
  //    mesh_decl.vertexUvStride = vertex_stride;
  //    mesh_decl.indexCount = index_count;
  //    mesh_decl.indexData = index_data;
  //    mesh_decl.indexFormat = index_format;
  //    BLAST_ASSERT(xatlas::AddMeshError::Success == xatlas::AddMesh(atlas, mesh_decl));

  //    //xatlas::UvMeshDecl uv_mesh_decl;
  //    //uv_mesh_decl.vertexCount = vertex_count;
  //    //uv_mesh_decl.vertexStride = vertex_stride;
  //    //uv_mesh_decl.vertexUvData = vertex_data + 48;
  //    //uv_mesh_decl.indexCount = index_count;
  //    //uv_mesh_decl.indexData = index_data;
  //    //uv_mesh_decl.indexFormat = index_format;
  //    //uv_mesh_decl.indexOffset = 0;
  //    //BLAST_ASSERT(xatlas::AddMeshError::Success == xatlas::AddUvMesh(atlas, uv_mesh_decl));
  //  }

  //  xatlas::ChartOptions chartOptions;
  //  chartOptions.useInputMeshUvs = true;
  //  xatlas::ComputeCharts(atlas, chartOptions);

  //  xatlas::PackOptions packOptions;
  //  packOptions.resolution = 2048;
  //  packOptions.texelsPerUnit = 200.0;
  //  xatlas::PackCharts(atlas, packOptions);

  //  BLAST_ASSERT(atlas->meshCount == uint32_t(instances_array.size()));

  //  for (uint32_t i = 0; i < atlas->meshCount; i++)
  //  {
  //    const auto& triangles = triangles_arrays[i];
  //    const auto& mesh = atlas->meshes[i];
  //    BLAST_ASSERT(mesh.indexCount == 3 * uint32_t(triangles.size()))

  //      const auto& vertices = vertices_arrays[i];

  //    std::vector<Vertex> temp_vertices(mesh.vertexCount);
  //    for (uint32_t j = 0; j < mesh.vertexCount; j++)
  //    {
  //      const auto& vertex = mesh.vertexArray[j];

  //      temp_vertices[j] = vertices[vertex.xref];
  //      temp_vertices[j].msk = vertex.atlasIndex;
  //      temp_vertices[j].tc1 = { vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height };
  //    }

  //    std::vector<Triangle> temp_triangles(mesh.indexCount / 3);
  //    for (uint32_t j = 0; j < mesh.indexCount / 3; j++)
  //    {
  //      temp_triangles[j].idx[0] = mesh.indexArray[j * 3 + 0];
  //      temp_triangles[j].idx[1] = mesh.indexArray[j * 3 + 1];
  //      temp_triangles[j].idx[2] = mesh.indexArray[j * 3 + 2];
  //    }

  //    std::swap(vertices_arrays[i], temp_vertices);
  //    std::swap(triangles_arrays[i], temp_triangles);

  //    instances_array[i].vert_count = uint32_t(vertices_arrays[i].size());
  //    instances_array[i].vert_offset = i > 0 ? instances_array[i - 1].vert_offset + instances_array[i - 1].vert_count : 0;
  //    instances_array[i].prim_count = uint32_t(triangles_arrays[i].size());
  //    instances_array[i].prim_offset = i > 0 ? instances_array[i - 1].prim_offset + instances_array[i - 1].prim_count : 0;
  //  }

  //  if (atlas->width > 0 && atlas->height > 0)
  //  {
  //    const auto white = glm::u8vec4(255, 255, 255, 255);

  //    const auto triangle_raster_fn = [](glm::u8vec4* pixels, uint32_t size_x, uint32_t size_y, const float* v0, const float* v1, const float* v2, const glm::u8vec4& color)
  //    {
  //      const auto tc0 = glm::f32vec2(v0[0], v0[1]);
  //      const auto tc1 = glm::f32vec2(v1[0], v1[1]);
  //      const auto tc2 = glm::f32vec2(v2[0], v2[1]);

  //      const auto min = glm::min(glm::min(tc0, tc1), tc2);
  //      const auto max = glm::max(glm::max(tc0, tc1), tc2);

  //      const auto denom = glm::determinant(glm::f32mat2x2(tc1 - tc0, tc2 - tc0));

  //      for (uint32_t i = 0; i < size_y; ++i)
  //      {
  //        if (i < min.y || i > max.y) continue;

  //        for (uint32_t j = 0; j < size_x; ++j)
  //        {
  //          if (j < min.x || j > max.x) continue;

  //          const auto pnt = glm::f32vec2(j + 0.0f, i + 0.0f);

  //          const auto v = glm::determinant(glm::f32mat2x2(pnt - tc0, tc2 - tc0)) / denom;
  //          if (v < 0.0f || v > 1.0f) continue;

  //          const auto w = glm::determinant(glm::f32mat2x2(tc1 - tc0, pnt - tc0)) / denom;
  //          if (w < 0.0f || w > 1.0f) continue;

  //          const auto u = 1.0f - v - w;
  //          if (u < 0.0f || u > 1.0f) continue;

  //          pixels[(size_x * i + j)] = color;

  //          const auto pattern = glm::floor(pnt / 16.0f);
  //          const auto fading = 0.5f * glm::fract(0.5f * (pattern.x + pattern.y)) + 0.5f;

  //          pixels[(size_x * i + j)].x *= fading;
  //          pixels[(size_x * i + j)].y *= fading;
  //          pixels[(size_x * i + j)].z *= fading;
  //        }
  //      }
  //    };

  //    textures_4.resize(atlas->atlasCount);

  //    for (uint32_t z = 0; z < atlas->atlasCount; ++z)
  //    {
  //      //std::vector<uint8_t> image(atlas->width* atlas->height * 3);

  //      Texture texture;
  //      texture.texels.resize(atlas->width * atlas->height);
  //      texture.extent_x = atlas->width;
  //      texture.extent_y = atlas->height;

  //      for (uint32_t i = 0; i < atlas->meshCount; ++i)
  //      {
  //        const auto& mesh = atlas->meshes[i];

  //        for (uint32_t j = 0; j < mesh.chartCount; ++j)
  //        {
  //          const auto& chart = mesh.chartArray[j];
  //          if (chart.atlasIndex != z) continue;

  //          BLAST_ASSERT(chart.atlasIndex != -1);

  //          const auto color = glm::u8vec4(uint8_t((rand() % 255 + 192) * 0.5f), uint8_t((rand() % 255 + 192) * 0.5f), uint8_t((rand() % 255 + 192) * 0.5f), 255);

  //          for (uint32_t k = 0; k < chart.faceCount; ++k)
  //          {
  //            const auto face = chart.faceArray[k];

  //            const auto& vtx0 = mesh.vertexArray[mesh.indexArray[3 * face + 0]];
  //            const auto& vtx1 = mesh.vertexArray[mesh.indexArray[3 * face + 1]];
  //            const auto& vtx2 = mesh.vertexArray[mesh.indexArray[3 * face + 2]];


  //            const int v0[2] = { int(vtx0.uv[0]), int(vtx0.uv[1]) };
  //            const int v1[2] = { int(vtx1.uv[0]), int(vtx1.uv[1]) };
  //            const int v2[2] = { int(vtx2.uv[0]), int(vtx2.uv[1]) };

  //            triangle_raster_fn(texture.texels.data(), texture.extent_x, texture.extent_y, vtx0.uv, vtx1.uv, vtx2.uv, color);
  //          }
  //        }
  //      }

  //      //char filename[256];
  //      //snprintf(filename, sizeof(filename), "example_tris%02u.png", z);
  //      //printf("Writing '%s'...\n", filename);
  //      //stbi_write_png(filename, texture.extent_x, texture.extent_y, 4, texture.texels.data(), 4 * texture.extent_x);

  //      textures_4[z] = std::move(texture);
  //    }
  //  }

  //  xatlas::Destroy(atlas);
  //}

  void XAtlasBroker::Initialize()
  {
    const auto tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_scene = tree->GetObjectItem("scene_property");
    {
      prop_instances = prop_scene->GetObjectItem("instances");
      prop_triangles = prop_scene->GetObjectItem("triangles");
      prop_vertices = prop_scene->GetObjectItem("vertices");
    }
  }

  void XAtlasBroker::Use()
  {
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
      mesh_decl.vertexPositionData = vrt_items + 0u;
      mesh_decl.vertexPositionStride = uint32_t(sizeof(Vertex));
      mesh_decl.vertexNormalData = vrt_items + 16u;
      mesh_decl.vertexNormalStride = uint32_t(sizeof(Vertex));
      mesh_decl.vertexUvData = vrt_items + 48u;
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
    packOptions.resolution = 2048;
    packOptions.texelsPerUnit = 200.0;
    xatlas::PackCharts(atlas, packOptions);

    BLAST_ASSERT(atlas->meshCount == ins_count);

    auto updated_vrt_count = 0u;
    auto updated_trg_count = 0u;
    for (uint32_t i = 0; i < atlas->meshCount; i++)
    {
      const auto& mesh = atlas->meshes[i];

      updated_vrt_count += mesh.vertexCount;
      updated_trg_count += mesh.indexCount * 3;
    }

    const auto updated_prop_vrt = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
    updated_prop_vrt->RawAllocate(updated_vrt_count * uint32_t(sizeof(Vertex)));

    const auto updated_prop_trg = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
    updated_prop_trg->RawAllocate(updated_trg_count * uint32_t(sizeof(Triangle)));

    const auto updated_prop_ins = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
    updated_prop_trg->RawAllocate(updated_trg_count * uint32_t(sizeof(Instance)));

    auto updated_vrt_offset = 0u;
    auto updated_trg_offset = 0u;
    for (uint32_t i = 0; i < atlas->meshCount; i++)
    {
      const auto& mesh = atlas->meshes[i];

      for (uint32_t j = 0; j < mesh.vertexCount; j++)
      {
        const auto& vertex = mesh.vertexArray[j];

        Vertex updated_vrt = vrt_array[ins_array[i].vert_offset + mesh.vertexArray[j].xref];
        updated_vrt.msk = vertex.atlasIndex;
        updated_vrt.tc1 = { vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height };
        updated_prop_vrt->SetTypedBytes<Vertex>({ &updated_vrt, 1}, updated_vrt_offset + j);
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

      Instance updated_ins = ins_array[i];
      updated_ins.vert_count = mesh.vertexCount;
      updated_ins.vert_offset = updated_vrt_offset;
      updated_ins.prim_count = mesh.indexCount / 3;
      updated_ins.prim_offset = updated_trg_offset;
      updated_prop_ins->SetTypedBytes<Instance>({ &updated_ins, 1 }, i);

      updated_vrt_offset += mesh.vertexCount;
      updated_trg_offset += mesh.indexCount / 3;
    }

    prop_instances = updated_prop_ins;
    prop_triangles = updated_prop_trg;
    prop_vertices = updated_prop_vrt;

    xatlas::Destroy(atlas);
  }

  void XAtlasBroker::Discard()
  {
    prop_scene->SetObjectItem("instances", prop_instances);
    prop_scene->SetObjectItem("triangles", prop_triangles);
    prop_scene->SetObjectItem("vertices", prop_vertices);

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