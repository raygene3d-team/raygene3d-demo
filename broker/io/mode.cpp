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


#include "mode.h"

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#include <stb/stb_image.h>
//#include <stb/stb_image_write.h>
//#include <stb/stb_image_resize.h>



namespace RayGene3D
{
  namespace IO
  {
    Geometry::Geometry(size_t idx_count, uint32_t idx_align, const glm::uvec3& idx_order,
      CByteData pos_data, uint32_t pos_stride, CByteData pos_idx_data, uint32_t pos_idx_stride, const glm::fmat4x4& pos_transform,
      CByteData nrm_data, uint32_t nrm_stride, CByteData nrm_idx_data, uint32_t nrm_idx_stride, const glm::fmat3x3& nrm_transform,
      CByteData tc0_data, uint32_t tc0_stride, CByteData tc0_idx_data, uint32_t tc0_idx_stride, const glm::fmat2x2& tc0_transform)
    {
      const auto create_vertex_fn = [idx_align,
        pos_transform, nrm_transform, tc0_transform,
        pos_data, pos_stride, pos_idx_data, pos_idx_stride,
        nrm_data, nrm_stride, nrm_idx_data, nrm_idx_stride,
        tc0_data, tc0_stride, tc0_idx_data, tc0_idx_stride]
        (size_t index)
        {
          const auto pos_idx =
            idx_align == 4 ? *(reinterpret_cast<const uint32_t*>(pos_idx_data.first + pos_idx_stride * index)) :
            idx_align == 2 ? *(reinterpret_cast<const uint16_t*>(pos_idx_data.first + pos_idx_stride * index)) :
            idx_align == 1 ? *(reinterpret_cast<const uint8_t*>(pos_idx_data.first + pos_idx_stride * index)) :
            -1;
          const auto pos_ptr = reinterpret_cast<const float*>(pos_data.first + pos_stride * pos_idx);
          const auto pos = glm::fvec3(pos_ptr[0], pos_ptr[1], pos_ptr[2]);

          const auto nrm_idx =
            idx_align == 4 ? *(reinterpret_cast<const uint32_t*>(nrm_idx_data.first + nrm_idx_stride * index)) :
            idx_align == 2 ? *(reinterpret_cast<const uint16_t*>(nrm_idx_data.first + nrm_idx_stride * index)) :
            idx_align == 1 ? *(reinterpret_cast<const uint8_t*>(nrm_idx_data.first + nrm_idx_stride * index)) :
            -1;
          const auto nrm_ptr = reinterpret_cast<const float*>(nrm_data.first + nrm_stride * nrm_idx);
          const auto nrm = glm::fvec3(nrm_ptr[0], nrm_ptr[1], nrm_ptr[2]);

          const auto tc0_idx =
            idx_align == 4 ? *(reinterpret_cast<const uint32_t*>(tc0_idx_data.first + tc0_idx_stride * index)) :
            idx_align == 2 ? *(reinterpret_cast<const uint16_t*>(tc0_idx_data.first + tc0_idx_stride * index)) :
            idx_align == 1 ? *(reinterpret_cast<const uint8_t*>(tc0_idx_data.first + tc0_idx_stride * index)) :
            -1;
          const auto tc0_ptr = reinterpret_cast<const float*>(tc0_data.first + tc0_stride * tc0_idx);
          const auto tc0 = glm::f32vec2(tc0_ptr[0], tc0_ptr[1]);

          Vertex vertex;

          vertex.pos = pos_transform * glm::f32vec4{ pos.x, pos.y, pos.z, 1.0f };
          vertex.nrm = nrm_transform * glm::f32vec3{ nrm.x, nrm.y, nrm.z };
          vertex.tc0 = tc0_transform * glm::f32vec2{ tc0.x, tc0.y };

          vertex.nrm = glm::normalize(vertex.nrm);

          return vertex;
        };

      const auto hash_vertex_fn =
        [](const Vertex& vertex)
        {
          const auto uref = reinterpret_cast<const glm::u32vec4*>(&vertex);

          auto hash = 0u;
          hash ^= std::hash<glm::u32vec4>()(uref[0]) + 0x9e3779b9 + (size_t(hash) << 6) + (hash >> 2);
          hash ^= std::hash<glm::u32vec4>()(uref[1]) + 0x9e3779b9 + (size_t(hash) << 6) + (hash >> 2);
          hash ^= std::hash<glm::u32vec4>()(uref[2]) + 0x9e3779b9 + (size_t(hash) << 6) + (hash >> 2);
          hash ^= std::hash<glm::u32vec4>()(uref[3]) + 0x9e3779b9 + (size_t(hash) << 6) + (hash >> 2);

          return hash;
        };

      const auto compare_vertex_fn = [](const Vertex& l, const Vertex& r) { return (memcmp(&l, &r, sizeof(Vertex)) == 0); };
      std::unordered_map<Vertex, uint32_t, decltype(hash_vertex_fn), decltype(compare_vertex_fn)> vertex_map(8, hash_vertex_fn, compare_vertex_fn);

      const auto remap_vertex_fn =
        [&vertex_map](std::vector<Vertex>& vertices, const Vertex& vertex)
        {
          const auto result = vertex_map.emplace(vertex, uint32_t(vertices.size()));
          if (result.second)
          {
            vertices.push_back(vertex);
          }
          return result.first->second;
        };



      for (size_t i = 0; i < idx_count / 3; ++i)
      {
        const auto vtx0 = create_vertex_fn(i * 3 + 0);
        aabb_min = glm::min(aabb_min, vtx0.pos);
        aabb_max = glm::max(aabb_max, vtx0.pos);

        const auto vtx1 = create_vertex_fn(i * 3 + 1);
        aabb_min = glm::min(aabb_min, vtx1.pos);
        aabb_max = glm::max(aabb_max, vtx1.pos);

        const auto vtx2 = create_vertex_fn(i * 3 + 2);
        aabb_min = glm::min(aabb_min, vtx2.pos);
        aabb_max = glm::max(aabb_max, vtx2.pos);

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
    }


    Mode::Mode(Scope& scope)
      : scope(scope)
    {
      atlas = xatlas::Create();
    }

    Mode::~Mode()
    {
      xatlas::Destroy(atlas);
    }



    void Mode::CalculateTangents(Geometry& geometry)
    {
      SMikkTSpaceInterface input = { 0 };

      input.m_getNumFaces = [](const SMikkTSpaceContext* ctx)
        {
          const auto geometry = reinterpret_cast<const Geometry*>(ctx->m_pUserData);
          return int32_t(geometry->triangles.size());
        };

      input.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* ctx, const int iFace)
        {
          const auto geometry = reinterpret_cast<const Geometry*>(ctx->m_pUserData);
          return 3;
        };

      input.m_getPosition = [](const SMikkTSpaceContext* ctx, float fvPosOut[], int iFace, int iVert)
        {
          const auto geometry = reinterpret_cast<const Geometry*>(ctx->m_pUserData);
          const auto idx = geometry->triangles[iFace].idx[iVert];
          const auto& pos = geometry->vertices[idx].pos;
          fvPosOut[0] = pos.x;
          fvPosOut[1] = pos.y;
          fvPosOut[2] = pos.z;
        };

      input.m_getNormal = [](const SMikkTSpaceContext* ctx, float fvNormOut[], int iFace, int iVert)
        {
          const auto geometry = reinterpret_cast<const Geometry*>(ctx->m_pUserData);
          const auto idx = geometry->triangles[iFace].idx[iVert];
          const auto& nrm = geometry->vertices[idx].nrm;
          fvNormOut[0] = nrm.x;
          fvNormOut[1] = nrm.y;
          fvNormOut[2] = nrm.z;
        };

      input.m_getTexCoord = [](const SMikkTSpaceContext* ctx, float fvTexcOut[], int iFace, int iVert)
        {
          const auto geometry = reinterpret_cast<const Geometry*>(ctx->m_pUserData);
          const auto idx = geometry->triangles[iFace].idx[iVert];
          const auto& tc0 = geometry->vertices[idx].tc0;
          fvTexcOut[0] = tc0.x;
          fvTexcOut[1] = tc0.y;
        };

      input.m_setTSpaceBasic = [](const SMikkTSpaceContext* ctx, const float fvTangent[], float fSign, int iFace, int iVert)
        {
          auto geometry = reinterpret_cast<Geometry*>(ctx->m_pUserData);
          const auto idx = geometry->triangles[iFace].idx[iVert];
          auto& tng = geometry->vertices[idx].tng;
          tng.x = fvTangent[0];
          tng.y = fvTangent[1];
          tng.z = fvTangent[2];
          auto& sgn = geometry->vertices[idx].sgn;
          sgn = fSign;
        };

      SMikkTSpaceContext context;
      context.m_pInterface = &input;
      context.m_pUserData = &geometry;

      BLAST_ASSERT(1 == genTangSpaceDefault(&context));
    }

    void Mode::CalculateMeshlets(Geometry& geometry)
    {

      meshopt_optimizeVertexCache((uint32_t*)geometry.triangles.data(), (const uint32_t*)geometry.triangles.data(), geometry.triangles.size() * 3, geometry.vertices.size());

      meshopt_optimizeOverdraw((uint32_t*)geometry.triangles.data(), (const uint32_t*)geometry.triangles.data(), geometry.triangles.size() * 3, (const float*)geometry.vertices.data(), geometry.vertices.size(), sizeof(Vertex), 1.0f);

      meshopt_optimizeVertexFetch((float*)geometry.vertices.data(), (uint32_t*)geometry.triangles.data(), geometry.triangles.size() * 3, (const float*)geometry.vertices.data(), geometry.vertices.size(), sizeof(Vertex));


      const size_t vrt_limit = 64;
      const size_t trg_limit = 128; // : 124;

      //// note: should be set to 0 unless cone culling is used at runtime!
      //const float cone_weight = flex ? -1.0f : 0.25f;
      //const float split_factor = flex ? 2.0f : 0.0f;

      //// note: input mesh is assumed to be optimized for vertex cache and vertex fetch
      //double start = timestamp();
      auto meshlet_count = meshopt_buildMeshletsBound(geometry.triangles.size() * 3, vrt_limit, trg_limit);
      std::vector<meshopt_Meshlet> meshlets_opt(meshlet_count);
      std::vector<uint32_t> meshlet_vrt(meshlet_count * vrt_limit);
      std::vector<uint8_t> meshlet_trg(meshlet_count * trg_limit * 3);

      //if (scan)
      //  meshlets.resize(meshopt_buildMeshletsScan(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size(), max_vertices, max_triangles));
      //else if (flex)
      //  meshlets.resize(meshopt_buildMeshletsFlex(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), max_vertices, min_triangles, max_triangles, cone_weight, split_factor));
      //else // note: equivalent to the call of buildMeshletsFlex() with non-negative cone_weight and split_factor = 0
      meshlets_opt.resize(meshopt_buildMeshlets(&meshlets_opt[0], &meshlet_vrt[0], &meshlet_trg[0],
        (const uint32_t*)geometry.triangles.data(), geometry.triangles.size() * 3, (const float*)geometry.vertices.data(), geometry.vertices.size(), sizeof(Vertex), vrt_limit, trg_limit, 0.0f));

      for (size_t i = 0; i < meshlets_opt.size(); ++i)
      {
        meshopt_optimizeMeshlet(&meshlet_vrt[meshlets_opt[i].vertex_offset], &meshlet_trg[meshlets_opt[i].triangle_offset], meshlets_opt[i].triangle_count, meshlets_opt[i].vertex_count);
      }

      if (meshlets_opt.size())
      {
        const meshopt_Meshlet& last = meshlets_opt.back();

        // this is an example of how to trim the vertex/triangle arrays when copying data out to GPU storage
        meshlet_vrt.resize(last.vertex_offset + last.vertex_count);
        meshlet_trg.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));
      }

      uint32_t max_vrt = 0;
      uint32_t max_trg = 0;

      for (size_t i = 0; i < meshlets_opt.size(); ++i)
      {
        //meshopt_optimizeMeshlet(&meshlet_vrt[meshlets_opt[i].vertex_offset], &meshlet_trg[meshlets_opt[i].triangle_offset], meshlets_opt[i].triangle_count, meshlets_opt[i].vertex_count);
      }

      geometry.meshlets.resize(meshlets_opt.size());
      for (auto i = 0ull; i < geometry.meshlets.size(); ++i)
      {
        geometry.meshlets[i].vrt_offset = meshlets_opt[i].vertex_offset;
        geometry.meshlets[i].vrt_count = meshlets_opt[i].vertex_count;
        geometry.meshlets[i].trg_offset = meshlets_opt[i].triangle_offset;
        geometry.meshlets[i].trg_count = meshlets_opt[i].triangle_count;
      }



      std::vector<int> boundary(geometry.vertices.size());

      for (const auto& meshlet : meshlets_opt)
      {
        geometry.meshopt_avg_vertices += meshlet.vertex_count;
        geometry.meshopt_avg_triangles += meshlet.triangle_count;
        geometry.meshopt_not_full += meshlet.triangle_count < trg_limit;

        for (uint32_t j = 0; j < meshlet.vertex_count; ++j)
        {
          const auto counter = boundary[meshlet_vrt[meshlet.vertex_offset + j]]++;
          geometry.meshopt_avg_boundary += counter == 2 ? 1 : 0;
        }

        std::array<int, vrt_limit> parents;
        for (auto j = 0u; j < meshlet.vertex_count; ++j)
        {
          parents[j] = int(j);
        }

        const auto follow = [&parents](int index)
          {
            while (index != parents[index])
            {
              const auto parent = parents[index];
              parents[index] = parents[parent];
              index = parent;
            }

            return index;
          };

        for (auto j = 0u; j < meshlet.triangle_count * 3; ++j)
        {
          const auto v0 = follow(meshlet_trg[meshlet.triangle_offset + j]);
          const auto v1 = follow(meshlet_trg[meshlet.triangle_offset + j + (j % 3 == 2 ? -2 : 1)]);

          parents[v0] = v1;
        }

        int roots = 0;
        for (auto j = 0u; j < meshlet.vertex_count; ++j)
        {
          roots += follow(j) == int(j);
        }

        assert(roots != 0);
        geometry.meshopt_avg_connected += roots;
      }
    }

    void Mode::CalculateAtlas(Geometry& geometry)
    {
      xatlas::MeshDecl mesh_decl;
      mesh_decl.vertexCount = geometry.vertices.size();
      mesh_decl.vertexPositionData = reinterpret_cast<const uint8_t*>(geometry.vertices.data()) + 0u;
      mesh_decl.vertexPositionStride = uint32_t(sizeof(Vertex));
      mesh_decl.vertexNormalData = reinterpret_cast<const uint8_t*>(geometry.vertices.data()) + 16u;
      mesh_decl.vertexNormalStride = uint32_t(sizeof(Vertex));
      mesh_decl.vertexUvData = reinterpret_cast<const uint8_t*>(geometry.vertices.data()) + 48u;
      mesh_decl.vertexUvStride = uint32_t(sizeof(Vertex));
      mesh_decl.indexCount = geometry.triangles.size() * 3u;
      mesh_decl.indexData = geometry.triangles.data();
      mesh_decl.indexFormat = xatlas::IndexFormat::UInt32;
      const auto res = xatlas::AddMesh(atlas, mesh_decl);
      BLAST_ASSERT(xatlas::AddMeshError::Success == res);
    }
  }
}