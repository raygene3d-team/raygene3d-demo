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
#pragma once

#include "import_broker.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#define OBJ_TRIANGULATE

#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE
//#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tinygltf/tiny_gltf.h>

#define TINYEXR_IMPLEMENTATION
#include <tinyexr/tinyexr.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_image_resize.h>

#include <mikktspace/mikktspace.h>

#include <xatlas/xatlas.h>


namespace RayGene3D
{
  std::pair<std::vector<Vertex>, std::vector<Triangle>> PopulateInstance(uint32_t count, uint32_t align, float scale, bool flip,
    std::pair<const uint8_t*, uint32_t> pos_data, uint32_t pos_stride, std::pair<const uint8_t*, uint32_t> pos_idx_data, uint32_t pos_id_stride,
    std::pair<const uint8_t*, uint32_t> nrm_data, uint32_t nrm_stride, std::pair<const uint8_t*, uint32_t> nrm_idx_data, uint32_t nrm_id_stride,
    std::pair<const uint8_t*, uint32_t> tc0_data, uint32_t tc0_stride, std::pair<const uint8_t*, uint32_t> tc0_idx_data, uint32_t tc0_id_stride,
    uint32_t& degenerated_geom_tris_count, uint32_t degenerated_wrap_tris_count)
  {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    for (uint32_t k = 0; k < count; ++k)
    {
      const auto create_vertex_fn = [align, scale, flip,
        pos_data, pos_stride, pos_idx_data, pos_id_stride,
        nrm_data, nrm_stride, nrm_idx_data, nrm_id_stride,
        tc0_data, tc0_stride, tc0_idx_data, tc0_id_stride]
        (uint32_t index)
      {
        const auto pos_idx = align == 4 ? *(reinterpret_cast<const uint32_t*>(pos_idx_data.first + pos_id_stride * index))
                           : align == 2 ? *(reinterpret_cast<const uint16_t*>(pos_idx_data.first + pos_id_stride * index))
                           : align == 1 ? *(reinterpret_cast<const uint8_t *>(pos_idx_data.first + pos_id_stride * index))
                           : -1;
        const auto pos_ptr = reinterpret_cast<const float*>(pos_data.first + pos_stride * pos_idx);
        const auto& pos = glm::fvec3(pos_ptr[0], pos_ptr[1], pos_ptr[2]);

        const auto nrm_idx = align == 4 ? *(reinterpret_cast<const uint32_t*>(nrm_idx_data.first + nrm_id_stride * index))
                           : align == 2 ? *(reinterpret_cast<const uint16_t*>(nrm_idx_data.first + nrm_id_stride * index))
                           : align == 1 ? *(reinterpret_cast<const uint8_t *>(nrm_idx_data.first + nrm_id_stride * index))
                           : -1;
        const auto nrm_ptr = reinterpret_cast<const float*>(nrm_data.first + nrm_stride * nrm_idx);
        const auto& nrm = glm::fvec3(nrm_ptr[0], nrm_ptr[1], nrm_ptr[2]);
        
        const auto tc0_idx = align == 4 ? *(reinterpret_cast<const uint32_t*>(tc0_idx_data.first + tc0_id_stride * index))
                           : align == 2 ? *(reinterpret_cast<const uint16_t*>(tc0_idx_data.first + tc0_id_stride * index))
                           : align == 1 ? *(reinterpret_cast<const uint8_t *>(tc0_idx_data.first + tc0_id_stride * index))
                           : -1;
        const auto tc0_ptr = reinterpret_cast<const float*>(tc0_data.first + tc0_stride * tc0_idx);
        const auto& tc0 = glm::f32vec2(tc0_ptr[0], tc0_ptr[1]);

        Vertex vertex;

        vertex.pos = scale * (flip ? glm::fvec3{ pos.x,-pos.z,-pos.y } : glm::fvec3{ pos.x, pos.y,-pos.z });
        vertex.nrm = glm::normalize(flip ? glm::fvec3{ nrm.x,-nrm.z,-nrm.y } : glm::fvec3{ nrm.x, nrm.y,-nrm.z });
        vertex.tc0 = glm::f32vec2(tc0.x, tc0.y);

        return vertex;
      };

      const auto vtx0 = create_vertex_fn(k * 3 + 0);
      const auto vtx1 = create_vertex_fn(k * 3 + 1);
      const auto vtx2 = create_vertex_fn(k * 3 + 2);

      const auto dp_10 = vtx1.pos - vtx0.pos;
      const auto dp_20 = vtx2.pos - vtx0.pos;
      const auto ng = glm::cross(dp_10, dp_20);
      if (glm::length(ng) == 0.0f)
      {
        ++degenerated_geom_tris_count;
        continue;
      }

      auto has_textures = false;
      if (has_textures)
      {
        const auto duv_10 = vtx1.tc0 - vtx0.tc0;
        const auto duv_20 = vtx2.tc0 - vtx0.tc0;
        const auto det = glm::determinant(glm::fmat2x2(duv_10, duv_20));
        if (det == 0.0f)
        {
          ++degenerated_wrap_tris_count;
          continue;
        }
      }

      const auto hash_vertex_fn = [](const Vertex& vertex)
      {
        const auto uref = reinterpret_cast<const glm::u32vec4*>(&vertex);

        auto hash = 0u;
        hash ^= std::hash<glm::u32vec4>()(uref[0]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<glm::u32vec4>()(uref[1]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<glm::u32vec4>()(uref[2]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<glm::u32vec4>()(uref[3]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        return hash;
      };
      auto compare_vertex_fn = [](const Vertex& l, const Vertex& r)
      {
        return (memcmp(&l, &r, sizeof(Vertex)) == 0);
      };
      std::unordered_map<Vertex, uint32_t, decltype(hash_vertex_fn), decltype(compare_vertex_fn)> vertex_map(8, hash_vertex_fn, compare_vertex_fn);

      const auto remap_vertex_fn = [&vertex_map](std::vector<Vertex>& vertices, const Vertex& vertex)
      {
        const auto result = vertex_map.emplace(vertex, uint32_t(vertices.size()));
        if (result.second)
        {
          vertices.push_back(vertex);
        }
        return result.first->second;
      };

      Triangle triangle;

      triangle.idx[0] = remap_vertex_fn(vertices, vtx0);
      triangle.idx[1] = remap_vertex_fn(vertices, vtx1);
      triangle.idx[2] = remap_vertex_fn(vertices, vtx0);

      triangles.push_back(triangle);
    }

    return { vertices, triangles };
  }


  void UpdateTangents(std::vector<Instance>& instances, std::vector<std::vector<Triangle>>& triangles, std::vector<std::vector<Vertex>>& vertices)
  {

    for (uint32_t i = 0; i < uint32_t(instances.size()); ++i)
    {
      auto prim_data = triangles[i].data();
      const auto prim_count = uint32_t(triangles[i].size());

      auto vert_data = vertices[i].data();
      const auto vert_count = uint32_t(vertices.size());

      struct SMikkTSpaceUserData
      {
        std::pair<Triangle*, uint32_t> prims;
        std::pair<Vertex*, uint32_t> verts;
      };
      SMikkTSpaceUserData data { {prim_data, prim_count}, { vert_data, vert_count } };

      SMikkTSpaceInterface input = { 0 };
      input.m_getNumFaces = [](const SMikkTSpaceContext* ctx)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        return int32_t(data->prims.second);
      };

      input.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* ctx, const int iFace)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        return 3;
      };

      //input.m_getPosition = &GetPositionCb;
      //input.m_getNormal = &GetNormalCb;
      //input.m_getTexCoord = &GetTexCoordCb;
      //input.m_setTSpaceBasic = &SetTspaceBasicCb;
      //input.m_setTSpace = NULL;


      input.m_getPosition = [](const SMikkTSpaceContext* ctx, float fvPosOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& pos = data->verts.first[data->prims.first[iFace].idx[iVert]].pos;
        fvPosOut[0] = pos.x;
        fvPosOut[1] = pos.y;
        fvPosOut[2] = pos.z;
      };

      input.m_getNormal = [](const SMikkTSpaceContext* ctx, float fvNormOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& nrm = data->verts.first[data->prims.first[iFace].idx[iVert]].nrm;
        fvNormOut[0] = nrm.x;
        fvNormOut[1] = nrm.y;
        fvNormOut[2] = nrm.z;
      };

      input.m_getTexCoord = [](const SMikkTSpaceContext* ctx, float fvTexcOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& tc0 = data->verts.first[data->prims.first[iFace].idx[iVert]].tc0;
        fvTexcOut[0] = tc0.x;
        fvTexcOut[1] = tc0.y;
      };

      input.m_setTSpaceBasic = [](const SMikkTSpaceContext* ctx, const float fvTangent[], float fSign, int iFace, int iVert)
      {
        auto data = reinterpret_cast<SMikkTSpaceUserData*>(ctx->m_pUserData);
        auto& tng = data->verts.first[data->prims.first[iFace].idx[iVert]].tng;
        tng.x = fvTangent[0];
        tng.y = fvTangent[1];
        tng.z = fvTangent[2];
        auto& sgn = data->verts.first[data->prims.first[iFace].idx[iVert]].sgn;
        sgn = fSign;
      };

      SMikkTSpaceContext context;
      context.m_pInterface = &input;
      context.m_pUserData = &data;

      BLAST_ASSERT(1 == genTangSpaceDefault(&context));
    }
  }

  //void UpdateWrap()
  //{
  //  {
  //    auto atlas = xatlas::Create();

  //    for (uint32_t i = 0; i < uint32_t(instances_array.size()); ++i)
  //    {
  //      const auto& vertices = vertices_arrays[i];
  //      const auto vertex_data = reinterpret_cast<const uint8_t*>(vertices.data());
  //      const auto vertex_count = uint32_t(vertices.size());
  //      const auto vertex_stride = uint32_t(sizeof(Vertex));

  //      const auto& triangles = triangles_arrays[i];
  //      const auto index_data = reinterpret_cast<const uint8_t*>(triangles.data());
  //      const auto index_count = uint32_t(triangles.size()) * 3;
  //      const auto index_format = xatlas::IndexFormat::UInt32;

  //      xatlas::MeshDecl mesh_decl;
  //      mesh_decl.vertexCount = vertex_count;
  //      mesh_decl.vertexPositionData = vertex_data + 0;
  //      mesh_decl.vertexPositionStride = vertex_stride;
  //      mesh_decl.vertexNormalData = vertex_data + 16;
  //      mesh_decl.vertexNormalStride = vertex_stride;
  //      mesh_decl.vertexUvData = vertex_data + 48;
  //      mesh_decl.vertexUvStride = vertex_stride;
  //      mesh_decl.indexCount = index_count;
  //      mesh_decl.indexData = index_data;
  //      mesh_decl.indexFormat = index_format;
  //      BLAST_ASSERT(xatlas::AddMeshError::Success == xatlas::AddMesh(atlas, mesh_decl));

  //      //xatlas::UvMeshDecl uv_mesh_decl;
  //      //uv_mesh_decl.vertexCount = vertex_count;
  //      //uv_mesh_decl.vertexStride = vertex_stride;
  //      //uv_mesh_decl.vertexUvData = vertex_data + 48;
  //      //uv_mesh_decl.indexCount = index_count;
  //      //uv_mesh_decl.indexData = index_data;
  //      //uv_mesh_decl.indexFormat = index_format;
  //      //uv_mesh_decl.indexOffset = 0;
  //      //BLAST_ASSERT(xatlas::AddMeshError::Success == xatlas::AddUvMesh(atlas, uv_mesh_decl));
  //    }

  //    xatlas::ChartOptions chartOptions;
  //    chartOptions.useInputMeshUvs = true;
  //    xatlas::ComputeCharts(atlas, chartOptions);

  //    xatlas::PackOptions packOptions;
  //    packOptions.resolution = 2048;
  //    packOptions.texelsPerUnit = 200.0;
  //    xatlas::PackCharts(atlas, packOptions);

  //    BLAST_ASSERT(atlas->meshCount == uint32_t(instances_array.size()));

  //    for (uint32_t i = 0; i < atlas->meshCount; i++)
  //    {
  //      const auto& triangles = triangles_arrays[i];
  //      const auto& mesh = atlas->meshes[i];
  //      BLAST_ASSERT(mesh.indexCount == 3 * uint32_t(triangles.size()))

  //        const auto& vertices = vertices_arrays[i];

  //      std::vector<Vertex> temp_vertices(mesh.vertexCount);
  //      for (uint32_t j = 0; j < mesh.vertexCount; j++)
  //      {
  //        const auto& vertex = mesh.vertexArray[j];

  //        temp_vertices[j] = vertices[vertex.xref];
  //        temp_vertices[j].msk = vertex.atlasIndex;
  //        temp_vertices[j].tc1 = { vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height };
  //      }

  //      std::vector<Triangle> temp_triangles(mesh.indexCount / 3);
  //      for (uint32_t j = 0; j < mesh.indexCount / 3; j++)
  //      {
  //        temp_triangles[j].idx[0] = mesh.indexArray[j * 3 + 0];
  //        temp_triangles[j].idx[1] = mesh.indexArray[j * 3 + 1];
  //        temp_triangles[j].idx[2] = mesh.indexArray[j * 3 + 2];
  //      }

  //      std::swap(vertices_arrays[i], temp_vertices);
  //      std::swap(triangles_arrays[i], temp_triangles);

  //      instances_array[i].vert_count = uint32_t(vertices_arrays[i].size());
  //      instances_array[i].vert_offset = i > 0 ? instances_array[i - 1].vert_offset + instances_array[i - 1].vert_count : 0;
  //      instances_array[i].prim_count = uint32_t(triangles_arrays[i].size());
  //      instances_array[i].prim_offset = i > 0 ? instances_array[i - 1].prim_offset + instances_array[i - 1].prim_count : 0;
  //    }

  //    if (atlas->width > 0 && atlas->height > 0)
  //    {
  //      const auto white = glm::u8vec4(255, 255, 255, 255);

  //      const auto triangle_raster_fn = [](glm::u8vec4* pixels, uint32_t size_x, uint32_t size_y, const float* v0, const float* v1, const float* v2, const glm::u8vec4& color)
  //      {
  //        const auto tc0 = glm::f32vec2(v0[0], v0[1]);
  //        const auto tc1 = glm::f32vec2(v1[0], v1[1]);
  //        const auto tc2 = glm::f32vec2(v2[0], v2[1]);

  //        const auto min = glm::min(glm::min(tc0, tc1), tc2);
  //        const auto max = glm::max(glm::max(tc0, tc1), tc2);

  //        const auto denom = glm::determinant(glm::f32mat2x2(tc1 - tc0, tc2 - tc0));

  //        for (uint32_t i = 0; i < size_y; ++i)
  //        {
  //          if (i < min.y || i > max.y) continue;

  //          for (uint32_t j = 0; j < size_x; ++j)
  //          {
  //            if (j < min.x || j > max.x) continue;

  //            const auto pnt = glm::f32vec2(j + 0.0f, i + 0.0f);

  //            const auto v = glm::determinant(glm::f32mat2x2(pnt - tc0, tc2 - tc0)) / denom;
  //            if (v < 0.0f || v > 1.0f) continue;

  //            const auto w = glm::determinant(glm::f32mat2x2(tc1 - tc0, pnt - tc0)) / denom;
  //            if (w < 0.0f || w > 1.0f) continue;

  //            const auto u = 1.0f - v - w;
  //            if (u < 0.0f || u > 1.0f) continue;

  //            pixels[(size_x * i + j)] = color;

  //            const auto pattern = glm::floor(pnt / 16.0f);
  //            const auto fading = 0.5f * glm::fract(0.5f * (pattern.x + pattern.y)) + 0.5f;

  //            pixels[(size_x * i + j)].x *= fading;
  //            pixels[(size_x * i + j)].y *= fading;
  //            pixels[(size_x * i + j)].z *= fading;
  //          }
  //        }
  //      };

  //      textures_4.resize(atlas->atlasCount);

  //      for (uint32_t z = 0; z < atlas->atlasCount; ++z)
  //      {
  //        //std::vector<uint8_t> image(atlas->width* atlas->height * 3);

  //        Texture texture;
  //        texture.texels.resize(atlas->width * atlas->height);
  //        texture.extent_x = atlas->width;
  //        texture.extent_y = atlas->height;

  //        for (uint32_t i = 0; i < atlas->meshCount; ++i)
  //        {
  //          const auto& mesh = atlas->meshes[i];

  //          for (uint32_t j = 0; j < mesh.chartCount; ++j)
  //          {
  //            const auto& chart = mesh.chartArray[j];
  //            if (chart.atlasIndex != z) continue;

  //            BLAST_ASSERT(chart.atlasIndex != -1);

  //            const auto color = glm::u8vec4(uint8_t((rand() % 255 + 192) * 0.5f), uint8_t((rand() % 255 + 192) * 0.5f), uint8_t((rand() % 255 + 192) * 0.5f), 255);

  //            for (uint32_t k = 0; k < chart.faceCount; ++k)
  //            {
  //              const auto face = chart.faceArray[k];

  //              const auto& vtx0 = mesh.vertexArray[mesh.indexArray[3 * face + 0]];
  //              const auto& vtx1 = mesh.vertexArray[mesh.indexArray[3 * face + 1]];
  //              const auto& vtx2 = mesh.vertexArray[mesh.indexArray[3 * face + 2]];


  //              const int v0[2] = { int(vtx0.uv[0]), int(vtx0.uv[1]) };
  //              const int v1[2] = { int(vtx1.uv[0]), int(vtx1.uv[1]) };
  //              const int v2[2] = { int(vtx2.uv[0]), int(vtx2.uv[1]) };

  //              triangle_raster_fn(texture.texels.data(), texture.extent_x, texture.extent_y, vtx0.uv, vtx1.uv, vtx2.uv, color);
  //            }
  //          }
  //        }

  //        //char filename[256];
  //        //snprintf(filename, sizeof(filename), "example_tris%02u.png", z);
  //        //printf("Writing '%s'...\n", filename);
  //        //stbi_write_png(filename, texture.extent_x, texture.extent_y, 4, texture.texels.data(), 4 * texture.extent_x);

  //        textures_4[z] = std::move(texture);
  //      }
  //    }

  //    xatlas::Destroy(atlas);
  //  }
  //}

  void ImportBroker::Initialize()
  {
    const auto extension = ExtractExtension(file_name);

    if (std::strcmp(extension.c_str(), "obm") == 0)
    {
      ImportOBJM();
    }
    else if (std::strcmp(extension.c_str(), "gltf") == 0)
    {
      ImportGLTF();
    }


    UpdateTangents(instances, triangles, vertices);
  }

  void ImportBroker::Use()
  {}

  void ImportBroker::Discard()
  {
    instances.clear();
    triangles.clear();
    vertices.clear();
  }

  void ImportBroker::ImportGLTF()
  {
    tinygltf::TinyGLTF gltf_ctx;

    tinygltf::Model gltf_model;
    std::string err, warn;
    BLAST_ASSERT(gltf_ctx.LoadASCIIFromFile(&gltf_model, &err, &warn, (path_name + file_name).c_str()));

    std::vector<uint32_t> texture_0_indices;
    std::vector<uint32_t> texture_1_indices;
    std::vector<uint32_t> texture_2_indices;
    std::vector<uint32_t> texture_3_indices;

    const auto gltf_scene = gltf_model.scenes[0];

    for (uint32_t i = 0; i < uint32_t(gltf_scene.nodes.size()); ++i)
    {
      const auto& gltf_node = gltf_model.nodes[gltf_scene.nodes[i]];
      const auto& gltf_mesh = gltf_model.meshes[gltf_node.mesh];

      for (uint32_t j = 0; j < uint32_t(gltf_mesh.primitives.size()); ++j)
      {
        const auto& gltf_primitive = gltf_mesh.primitives[j];
        BLAST_ASSERT(gltf_primitive.mode == TINYGLTF_MODE_TRIANGLES);

        const auto& gltf_material = gltf_model.materials[gltf_primitive.material];

        const auto access_buffer_fn = [&gltf_model](const tinygltf::Accessor& accessor)
        {
          const auto& gltf_view = gltf_model.bufferViews[accessor.bufferView];
          const auto length = gltf_view.byteLength;
          const auto offset = gltf_view.byteOffset;

          const auto& gltf_buffer = gltf_model.buffers[gltf_view.buffer];
          const auto data = &gltf_buffer.data[gltf_view.byteOffset];

          return std::pair{ data, uint32_t(length) };
        };

        const auto& gltf_positions = gltf_model.accessors[gltf_primitive.attributes.at("POSITION")];
        BLAST_ASSERT(gltf_positions.type == TINYGLTF_TYPE_VEC3 && gltf_positions.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
        const auto pos_data = access_buffer_fn(gltf_positions);
        const auto pos_stride = 12;

        const auto& gltf_normals = gltf_model.accessors[gltf_primitive.attributes.at("NORMAL")];
        BLAST_ASSERT(gltf_normals.type == TINYGLTF_TYPE_VEC3 && gltf_normals.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
        const auto nrm_data = access_buffer_fn(gltf_normals);
        const auto nrm_stride = 12;

        const auto& gltf_texcoords = gltf_model.accessors[gltf_primitive.attributes.at("TEXCOORD_0")];
        BLAST_ASSERT(gltf_texcoords.type == TINYGLTF_TYPE_VEC2 && gltf_texcoords.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
        const auto tc0_data = access_buffer_fn(gltf_texcoords);
        const auto tc0_stride = 8;

        const auto& gltf_indices = gltf_model.accessors[gltf_primitive.indices];
        BLAST_ASSERT(gltf_indices.type == TINYGLTF_TYPE_SCALAR);
        const auto idx_data = access_buffer_fn(gltf_indices);
        const auto idx_stride = gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_INT   || gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT   ? 4
                              : gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_SHORT || gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? 2
                              : gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_BYTE  || gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE  ? 1
                              : 0;
        const auto idx_count = gltf_indices.count;

        const auto [instance_vertices, instance_triangles] = PopulateInstance(idx_count, idx_stride, position_scale, coordinate_flip,
          pos_data, pos_stride, idx_data, idx_stride,
          nrm_data, nrm_stride, idx_data, idx_stride,
          tc0_data, tc0_stride, idx_data, idx_stride,
          degenerated_geom_tris_count, degenerated_wrap_tris_count);

        if (instance_vertices.empty() || instance_triangles.empty()) continue;

        const auto vertices_count = uint32_t(instance_vertices.size());
        const auto triangles_count = uint32_t(instance_triangles.size());
        BLAST_LOG("Instance %d: Added vert/prim: %d/%d", j, vertices_count, triangles_count);

        Instance instance;
        instance.transform;
        instance.geometry_idx = uint32_t(instances.size());
        //instance.debug_color{ 0.0f, 0.0f, 0.0f };

        instance.vert_count = uint32_t(instance_vertices.size());
        instance.vert_offset = uint32_t(vertices.size());
        instance.prim_count = uint32_t(instance_triangles.size());
        instance.prim_offset = uint32_t(triangles.size());

        vertices.emplace_back(std::move(instance_vertices));
        triangles.emplace_back(std::move(instance_triangles));

        const auto tex_reindex_fn = [](std::vector<uint32_t>& tex_ids, uint32_t tex_id)
        {
          if (tex_id == -1)
          {
            return uint32_t(-1);
          }

          const auto tex_iter = std::find_if(tex_ids.cbegin(), tex_ids.cend(), [&tex_id](const auto& index) { return tex_id == index; });
          const auto tex_index = tex_iter == tex_ids.cend() ? uint32_t(tex_ids.size()) : uint32_t(tex_iter - tex_ids.cbegin());
          if (tex_index == tex_ids.size())
          {
            tex_ids.push_back(tex_id);
          }
          return tex_index;
        };

        const auto texture_0_id = gltf_material.pbrMetallicRoughness.baseColorTexture.index;
        instance.texture0_idx = texture_0_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_0_indices, texture_0_id);
        const auto texture_1_id = gltf_material.emissiveTexture.index;
        instance.texture1_idx = texture_1_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_1_indices, texture_1_id);
        const auto texture_2_id = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
        instance.texture2_idx = texture_2_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_2_indices, texture_2_id);
        const auto texture_3_id = gltf_material.normalTexture.index;
        instance.texture3_idx = texture_3_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_3_indices, texture_3_id);

        instances.push_back(instance);
      }
    }

    const auto tex_prepare_fn = [&gltf_model](uint32_t texture_index)
    {
      const auto image_index = gltf_model.textures[texture_index].source;

      const auto tex_x = gltf_model.images[image_index].width;
      const auto tex_y = gltf_model.images[image_index].height;
      const auto tex_n = gltf_model.images[image_index].component;
      const auto tex_data = gltf_model.images[image_index].image.data();

      Texture texture;
      texture.texels.resize(tex_x * tex_y);

      for (uint32_t i = 0; i < tex_x * tex_y; ++i)
      {
        const uint8_t r = tex_n > 0 ? tex_data[i * tex_n + 0] : 0; //0xFF;
        const uint8_t g = tex_n > 1 ? tex_data[i * tex_n + 1] : r; //0xFF;
        const uint8_t b = tex_n > 2 ? tex_data[i * tex_n + 2] : r; //0xFF;
        const uint8_t a = tex_n > 3 ? tex_data[i * tex_n + 3] : r; //0xFF;
        texture.texels[i] = glm::u8vec4(r, g, b, a);

      }
      texture.extent_x = tex_x;
      texture.extent_y = tex_y;

      return texture;
    };

    textures_0.resize(texture_0_indices.size());
    for (uint32_t i = 0; i < uint32_t(texture_0_indices.size()); ++i)
    {
      const auto texture_0_index = texture_0_indices[i];
      textures_0[i] = std::move(tex_prepare_fn(texture_0_index));
    }

    textures_1.resize(texture_1_indices.size());
    for (uint32_t i = 0; i < uint32_t(texture_1_indices.size()); ++i)
    {
      const auto texture_1_index = texture_1_indices[i];
      textures_1[i] = std::move(tex_prepare_fn(texture_1_index));
    }

    textures_2.resize(texture_2_indices.size());
    for (uint32_t i = 0; i < uint32_t(texture_2_indices.size()); ++i)
    {
      const auto texture_2_index = texture_2_indices[i];
      textures_2[i] = std::move(tex_prepare_fn(texture_2_index));
    }

    textures_3.resize(texture_3_indices.size());
    for (uint32_t i = 0; i < uint32_t(texture_3_indices.size()); ++i)
    {
      const auto texture_3_index = texture_3_indices[i];
      textures_3[i] = std::move(tex_prepare_fn(texture_3_index));
    }
  }

  void ImportBroker::ImportOBJM()
  {
    tinyobj::attrib_t obj_attrib;
    std::vector<tinyobj::shape_t> obj_shapes;
    std::vector<tinyobj::material_t> obj_materials;

    std::string err, warn;
    BLAST_ASSERT(true == tinyobj::LoadObj(&obj_attrib, &obj_shapes, &obj_materials, &warn, &err, (path_name + file_name).c_str(), path_name.c_str(), true));

    const auto pos_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.vertices.data()), uint32_t(obj_attrib.vertices.size()) };
    const auto pos_stride = 12;

    const auto nrm_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.normals.data()), uint32_t(obj_attrib.normals.size()) };
    const auto nrm_stride = 12;

    const auto tc0_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.texcoords.data()), uint32_t(obj_attrib.texcoords.size()) };
    const auto tc0_stride = 8;

    std::vector<std::string> textures_0_names;
    std::vector<std::string> textures_1_names;
    std::vector<std::string> textures_2_names;
    std::vector<std::string> textures_3_names;

    for (uint32_t i = 0; i < uint32_t(obj_shapes.size()); ++i)
    {
      const auto& obj_mesh = obj_shapes[i].mesh;

      std::unordered_map<int, std::vector<int>> material_id_map;
      for (uint32_t j = 0; j < uint32_t(obj_mesh.material_ids.size()); ++j)
      {
        const auto id = obj_mesh.material_ids[j];
        material_id_map[id].push_back(j);
      }

      uint32_t instance_id = 0u;
      for (const auto& material_id : material_id_map)
      {
        const auto& indices = material_id.second;

        const auto idx_count = uint32_t(indices.size());
        const auto idx_align = 4u;

        const auto idx_data = std::pair{ reinterpret_cast<const uint8_t*>(indices.data()), uint32_t(indices.size()) };
        const auto idx_stride = 4u;

        const auto [instance_vertices, instance_triangles] = PopulateInstance(idx_count, idx_stride, position_scale, coordinate_flip,
          pos_data, pos_stride, idx_data, idx_stride,
          nrm_data, nrm_stride, idx_data, idx_stride,
          tc0_data, tc0_stride, idx_data, idx_stride,
          degenerated_geom_tris_count, degenerated_wrap_tris_count);

        if (instance_vertices.empty() || instance_triangles.empty()) continue;

        const auto vertices_count = uint32_t(instance_vertices.size());
        const auto triangles_count = uint32_t(instance_triangles.size());
        BLAST_LOG("Instance %d: Added vert/prim: %d/%d", instance_id, vertices_count, triangles_count);

        Instance instance;
        instance.transform;
        instance.geometry_idx = uint32_t(instances.size());
        //instance.debug_color{ 0.0f, 0.0f, 0.0f };

        const auto& obj_material = obj_materials[material_id.first];

        const auto debug = false;
        if (debug)
        {
          instance.emission = glm::vec3(0.0f, 0.0f, 0.0f);
          instance.intensity = 0.0f;
          //material.ambient = glm::vec3(obj_material.ambient[0], obj_material.ambient[1], obj_material.ambient[2]);
          //material.dissolve = obj_material.dissolve;
          instance.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
          instance.shininess = 1.0f;
          instance.specular = glm::vec3(0.0f, 0.0f, 0.0f);
          instance.alpha = 1.0f;
          //instance.transmittance = glm::vec3(0.0f, 0.0f, 0.0f) * (1.0f - obj_material.dissolve);
          //instance.ior = 1.0f;
        }
        else
        {

          instance.emission = glm::vec3(obj_material.emission[0], obj_material.emission[1], obj_material.emission[2]);
          instance.intensity = 10.0f;
          //material.ambient = glm::vec3(obj_material.ambient[0], obj_material.ambient[1], obj_material.ambient[2]);
          //material.dissolve = obj_material.dissolve;
          instance.diffuse = glm::vec3(obj_material.diffuse[0], obj_material.diffuse[1], obj_material.diffuse[2]);
          instance.shininess = obj_material.shininess;
          instance.specular = glm::vec3(obj_material.specular[0], obj_material.specular[1], obj_material.specular[2]);
          instance.alpha = 1.0f; // -obj_material.dissolve;
          //instance.transmittance = glm::vec3(obj_material.transmittance[0], obj_material.transmittance[1], obj_material.transmittance[2]) * (1.0f - obj_material.dissolve);
          //instance.ior = obj_material.ior;

          const auto tex_reindex_fn = [](std::vector<std::string>& tex_names, const std::string& tex_name)
          {
            if (tex_name.empty())
            {
              return -1;
            }

            const auto tex_iter = std::find_if(tex_names.cbegin(), tex_names.cend(), [&tex_name](const auto& name) { return tex_name == name; });
            const auto tex_index = tex_iter == tex_names.cend() ? int32_t(tex_names.size()) : int32_t(tex_iter - tex_names.cbegin());
            if (tex_index == tex_names.size())
            {
              tex_names.push_back(tex_name);
            }

            return tex_index;
          };

          const auto& texture_0_name = obj_material.diffuse_texname;
          instance.texture0_idx = texture_0_name.empty() ? uint32_t(-1) : tex_reindex_fn(textures_0_names, texture_0_name);
          const auto& texture_1_name = obj_material.alpha_texname;
          instance.texture1_idx = texture_1_name.empty() ? uint32_t(-1) : tex_reindex_fn(textures_1_names, texture_1_name);
          const auto& texture_2_name = obj_material.specular_texname;
          instance.texture2_idx = texture_2_name.empty() ? uint32_t(-1) : tex_reindex_fn(textures_2_names, texture_2_name);
          const auto& texture_3_name = obj_material.bump_texname;
          instance.texture3_idx = texture_3_name.empty() ? uint32_t(-1) : tex_reindex_fn(textures_3_names, texture_3_name);

          switch (obj_material.illum)
          {
          case 3: // mirror
            instance.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
            instance.specular = glm::vec3(1.0f, 1.0f, 1.0f);
            instance.shininess = float(1 << 16);
            break;
          case 7: // glass
            instance.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
            instance.specular = glm::vec3(0.0f, 0.0f, 0.0f);
            instance.alpha = 1.5f;
            break;
          }
        }

        instance.vert_count = uint32_t(instance_vertices.size());
        instance.vert_offset = uint32_t(vertices.size());
        instance.prim_count = uint32_t(instance_triangles.size());
        instance.prim_offset = uint32_t(triangles.size());

        vertices.emplace_back(std::move(instance_vertices));
        triangles.emplace_back(std::move(instance_triangles));

        instances.push_back(instance);
      }
    }


    const auto load_texture_fn = [this](const std::string& file_name)
    {
      int32_t tex_x = 0;
      int32_t tex_y = 0;
      int32_t tex_n = 0;
      unsigned char* tex_data = stbi_load((path_name + file_name).c_str(), &tex_x, &tex_y, &tex_n, STBI_default);

      Texture texture;
      texture.texels.resize(tex_x * tex_y);

      for (uint32_t i = 0; i < tex_x * tex_y; ++i)
      {
        const uint8_t r = tex_n > 0 ? tex_data[i * tex_n + 0] : 0; //0xFF;
        const uint8_t g = tex_n > 1 ? tex_data[i * tex_n + 1] : r; //0xFF;
        const uint8_t b = tex_n > 2 ? tex_data[i * tex_n + 2] : r; //0xFF;
        const uint8_t a = tex_n > 3 ? tex_data[i * tex_n + 3] : r; //0xFF;
        texture.texels[i] = glm::u8vec4(r, g, b, a);

      }
      stbi_image_free(tex_data);

      texture.extent_x = tex_x;
      texture.extent_y = tex_y;

      return texture;
    };

    if (!textures_0_names.empty())
    {
      textures_0.resize(textures_0_names.size());
      for (uint32_t i = 0; i < uint32_t(textures_0_names.size()); ++i)
      {
        const auto& texture_0_name = textures_0_names[i];
        textures_0[i] = std::move(load_texture_fn(texture_0_name));
      }
    }

    if (!textures_1_names.empty())
    {
      textures_1.resize(textures_1_names.size());
      for (uint32_t i = 0; i < uint32_t(textures_1_names.size()); ++i)
      {
        const auto& texture_1_name = textures_1_names[i];
        textures_1[i] = std::move(load_texture_fn(texture_1_name));
      }
    }

    if (!textures_2_names.empty())
    {
      textures_2.resize(textures_2_names.size());
      for (uint32_t i = 0; i < uint32_t(textures_2_names.size()); ++i)
      {
        const auto& texture_2_name = textures_2_names[i];
        textures_2[i] = std::move(load_texture_fn(texture_2_name));
      }
    }

    if (!textures_3_names.empty())
    {
      textures_3.resize(textures_3_names.size());
      for (uint32_t i = 0; i < uint32_t(textures_3_names.size()); ++i)
      {
        const auto& texture_3_name = textures_3_names[i];
        textures_3[i] = std::move(load_texture_fn(texture_3_name));
      }
    }
  }

  ImportBroker::ImportBroker(const std::shared_ptr<Core>& core, const std::shared_ptr<Util>& util)
    : Broker("ImportBroker", core, util)
  {}

  ImportBroker::~ImportBroker()
  {}
}