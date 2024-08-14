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
  std::tuple<std::vector<Vertex>, std::vector<Triangle>, glm::fvec3, glm::fvec3> PopulateInstance(uint32_t count, uint32_t align,
    const glm::uvec3& idx_order, const glm::fmat4x4& pos_transform, const glm::fmat3x3& nrm_transform, const glm::fmat2x2& tc0_transform,
    std::pair<const uint8_t*, uint32_t> pos_data, uint32_t pos_stride, std::pair<const uint8_t*, uint32_t> pos_idx_data, uint32_t pos_id_stride,
    std::pair<const uint8_t*, uint32_t> nrm_data, uint32_t nrm_stride, std::pair<const uint8_t*, uint32_t> nrm_idx_data, uint32_t nrm_id_stride,
    std::pair<const uint8_t*, uint32_t> tc0_data, uint32_t tc0_stride, std::pair<const uint8_t*, uint32_t> tc0_idx_data, uint32_t tc0_id_stride)
  {
    const auto create_vertex_fn = [align,
      pos_transform, nrm_transform, tc0_transform,
      pos_data, pos_stride, pos_idx_data, pos_id_stride,
      nrm_data, nrm_stride, nrm_idx_data, nrm_id_stride,
      tc0_data, tc0_stride, tc0_idx_data, tc0_id_stride]
      (uint32_t index)
    {
      const auto pos_idx = align == 4 ? *(reinterpret_cast<const uint32_t*>(pos_idx_data.first + pos_id_stride * index))
        : align == 2 ? *(reinterpret_cast<const uint16_t*>(pos_idx_data.first + pos_id_stride * index))
        : align == 1 ? *(reinterpret_cast<const uint8_t*>(pos_idx_data.first + pos_id_stride * index))
        : -1;
      const auto pos_ptr = reinterpret_cast<const float*>(pos_data.first + pos_stride * pos_idx);
      const auto& pos = glm::fvec3(pos_ptr[0], pos_ptr[1], pos_ptr[2]);

      const auto nrm_idx = align == 4 ? *(reinterpret_cast<const uint32_t*>(nrm_idx_data.first + nrm_id_stride * index))
        : align == 2 ? *(reinterpret_cast<const uint16_t*>(nrm_idx_data.first + nrm_id_stride * index))
        : align == 1 ? *(reinterpret_cast<const uint8_t*>(nrm_idx_data.first + nrm_id_stride * index))
        : -1;
      const auto nrm_ptr = reinterpret_cast<const float*>(nrm_data.first + nrm_stride * nrm_idx);
      const auto& nrm = glm::fvec3(nrm_ptr[0], nrm_ptr[1], nrm_ptr[2]);

      const auto tc0_idx = align == 4 ? *(reinterpret_cast<const uint32_t*>(tc0_idx_data.first + tc0_id_stride * index))
        : align == 2 ? *(reinterpret_cast<const uint16_t*>(tc0_idx_data.first + tc0_id_stride * index))
        : align == 1 ? *(reinterpret_cast<const uint8_t*>(tc0_idx_data.first + tc0_id_stride * index))
        : -1;
      const auto tc0_ptr = reinterpret_cast<const float*>(tc0_data.first + tc0_stride * tc0_idx);
      const auto& tc0 = glm::f32vec2(tc0_ptr[0], tc0_ptr[1]);

      Vertex vertex;

      vertex.pos = pos_transform * glm::fvec4{ pos.x, pos.y, pos.z, 1.0f };
      vertex.nrm = nrm_transform * glm::normalize(glm::fvec3{ nrm.x, nrm.y, nrm.z });
      vertex.tc0 = tc0_transform * glm::f32vec2(tc0.x, tc0.y);

      return vertex;
    };

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

    const auto compare_vertex_fn = [](const Vertex& l, const Vertex& r) { return (memcmp(&l, &r, sizeof(Vertex)) == 0); };
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

    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    auto bb_min = glm::fvec3{ FLT_MAX, FLT_MAX, FLT_MAX };
    auto bb_max = glm::fvec3{-FLT_MAX,-FLT_MAX,-FLT_MAX };

    auto degenerated_geom_tris_count = 0u;
    auto degenerated_wrap_tris_count = 0u;

    for (uint32_t k = 0; k < count / 3; ++k)
    {
      const auto vtx0 = create_vertex_fn(k * 3 + 0);
      bb_min = glm::min(bb_min, vtx0.pos);
      bb_max = glm::max(bb_max, vtx0.pos);

      const auto vtx1 = create_vertex_fn(k * 3 + 1);
      bb_min = glm::min(bb_min, vtx1.pos);
      bb_max = glm::max(bb_max, vtx1.pos);
      
      const auto vtx2 = create_vertex_fn(k * 3 + 2);      
      bb_min = glm::min(bb_min, vtx2.pos);
      bb_max = glm::max(bb_max, vtx2.pos);

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

      Triangle triangle;

      triangle.idx[idx_order[0]] = remap_vertex_fn(vertices, vtx0);
      triangle.idx[idx_order[1]] = remap_vertex_fn(vertices, vtx1);
      triangle.idx[idx_order[2]] = remap_vertex_fn(vertices, vtx2);

      triangles.push_back(triangle);
    }

    return { vertices, triangles, bb_min, bb_max };
  }


  void UpdateTangents(std::vector<Instance>& instances, std::vector<Triangle>& triangles, std::vector<Vertex>& vertices)
  {

    for (uint32_t i = 0; i < uint32_t(instances.size()); ++i)
    {
      auto prim_data = triangles.data() + instances[i].prim_offset;
      const auto prim_count = instances[i].prim_count;

      auto vert_data = vertices.data() + instances[i].vert_offset;
      const auto vert_count = instances[i].vert_count;

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


  std::shared_ptr<Property> CreatePropertyFromTexture(std::pair<const void*, uint32_t> bytes, int32_t tex_x, int32_t tex_y, uint32_t mipmaps)
  {
    const auto mipmap_count_fn = [](uint32_t value)
    {
      uint32_t power = 0;
      while ((value >> power) > 0) ++power;
      return power;
    };
    mipmaps = mipmaps > 0 ? mipmaps : std::min(mipmap_count_fn(tex_x), mipmap_count_fn(tex_y));

    //const uint32_t tex_x = (1 << mipmaps) - 1;
    //const uint32_t tex_y = (1 << mipmaps) - 1;
    //const uint32_t tex_n = 4;

    const auto texel_count = uint32_t(((1 << mipmaps) * (1 << mipmaps) - 1) / 3);
    const auto texel_stride = uint32_t(sizeof(glm::u8vec4));
    unsigned char* texel_data = new unsigned char[texel_count * 4];

    auto dst_tex_x = 1 << (mipmaps - 1);
    auto dst_tex_y = 1 << (mipmaps - 1);
    auto dst_tex_n = 4;
    auto dst_tex_data = texel_data;

    stbir_resize_uint8(reinterpret_cast<const unsigned char*>(bytes.first), tex_x, tex_y, 0, dst_tex_data, dst_tex_x, dst_tex_y, 0, 4);

    auto src_tex_x = dst_tex_x;
    auto src_tex_y = dst_tex_y;
    auto src_tex_n = dst_tex_n;
    auto src_tex_data = dst_tex_data;

    for (uint32_t i = 1; i < mipmaps; ++i)
    {
      dst_tex_x = 1 << (mipmaps - 1 - i);
      dst_tex_y = 1 << (mipmaps - 1 - i);
      dst_tex_n = src_tex_n;
      dst_tex_data += src_tex_x * src_tex_y * src_tex_n;
      stbir_resize_uint8(src_tex_data, src_tex_x, src_tex_y, 0, dst_tex_data, dst_tex_x, dst_tex_y, 0, dst_tex_n);

      src_tex_data = dst_tex_data;
      src_tex_x = dst_tex_x;
      src_tex_y = dst_tex_y;
      src_tex_n = dst_tex_n;
    }


    const auto texels_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
    {
      texels_property->RawAllocate(texel_count * texel_stride);
      texels_property->SetRawBytes({ texel_data, texel_count * texel_stride }, 0);
    }
    delete[] texel_data;

    return texels_property;
  }


  std::shared_ptr<Property> CreatePropertyFromTextures(const std::vector<Texture>& textures, uint32_t mipmaps)
  {
    auto textures_property = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));

    if (textures.empty())
    {
      textures_property->SetArraySize(uint32_t(1));

      const auto texel_value = glm::u8vec4(255, 255, 255, 255);
      const auto texel_size = uint32_t(sizeof(texel_value));

      const auto texels_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
      texels_property->RawAllocate(texel_size);
      texels_property->SetRawBytes({ &texel_value, texel_size }, 0);
      textures_property->SetArrayItem(0, texels_property);
    }
    else
    {
      textures_property->SetArraySize(uint32_t(textures.size()));
      for (uint32_t i = 0; i < textures_property->GetArraySize(); ++i)
      {
        const Texture& texture = textures[i];

        const auto tex_x = int32_t(texture.extent_x);
        const auto tex_y = int32_t(texture.extent_y);
        const auto tex_data = reinterpret_cast<const void*>(texture.texels.data());
        const auto tex_size = uint32_t(texture.texels.size() * sizeof(glm::u8vec4));

        const auto mipmaps_property = CreatePropertyFromTexture(std::pair(tex_data, tex_size), tex_x, tex_y, mipmaps);

        textures_property->SetArrayItem(i, mipmaps_property);
      }
    }
    return textures_property;
  }

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

    const auto gltf_scene = gltf_model.scenes[0];
    std::stack<int, std::vector<int>> node_indices(gltf_scene.nodes);

    struct NodeData
    {
      int mesh_index = -1;
      glm::fmat4x4 world_matrix;
    };

    std::vector<NodeData> mesh_relations;

    std::function<void(const tinygltf::Node&, const glm::fmat4x4&)> parse_node_hierarchy;

    parse_node_hierarchy = [&gltf_model, &mesh_relations, &parse_node_hierarchy]
    (const tinygltf::Node& node, glm::fmat4x4 parent_transform) -> void
    {
        auto transform = glm::identity<glm::fmat4x4>();

        if (node.rotation.size() == 4)
        {
          glm::quat rotation = glm::make_quat(node.rotation.data());
          transform = glm::mat4_cast(rotation);
        }

        if (node.translation.size() == 3)
        {
          const glm::fvec3 translation = glm::make_vec3(node.translation.data());
          transform[3] = glm::fvec4(translation, 1.0f);
        }

        if (node.scale.size() == 3)
        {
          const glm::fvec3 scale = glm::make_vec3(node.scale.data());
          transform = glm::scale(transform, scale);
        }

        transform = parent_transform * transform;

        NodeData node_data = { node.mesh, transform };

        if (node_data.mesh_index != -1)
        {
          mesh_relations.push_back(node_data);
        }

        for (const auto& child_index : node.children)
        {
          parse_node_hierarchy(gltf_model.nodes[child_index], transform);
        }
    };

    for (auto node_index : gltf_scene.nodes)
    {
      const auto& node = gltf_model.nodes[node_index];
      parse_node_hierarchy(node, glm::identity<glm::fmat4x4>());
    }
    

    /*while (!node_indices.empty())
    {
      const auto& gltf_node = gltf_model.nodes[node_indices.top()]; node_indices.pop();
      for (const auto& node_index : gltf_node.children) { node_indices.push(node_index); }

      NodeData node_data;

      if (gltf_node.mesh != -1) { node_data.mesh_index = gltf_node.mesh; }


      if (node_data.mesh_index != -1) { mesh_relations.push_back(node_data); }
    }*/

    const auto instance_convert_fn = [&gltf_model](const tinygltf::Primitive& gltf_primitive, const glm::fmat4x4 transform, float scale, bool z_up, bool to_lhs, bool flip_v)
    {
      BLAST_ASSERT(gltf_primitive.mode == TINYGLTF_MODE_TRIANGLES);

      const auto access_buffer_fn = [&gltf_model](const tinygltf::Accessor& accessor)
      {
        const auto& gltf_view = gltf_model.bufferViews[accessor.bufferView];
        const auto length = gltf_view.byteLength;
        const auto offset = gltf_view.byteOffset;

        const auto& gltf_buffer = gltf_model.buffers[gltf_view.buffer];
        const auto data = &gltf_buffer.data[accessor.byteOffset];

        return std::pair{ data + offset, uint32_t(length) };
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
      const auto idx_stride = gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_INT || gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT ? 4
        : gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_SHORT || gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? 2
        : gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_BYTE || gltf_indices.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? 1
        : 0;
      const auto idx_count = gltf_indices.count;

      const auto z_up_transform = glm::fmat3x3(
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 1.0f, 0.0f
      );
      const auto lhs_transform = glm::fmat3x3(
       -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
      );
      const auto trs_transform = glm::fmat3x3(transform);
      const auto crd_transform = to_lhs ? lhs_transform * trs_transform : trs_transform;
      const auto nrm_transform = z_up ? z_up_transform * crd_transform : crd_transform;
      const auto scl_transform = glm::fmat4x4(nrm_transform * scale);
      const auto pos_transform = glm::fmat4x4(scl_transform[0], scl_transform[1], scl_transform[2], transform[3]);

      const auto flip_v_tranform = glm::fmat2x2(
        0.0f,-1.0f,
        1.0f, 0.0f
      );
      const auto tc0_transform = flip_v ? flip_v_tranform : glm::identity<glm::fmat2x2>();
      const auto idx_order = to_lhs ? glm::uvec3{ 0u, 2u, 1u } : glm::uvec3{ 0u, 1u, 2u };

      auto degenerated_geom_tris_count = 0u;
      auto degenerated_wrap_tris_count = 0u;
      const auto [vertices, triangles, bb_min, bb_max] = PopulateInstance(idx_count, idx_stride,
        idx_order, pos_transform, nrm_transform, tc0_transform,
        pos_data, pos_stride, idx_data, idx_stride,
        nrm_data, nrm_stride, idx_data, idx_stride,
        tc0_data, tc0_stride, idx_data, idx_stride);

      return std::make_tuple(vertices, triangles, bb_min, bb_max);
    };

    std::vector<uint32_t> texture_0_indices;
    std::vector<uint32_t> texture_1_indices;
    std::vector<uint32_t> texture_2_indices;
    std::vector<uint32_t> texture_3_indices;

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

    for (uint32_t i = 0; i < uint32_t(mesh_relations.size()); ++i)
    {
      const auto& gltf_mesh = gltf_model.meshes[mesh_relations[i].mesh_index];
      const auto& transform = mesh_relations[i].world_matrix;
      for (uint32_t k = 0; k < uint32_t(gltf_mesh.primitives.size()); ++k)
      {
        const auto& gltf_primitive = gltf_mesh.primitives[k];
        const auto [instance_vertices, instance_triangles, instance_bb_min, instance_bb_max] = 
          instance_convert_fn(gltf_primitive, transform, position_scale, coordinate_flip, conversion_from_rhs, false);

        if (instance_vertices.empty() || instance_triangles.empty()) continue;

        Instance instance;
        instance.transform = glm::identity<glm::fmat3x4>();
        instance.geometry_idx = uint32_t(instances.size());
        instance.bb_min = instance_bb_min;
        instance.bb_max = instance_bb_max;
        //instance.debug_color{ 0.0f, 0.0f, 0.0f };

        const auto& gltf_material = gltf_model.materials[gltf_primitive.material];
        const auto texture_0_id = gltf_material.pbrMetallicRoughness.baseColorTexture.index;
        instance.texture0_idx = texture_0_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_0_indices, texture_0_id);
        const auto texture_1_id = gltf_material.emissiveTexture.index;
        instance.texture1_idx = texture_1_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_1_indices, texture_1_id);
        const auto texture_2_id = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
        instance.texture2_idx = texture_2_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_2_indices, texture_2_id);
        const auto texture_3_id = gltf_material.normalTexture.index;
        instance.texture3_idx = texture_3_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_3_indices, texture_3_id);

        instance.vert_count = uint32_t(instance_vertices.size());
        instance.vert_offset = uint32_t(vertices.size());
        instance.prim_count = uint32_t(instance_triangles.size());
        instance.prim_offset = uint32_t(triangles.size());

        std::copy(instance_vertices.begin(), instance_vertices.end(), std::back_inserter(vertices));
        std::copy(instance_triangles.begin(), instance_triangles.end(), std::back_inserter(triangles));

        instances.push_back(instance);

        BLAST_LOG("Instance %d: Added vert/prim: %d/%d", instance.geometry_idx, instance.vert_count, instance.prim_count);
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

      std::unordered_map<int, std::vector<glm::uvec3>> material_id_map;
      for (uint32_t j = 0; j < uint32_t(obj_mesh.material_ids.size()); ++j)
      {
        const auto id = obj_mesh.material_ids[j];

        const auto& idx_0 = obj_mesh.indices[3 * j + 0];
        material_id_map[id].emplace_back(idx_0.vertex_index, idx_0.normal_index, idx_0.texcoord_index);
        const auto& idx_1 = obj_mesh.indices[3 * j + 1];
        material_id_map[id].emplace_back(idx_1.vertex_index, idx_1.normal_index, idx_1.texcoord_index);
        const auto& idx_2 = obj_mesh.indices[3 * j + 2];
        material_id_map[id].emplace_back(idx_2.vertex_index, idx_2.normal_index, idx_2.texcoord_index);
      }

      for (const auto& material_id : material_id_map)
      {
        const auto idx_count = uint32_t(material_id.second.size());
        const auto idx_align = 4u;

        const auto pos_idx_data = std::pair{ reinterpret_cast<const uint8_t*>(material_id.second.data()) + 0u, uint32_t(material_id.second.size()) };
        const auto pos_idx_stride = 3 * 4u;
        const auto nrm_idx_data = std::pair{ reinterpret_cast<const uint8_t*>(material_id.second.data()) + 4u, uint32_t(material_id.second.size()) };
        const auto nrm_idx_stride = 3 * 4u;
        const auto tc0_idx_data = std::pair{ reinterpret_cast<const uint8_t*>(material_id.second.data()) + 8u, uint32_t(material_id.second.size()) };
        const auto tc0_idx_stride = 3 * 4u;

        const auto z_up_transform = glm::fmat3x3(
          1.0f, 0.0f, 0.0f,
          0.0f, 0.0f,-1.0f,
          0.0f, 1.0f, 0.0f
        );
        const auto lhs_transform = glm::fmat3x3(
         -1.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 1.0f
        );
        const auto crd_transform = conversion_from_rhs ? lhs_transform : glm::identity<glm::fmat3x3>();
        const auto nrm_transform = coordinate_flip ? z_up_transform * crd_transform : crd_transform;
        const auto pos_transform = nrm_transform * position_scale;

        const auto flip_v_tranform = glm::fmat2x2(
          1.0f, 0.0f,
          0.0f,-1.0f
        );
        const auto tc0_transform = true ? flip_v_tranform : glm::identity<glm::fmat2x2>();
        const auto idx_order = conversion_from_rhs ? glm::uvec3{ 0u, 2u, 1u } : glm::uvec3{ 0u, 1u, 2u };

        const auto [instance_vertices, instance_triangles, instance_bb_min, instance_bb_max] = PopulateInstance(idx_count, idx_align,
          idx_order, pos_transform, nrm_transform, tc0_transform,
          pos_data, pos_stride, pos_idx_data, pos_idx_stride,
          nrm_data, nrm_stride, nrm_idx_data, nrm_idx_stride,
          tc0_data, tc0_stride, tc0_idx_data, tc0_idx_stride);

        if (instance_vertices.empty() || instance_triangles.empty()) continue;

        const auto vertices_count = uint32_t(instance_vertices.size());
        const auto triangles_count = uint32_t(instance_triangles.size());
        BLAST_LOG("Instance %d: Added vert/prim: %d/%d", instances.size(), vertices_count, triangles_count);

        Instance instance;
        instance.transform = glm::identity<glm::fmat3x4>();
        instance.geometry_idx = uint32_t(instances.size());
        instance.bb_min = instance_bb_min;
        instance.bb_max = instance_bb_max;
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

        //vertices.emplace_back(std::move(instance_vertices));
        //triangles.emplace_back(std::move(instance_triangles));

        std::copy(instance_vertices.begin(), instance_vertices.end(), std::back_inserter(vertices));
        std::copy(instance_triangles.begin(), instance_triangles.end(), std::back_inserter(triangles));

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

  void ImportBroker::Export(std::shared_ptr<Property>& property) const
  {
    auto temp = std::shared_ptr<Property>(new Property(Property::TYPE_OBJECT));
    //property->setSetValue(Property::object());
  
    const auto instances_property = CreateBufferProperty(instances.data(), uint32_t(sizeof(Instance)), uint32_t(instances.size()));
    temp->SetObjectItem("instances", instances_property);
  
    const auto triangles_property = CreateBufferProperty(triangles.data(), uint32_t(sizeof(Triangle)), uint32_t(triangles.size()));
    temp->SetObjectItem("triangles", triangles_property);

    const auto vertices_property = CreateBufferProperty(vertices.data(), uint32_t(sizeof(Vertex)), uint32_t(vertices.size()));
    temp->SetObjectItem("vertices", vertices_property);
  
    //const auto vertices0_property = CreateBufferProperty(scene_vertices0.data(), uint32_t(sizeof(Vertex0)), uint32_t(scene_vertices0.size()));
    //property->SetObjectItem("vertices0", vertices0_property);
    //const auto vertices1_property = CreateBufferProperty(scene_vertices1.data(), uint32_t(sizeof(Vertex1)), uint32_t(scene_vertices1.size()));
    //property->SetObjectItem("vertices1", vertices1_property);
    //const auto vertices2_property = CreateBufferProperty(scene_vertices2.data(), uint32_t(sizeof(Vertex2)), uint32_t(scene_vertices2.size()));
    //property->SetObjectItem("vertices2", vertices2_property);
    //const auto vertices3_property = CreateBufferProperty(scene_vertices3.data(), uint32_t(sizeof(Vertex3)), uint32_t(scene_vertices3.size()));
    //property->SetObjectItem("vertices3", vertices3_property);
  
    const auto textures0_property = CreatePropertyFromTextures(textures_0, texture_level);
    temp->SetObjectItem("textures0", textures0_property);
    const auto textures1_property = CreatePropertyFromTextures(textures_1, texture_level);
    temp->SetObjectItem("textures1", textures1_property);
    const auto textures2_property = CreatePropertyFromTextures(textures_2, texture_level);
    temp->SetObjectItem("textures2", textures2_property);
    const auto textures3_property = CreatePropertyFromTextures(textures_3, texture_level);
    temp->SetObjectItem("textures3", textures3_property);

    property = temp;
  }


  std::shared_ptr<Property> ImportAsPanoEXR(const std::string& path, const std::string& name, float exposure, uint32_t mipmaps)
  {
    int32_t src_tex_x = 0;
    int32_t src_tex_y = 0;
    int32_t src_tex_n = 4;
    float* src_tex_data = nullptr;
    BLAST_ASSERT(0 == LoadEXR(&src_tex_data, &src_tex_x, &src_tex_y, (name).c_str(), nullptr));

    int32_t dst_tex_x = src_tex_x;
    int32_t dst_tex_y = src_tex_y;
    int32_t dst_tex_n = 4;
    float* dst_tex_data = new float[dst_tex_x * dst_tex_y * dst_tex_n];
    for (uint32_t j = 0; j < src_tex_x * src_tex_y; ++j)
    {
      const auto r = src_tex_n > 0 ? src_tex_data[j * src_tex_n + 0] : 0; //0xFF;
      const auto g = src_tex_n > 1 ? src_tex_data[j * src_tex_n + 1] : r; //0xFF;
      const auto b = src_tex_n > 2 ? src_tex_data[j * src_tex_n + 2] : r; //0xFF;
      const auto a = src_tex_n > 3 ? src_tex_data[j * src_tex_n + 3] : r; //0xFF;
      dst_tex_data[j * dst_tex_n + 0] = r * exposure;
      dst_tex_data[j * dst_tex_n + 1] = g * exposure;
      dst_tex_data[j * dst_tex_n + 2] = b * exposure;
      dst_tex_data[j * dst_tex_n + 3] = a * exposure;
    }
    delete[] src_tex_data;

    src_tex_data = dst_tex_data;
    src_tex_x = dst_tex_x;
    src_tex_y = dst_tex_y;
    src_tex_n = dst_tex_n;

    const auto get_pot_fn = [](int32_t value)
    {
      int32_t power = 0;
      while ((1 << power) < value) ++power;
      return power;
    };

    const auto pow_tex_x = get_pot_fn(src_tex_x);
    const auto pow_tex_y = get_pot_fn(src_tex_y);
    const auto pow_delta = std::abs(pow_tex_x - pow_tex_y);

    //const uint32_t tex_x = (1 << (pow_tex_x > pow_tex_y ? mipmap_count + pow_delta : mipmap_count)) - 1;
    //const uint32_t tex_y = (1 << (pow_tex_y > pow_tex_x ? mipmap_count + pow_delta : mipmap_count)) - 1;
    //const uint32_t tex_n = 4;

    const auto texel_pow_x = pow_tex_x > pow_tex_y ? mipmaps + pow_delta : mipmaps;
    const auto texel_pow_y = pow_tex_y > pow_tex_x ? mipmaps + pow_delta : mipmaps;
    const auto texel_count = uint32_t(((1 << texel_pow_x) * (1 << texel_pow_y) - 1) / 3);
    const auto texel_stride = uint32_t(sizeof(glm::f32vec4));
    float* texel_data = new float[texel_count * 4];

    dst_tex_x = 1 << (texel_pow_x - 1);
    dst_tex_y = 1 << (texel_pow_y - 1);
    dst_tex_n = src_tex_n;
    dst_tex_data = texel_data;
    stbir_resize_float(src_tex_data, src_tex_x, src_tex_y, 0, dst_tex_data, dst_tex_x, dst_tex_y, 0, dst_tex_n);
    delete[] src_tex_data;

    //const auto size_x_property = std::shared_ptr<Property>(new Property());
    //size_x_property->SetValue(uint32_t(dst_tex_x));

    //const auto size_y_property = std::shared_ptr<Property>(new Property());
    //size_y_property->SetValue(uint32_t(dst_tex_y));

    //const auto mipmaps_property = std::shared_ptr<Property>(new Property());
    //mipmaps_property->SetValue(uint32_t(mipmaps));

    src_tex_data = dst_tex_data;
    src_tex_x = dst_tex_x;
    src_tex_y = dst_tex_y;
    src_tex_n = dst_tex_n;

    for (uint32_t i = 1; i < mipmaps; ++i)
    {
      dst_tex_x = src_tex_x >> 1;
      dst_tex_y = src_tex_y >> 1;
      dst_tex_n = src_tex_n;
      dst_tex_data += src_tex_x * src_tex_y * src_tex_n;
      stbir_resize_float(src_tex_data, src_tex_x, src_tex_y, 0, dst_tex_data, dst_tex_x, dst_tex_y, 0, dst_tex_n);

      src_tex_data = dst_tex_data;
      src_tex_x = dst_tex_x;
      src_tex_y = dst_tex_y;
      src_tex_n = dst_tex_n;
    }

    const auto texels_property = CreateBufferProperty(texel_data, texel_stride, texel_count);
    //std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
    //{
    //  texels_property->RawAllocate(texel_count * texel_stride);
    //  texels_property->SetRawBytes({ texel_data, texel_count * texel_stride }, 0);
    //}
    delete[] texel_data;

    const auto texture_property = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
    //texture_property->SetValue(Property::array());
    texture_property->SetArraySize(1);
    texture_property->SetArrayItem(0, texels_property);

    return texture_property;
  }

  std::shared_ptr<Property> ImportAsCubeMapEXR(const std::string& path, const std::string& name, float exposure, uint32_t mipmaps)
  {
    const auto cubemap_layer_counter = 6u;
    const auto cubemap_layer_size = 1 << (mipmaps - 1);

    auto texture_property = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
    texture_property->SetArraySize(cubemap_layer_counter);

    enum CUBEMAP_LAYERS
    {
      CUBEMAP_POSITIVE_X,
      CUBEMAP_NEGATIVE_X,
      CUBEMAP_POSITIVE_Y,
      CUBEMAP_NEGATIVE_Y,
      CUBEMAP_POSITIVE_Z,
      CUBEMAP_NEGATIVE_Z
    };

    const auto uv_to_cube = [](float u, float v, uint32_t layer)
    {
      glm::fvec3 text_coord = { 0.0f, 0.0f, 0.0f };

      switch (layer)
      {
      case CUBEMAP_POSITIVE_X:
        text_coord = { 1.0f, -(2.0f * v - 1.0f), -(2.0f * u - 1.0f) };
        break;
      case CUBEMAP_NEGATIVE_X:
        text_coord = { -1.0f, -(2.0f * v - 1.0f), (2.0f * u - 1.0f) };
        break;
      case CUBEMAP_POSITIVE_Y:
        text_coord = { (2.0f * u - 1.0f), 1.0f, (2.0f * v - 1.0f) };
        break;
      case CUBEMAP_NEGATIVE_Y:
        text_coord = { (2.0f * u - 1.0f), -1.0f, -(2.0f * v - 1.0f) };
        break;
      case CUBEMAP_POSITIVE_Z:
        text_coord = { (2.0f * u - 1.0f), -(2.0f * v - 1.0f), 1.0f };
        break;
      case CUBEMAP_NEGATIVE_Z:
        text_coord = { -(2.0f * u - 1.0f), -(2.0f * v - 1.0f), -1.0f };
        break;
      }
      return glm::normalize(text_coord);
    };

    const auto cube_to_pano = [](const glm::fvec3& cube_tex_coord)
    {
      glm::fvec2 tex_coord = { 0.0f, 0.0f };
      tex_coord.x = 0.5f * atan2(cube_tex_coord.x, cube_tex_coord.z) / glm::pi<float>() + 0.5f;
      tex_coord.y = -asin(cube_tex_coord.y) / glm::pi<float>() + 0.5f;
      return tex_coord;
    };

    const auto src_tex_n = 4;
    const auto dst_tex_n = 4;
    auto pano_tex_x = 0;
    auto pano_tex_y = 0;
    float* pano_tex_data = nullptr;
    BLAST_ASSERT(0 == LoadEXR(&pano_tex_data, &pano_tex_x, &pano_tex_y, (name).c_str(), nullptr));

    for (uint32_t i = 0; i < cubemap_layer_counter; ++i)
    {
      auto dst_tex_x = cubemap_layer_size;
      auto dst_tex_y = cubemap_layer_size;
      const auto texel_pow = mipmaps;

      const auto texel_count = uint32_t(((1 << texel_pow) * (1 << texel_pow) - 1) / 3);
      float* texel_data = new float[texel_count * dst_tex_n];

      {
        auto dst_tex_data = texel_data;
        for (uint32_t j = 0; j < dst_tex_x; ++j)
        {
          for (uint32_t k = 0; k < dst_tex_y; ++k)
          {
            const float u = (float(j) + 0.5f) / dst_tex_x;
            const float v = (float(k) + 0.5f) / dst_tex_y;

            const auto cube_text_coord = uv_to_cube(u, v, i);
            const auto pano_text_coord = cube_to_pano(cube_text_coord);

            const auto x = uint32_t(pano_tex_x * pano_text_coord.x);
            const auto y = uint32_t(pano_tex_y * pano_text_coord.y);
            const auto texel_num = y * pano_tex_x + x;

            const auto index = j + k * dst_tex_x;
            const auto offset = index * dst_tex_n;

            dst_tex_data[offset + 0] = pano_tex_data[texel_num * src_tex_n + 0] * exposure;
            dst_tex_data[offset + 1] = pano_tex_data[texel_num * src_tex_n + 1] * exposure;
            dst_tex_data[offset + 2] = pano_tex_data[texel_num * src_tex_n + 2] * exposure;
            dst_tex_data[offset + 3] = pano_tex_data[texel_num * src_tex_n + 3] * exposure;
          }
        }
      }

      {
        auto src_tex_x = cubemap_layer_size;
        auto src_tex_y = cubemap_layer_size;
        auto dst_tex_data = texel_data;
        auto src_tex_data = dst_tex_data;

        auto offset = 0u;
        for (uint32_t i = 1; i < mipmaps; ++i)
        {
          dst_tex_x = src_tex_x >> 1;
          dst_tex_y = src_tex_y >> 1;
          dst_tex_data += src_tex_x * src_tex_y * dst_tex_n;
          offset += src_tex_x * src_tex_y * dst_tex_n;
          stbir_resize_float(src_tex_data, src_tex_x, src_tex_y, 0, dst_tex_data, dst_tex_x, dst_tex_y, 0, dst_tex_n);

          src_tex_data = dst_tex_data;
          src_tex_x = dst_tex_x;
          src_tex_y = dst_tex_y;
        }
      }

      const auto texels_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
      {
        const auto texel_stride = uint32_t(sizeof(float) * dst_tex_n);
        texels_property->RawAllocate(texel_count * texel_stride);
        texels_property->SetRawBytes({ texel_data, texel_count * texel_stride }, 0);
      }

      texture_property->SetArrayItem(i, texels_property);
      delete[] texel_data;
    }

    delete[] pano_tex_data;

    return texture_property;
  }

  ImportBroker::ImportBroker(Wrap& wrap)
    : Broker("import_broker", wrap)
  {}

  ImportBroker::~ImportBroker()
  {}
}