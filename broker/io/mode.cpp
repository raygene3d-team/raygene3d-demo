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
    Mesh::Mesh(size_t idx_count, uint32_t idx_align, const glm::uvec3& idx_order,
      CByteData pos_data, uint32_t pos_stride, CByteData pos_idx_data, uint32_t pos_idx_stride, const glm::fmat4x4& pos_transform,
      CByteData nrm_data, uint32_t nrm_stride, CByteData nrm_idx_data, uint32_t nrm_idx_stride, const glm::fmat3x3& nrm_transform,
      CByteData tc0_data, uint32_t tc0_stride, CByteData tc0_idx_data, uint32_t tc0_idx_stride, const glm::fmat2x2& tc0_transform,
      uint32_t aaam_index, uint32_t snno_index, uint32_t eeet_index, uint32_t mask_index,
      const glm::f32vec4& param_0, const glm::f32vec4& param_1, const glm::f32vec4& param_2, const glm::f32vec4& param_3)
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

      material_param_0 = param_0;
      material_param_1 = param_1;
      material_param_2 = param_2;
      material_param_3 = param_3;

      aaam_texture_index = aaam_index;
      snno_texture_index = snno_index;
      eeet_texture_index = eeet_index;
      mask_texture_index = mask_index;
    }


    Mode::Mode(Scope& scope)
      : scope(scope)
    {
    }

    Mode::~Mode()
    {
    }



    void Mode::CalculateTangents()
    {
      for (auto& mesh : mesh_items)
      {
        SMikkTSpaceInterface input = { 0 };

        input.m_getNumFaces = [](const SMikkTSpaceContext* ctx)
          {
            const auto mesh = reinterpret_cast<const Mesh*>(ctx->m_pUserData);
            return int32_t(mesh->triangles.size());
          };

        input.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* ctx, const int iFace)
          {
            const auto mesh = reinterpret_cast<const Mesh*>(ctx->m_pUserData);
            return 3;
          };

        input.m_getPosition = [](const SMikkTSpaceContext* ctx, float fvPosOut[], int iFace, int iVert)
          {
            const auto mesh = reinterpret_cast<const Mesh*>(ctx->m_pUserData);
            const auto idx = mesh->triangles[iFace].idx[iVert];
            const auto& pos = mesh->vertices[idx].pos;
            fvPosOut[0] = pos.x;
            fvPosOut[1] = pos.y;
            fvPosOut[2] = pos.z;
          };

        input.m_getNormal = [](const SMikkTSpaceContext* ctx, float fvNormOut[], int iFace, int iVert)
          {
            const auto mesh = reinterpret_cast<const Mesh*>(ctx->m_pUserData);
            const auto idx = mesh->triangles[iFace].idx[iVert];
            const auto& nrm = mesh->vertices[idx].nrm;
            fvNormOut[0] = nrm.x;
            fvNormOut[1] = nrm.y;
            fvNormOut[2] = nrm.z;
          };

        input.m_getTexCoord = [](const SMikkTSpaceContext* ctx, float fvTexcOut[], int iFace, int iVert)
          {
            const auto mesh = reinterpret_cast<const Mesh*>(ctx->m_pUserData);
            const auto idx = mesh->triangles[iFace].idx[iVert];
            const auto& tc0 = mesh->vertices[idx].tc0;
            fvTexcOut[0] = tc0.x;
            fvTexcOut[1] = tc0.y;
          };

        input.m_setTSpaceBasic = [](const SMikkTSpaceContext* ctx, const float fvTangent[], float fSign, int iFace, int iVert)
          {
            auto mesh = reinterpret_cast<Mesh*>(ctx->m_pUserData);
            const auto idx = mesh->triangles[iFace].idx[iVert];
            auto& tng = mesh->vertices[idx].tng;
            tng.x = fvTangent[0];
            tng.y = fvTangent[1];
            tng.z = fvTangent[2];
            auto& sgn = mesh->vertices[idx].sgn;
            sgn = fSign;
          };

        SMikkTSpaceContext context = { 0 };
        context.m_pInterface = &input;
        context.m_pUserData = &mesh;

        BLAST_ASSERT(1 == genTangSpaceDefault(&context));
      }
    }

    void Mode::CalculateMeshlets()
    {
      for (auto& mesh : mesh_items)
      {
        meshopt_optimizeVertexCache((uint32_t*)mesh.triangles.data(), (const uint32_t*)mesh.triangles.data(),
          mesh.triangles.size() * 3, mesh.vertices.size());
        meshopt_optimizeOverdraw((uint32_t*)mesh.triangles.data(), (const uint32_t*)mesh.triangles.data(),
          mesh.triangles.size() * 3, (const float*)mesh.vertices.data(), mesh.vertices.size(), sizeof(Vertex), 1.0f);
        meshopt_optimizeVertexFetch((float*)mesh.vertices.data(), (uint32_t*)mesh.triangles.data(),
          mesh.triangles.size() * 3, (const float*)mesh.vertices.data(), mesh.vertices.size(), sizeof(Vertex));

        const size_t vrt_limit = 64;
        const size_t trg_limit = 128; // : 124;

        //// note: should be set to 0 unless cone culling is used at runtime!
        //const float cone_weight = flex ? -1.0f : 0.25f;
        //const float split_factor = flex ? 2.0f : 0.0f;

        //// note: input mesh is assumed to be optimized for vertex cache and vertex fetch
        //double start = timestamp();
        auto meshlet_count = meshopt_buildMeshletsBound(mesh.triangles.size() * 3, vrt_limit, trg_limit);
        std::vector<meshopt_Meshlet> meshlets_opt(meshlet_count);
        std::vector<uint32_t> meshlets_vrt(meshlet_count * vrt_limit);
        std::vector<uint8_t> meshlets_trg(meshlet_count * trg_limit * 3);

        //if (scan)
        //  meshlets.resize(meshopt_buildMeshletsScan(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size(), max_vertices, max_triangles));
        //else if (flex)
        //  meshlets.resize(meshopt_buildMeshletsFlex(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), max_vertices, min_triangles, max_triangles, cone_weight, split_factor));
        //else // note: equivalent to the call of buildMeshletsFlex() with non-negative cone_weight and split_factor = 0
        meshlets_opt.resize(meshopt_buildMeshlets(meshlets_opt.data(), 
          meshlets_vrt.data(), meshlets_trg.data(),
          (const uint32_t*)mesh.triangles.data(), mesh.triangles.size() * 3,
          (const float*)mesh.vertices.data(), mesh.vertices.size(), 
          sizeof(Vertex), vrt_limit, trg_limit, 0.0f));

        for (size_t i = 0; i < meshlets_opt.size(); ++i)
        {
          meshopt_optimizeMeshlet(&meshlets_vrt[meshlets_opt[i].vertex_offset], &meshlets_trg[meshlets_opt[i].triangle_offset], 
            meshlets_opt[i].triangle_count, meshlets_opt[i].vertex_count);
        }

        if (meshlets_opt.size())
        {
          const meshopt_Meshlet& last = meshlets_opt.back();

          // this is an example of how to trim the vertex/triangle arrays when copying data out to GPU storage
          meshlets_vrt.resize(last.vertex_offset + last.vertex_count);
          meshlets_trg.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));
        }

        uint32_t max_v_index = 0;
        for (const auto& vrt : meshlets_vrt) max_v_index = std::max(max_v_index, vrt);

        //meshlet_opt_count += meshlets_opt.size();

        //auto vertices = std::vector<Vertex>(std::accumulate(meshlets_opt.cbegin(), meshlets_opt.cend(), size_t(0), 
        //  [](size_t count, const auto& meshlet_opt) { return count + meshlet_opt.vertex_count; }));
        //auto triangles = std::vector<Triangle>(std::accumulate(meshlets_opt.cbegin(), meshlets_opt.cend(), size_t(0),
        //  [](size_t count, const auto& meshlet_opt) { return count + meshlet_opt.triangle_count; }));
        //auto meshlets = std::vector<Meshlet>(meshlets_opt.size());
        //auto points = std::vector<Point>(meshlet_trg.size());
        //
        //
        //uint32_t upd_triangle_offset = 0;
        //for (size_t i = 0; i < meshlets_opt.size(); ++i)
        //{      
        //  const auto color = glm::u8vec4
        //  {
        //    rand() % 64 + 63,
        //    rand() % 64 + 63,
        //    rand() % 64 + 63,
        //    255
        //  };

        //  for (size_t j = 0; j < meshlets_opt[i].vertex_count; ++j)
        //  {
        //    vertices[j + meshlets_opt[i].vertex_offset] = mesh.vertices[meshlet_vrt[j + meshlets_opt[i].vertex_offset]];
        //  }

        //  
        //  for (size_t j = 0; j < meshlets_opt[i].triangle_count; ++j)
        //  {
        //    const auto knt0 = meshlet_trg[3 * j + 0 + meshlets_opt[i].triangle_offset];
        //    const auto knt1 = meshlet_trg[3 * j + 1 + meshlets_opt[i].triangle_offset];
        //    const auto knt2 = meshlet_trg[3 * j + 2 + meshlets_opt[i].triangle_offset];

        //    points[3 * j + 0 + meshlets_opt[i].triangle_offset].idx = knt0;
        //    points[3 * j + 1 + meshlets_opt[i].triangle_offset].idx = knt1;
        //    points[3 * j + 2 + meshlets_opt[i].triangle_offset].idx = knt2;

        //    const auto idx0 = knt0 + meshlets_opt[i].vertex_offset;
        //    const auto idx1 = knt1 + meshlets_opt[i].vertex_offset;
        //    const auto idx2 = knt2 + meshlets_opt[i].vertex_offset;

        //    triangles[j + 0 + upd_triangle_offset].idx = { idx0, idx1, idx2 };

        //    vertices[idx0].col = color;
        //    vertices[idx1].col = color;
        //    vertices[idx2].col = color;            
        //  }

        //  meshlets[i].vrt_offset = meshlets_opt[i].vertex_offset;
        //  meshlets[i].vrt_count = meshlets_opt[i].vertex_count;
        //  meshlets[i].pnt_offset = meshlets_opt[i].triangle_offset;
        //  meshlets[i].pnt_count = meshlets_opt[i].triangle_count * 3;

        //  upd_triangle_offset += meshlets_opt[i].triangle_count;
        //}

        mesh.meshlets.resize(meshlets_opt.size());
        for (size_t i = 0; i < meshlets_opt.size(); ++i)
        { 
          mesh.meshlets[i].vidx_offset = meshlets_opt[i].vertex_offset;
          mesh.meshlets[i].vidx_count = meshlets_opt[i].vertex_count;
          mesh.meshlets[i].tidx_offset = meshlets_opt[i].triangle_offset;
          mesh.meshlets[i].tidx_count = meshlets_opt[i].triangle_count;


        }

        std::swap(mesh.v_indices, meshlets_vrt);
        std::swap(mesh.t_indices, meshlets_trg);


        //std::swap(mesh.meshlets, meshlets);
        //std::swap(mesh.points, points);

        //std::vector<int> boundary(mesh.vertices.size());

        //for (const auto& meshlet : meshlets_opt)
        //{
        //  mesh.meshopt_avg_vertices += meshlet.vertex_count;
        //  mesh.meshopt_avg_triangles += meshlet.triangle_count;
        //  mesh.meshopt_not_full += meshlet.triangle_count < trg_limit;

        //  for (uint32_t j = 0; j < meshlet.vertex_count; ++j)
        //  {
        //    const auto counter = boundary[meshlets_vrt[meshlet.vertex_offset + j]]++;
        //    mesh.meshopt_avg_boundary += counter == 2 ? 1 : 0;
        //  }

        //  std::array<int, vrt_limit> parents;
        //  for (auto j = 0u; j < meshlet.vertex_count; ++j)
        //  {
        //    parents[j] = int(j);
        //  }

        //  const auto follow = [&parents](int index)
        //    {
        //      while (index != parents[index])
        //      {
        //        const auto parent = parents[index];
        //        parents[index] = parents[parent];
        //        index = parent;
        //      }

        //      return index;
        //    };

        //  for (auto j = 0u; j < meshlet.triangle_count * 3; ++j)
        //  {
        //    const auto v0 = follow(meshlets_trg[meshlet.triangle_offset + j]);
        //    const auto v1 = follow(meshlets_trg[meshlet.triangle_offset + j + (j % 3 == 2 ? -2 : 1)]);

        //    parents[v0] = v1;
        //  }

        //  int roots = 0;
        //  for (auto j = 0u; j < meshlet.vertex_count; ++j)
        //  {
        //    roots += follow(j) == int(j);
        //  }

        //  assert(roots != 0);
        //  mesh.meshopt_avg_connected += roots;
        //}
      }
    }

    void Mode::CalculateAtlas()
    {
      const uint32_t resolution = 2048;
      const float texel_density = 40.0;

      const auto xatlas = xatlas::Create();

      for (auto& mesh : mesh_items)
      {
        xatlas::MeshDecl mesh_decl;
        mesh_decl.vertexCount = mesh.vertices.size();
        mesh_decl.vertexPositionData = reinterpret_cast<const uint8_t*>(mesh.vertices.data()) + 0u;
        mesh_decl.vertexPositionStride = uint32_t(sizeof(Vertex));
        mesh_decl.vertexNormalData = reinterpret_cast<const uint8_t*>(mesh.vertices.data()) + 16u;
        mesh_decl.vertexNormalStride = uint32_t(sizeof(Vertex));
        mesh_decl.vertexUvData = reinterpret_cast<const uint8_t*>(mesh.vertices.data()) + 48u;
        mesh_decl.vertexUvStride = uint32_t(sizeof(Vertex));
        mesh_decl.indexCount = mesh.triangles.size() * 3u;
        mesh_decl.indexData = mesh.triangles.data();
        mesh_decl.indexFormat = xatlas::IndexFormat::UInt32;
        const auto res = xatlas::AddMesh(xatlas, mesh_decl);
        BLAST_ASSERT(xatlas::AddMeshError::Success == res);
      }

      xatlas::ChartOptions chartOptions;
      chartOptions.useInputMeshUvs = true;
      xatlas::ComputeCharts(xatlas, chartOptions);

      xatlas::PackOptions packOptions;
      packOptions.resolution = resolution;
      packOptions.texelsPerUnit = texel_density;
      xatlas::PackCharts(xatlas, packOptions);

      for (uint32_t i = 0; i < xatlas->meshCount; ++i)
      {
        const auto& xmesh = xatlas->meshes[i];

        std::vector<Vertex> vertices(xmesh.vertexCount);
        for (uint32_t j = 0; j < xmesh.vertexCount; j++)
        {
          const auto& xvertex = xmesh.vertexArray[j];

          vertices[j] = mesh_items[i].vertices[xvertex.xref];
          vertices[j].msk = xvertex.atlasIndex;
          vertices[j].tc1 = { xvertex.uv[0] / xatlas->width, xvertex.uv[1] / xatlas->height };
        }
        std::swap(mesh_items[i].vertices, vertices);

        std::vector<Triangle> triangles(xmesh.indexCount / 3);
        for (uint32_t j = 0; j < xmesh.indexCount / 3; j++)
        {
          const auto idx0 = xmesh.indexArray[j * 3 + 0];
          const auto idx1 = xmesh.indexArray[j * 3 + 1];
          const auto idx2 = xmesh.indexArray[j * 3 + 2];

          triangles[j] = mesh_items[i].triangles[j];
          triangles[j].idx = { idx0, idx1, idx2 };
        }
        std::swap(mesh_items[i].triangles, triangles);

        for (uint32_t j = 0; j < xmesh.chartCount; ++j)
        {
          const auto color = glm::u8vec4
          {
            rand() % 64 + 63,
            rand() % 64 + 63,
            rand() % 64 + 63,
            255
          };

          const auto& xchart = xmesh.chartArray[j];
          for (uint32_t k = 0; k < xchart.faceCount; ++k)
          {
            const auto xface = xchart.faceArray[k];

            const auto idx_0 = xmesh.indexArray[3 * xface + 0];
            auto& vtx_0 = mesh_items[i].vertices[idx_0];
            vtx_0.col = color;

            const auto idx_1 = xmesh.indexArray[3 * xface + 1];
            auto& vtx_1 = mesh_items[i].vertices[idx_1];
            vtx_1.col = color;

            const auto idx_2 = xmesh.indexArray[3 * xface + 2];
            auto& vtx_2 = mesh_items[i].vertices[idx_2];
            vtx_2.col = color;
          }
        }
      }

      xatlas::Destroy(xatlas);
    }


    void static RecursiveBuild(std::vector<Box>& leaves, size_t begin, size_t end, std::vector<Box>& nodes, uint32_t depth)
    {
      Box node;

      for (size_t i = begin; i < end; ++i)
      {
        const auto& leaf = leaves[i];

        node.min = glm::min(node.min, leaf.min);
        node.max = glm::max(node.max, leaf.max);
      }

      const auto node_center = 0.5f * (node.max + node.min);
      const auto node_extent = 0.5f * (node.max - node.min);
      const auto node_area = 4.0f * std::abs(node_extent.x * node_extent.y + node_extent.y * node_extent.z + node_extent.z * node_extent.x);

      node.offset = begin;
      node.count = 2 * (end - begin) - 1;
      nodes.emplace_back(node);

      if (end - begin > 1)
      {
        Box area;

        for (size_t i = begin; i < end; ++i)
        {
          const auto& leaf = leaves[i];

          const auto leaf_center = 0.5f * (leaf.max + leaf.min);
          const auto leaf_extent = 0.5f * (leaf.max - leaf.min);

          area.min = glm::min(area.min, leaf_center);
          area.max = glm::max(area.max, leaf_center);
        }

        const auto area_center = 0.5f * (area.max + area.min);
        const auto area_extent = 0.5f * (area.max - area.min);

        const auto area_norm = ((area_extent.y > area_extent.x) || (area_extent.z > area_extent.x)) ? area_extent.z > area_extent.y ? area_extent.z : area_extent.y : area_extent.x;
        const auto axis = /* (depth + 0) % 3; */ ((area_extent.y > area_extent.x) || (area_extent.z > area_extent.x)) ? area_extent.z > area_extent.y ? 2 : 1 : 0;
        //nodes[nodes.size() - 1].flags = axis;

        std::stable_sort(&leaves[begin], &leaves[end - 1] + 1, [axis](const Box& a, const Box& b)
          {
            return (a.max[axis] + a.min[axis]) > (b.max[axis] + b.min[axis]);
          });
        auto median = (begin + end) / 2;


        bool use_sah = true;
        if (use_sah && (end - begin > 2))
        {
          constexpr auto bucket_count = 16;
          const auto bucket_stride = (leaves[end - 1].count - leaves[begin].count) / bucket_count + 1;

          std::array<Box, bucket_count> bucket_array;
          for (size_t i = begin; i < end; ++i)
          {
            const auto& leaf = leaves[i];

            const auto leaf_center = 0.5f * (leaf.max + leaf.min);
            const auto leaf_extent = 0.5f * (leaf.max - leaf.min);

            const auto leaf_norm = 0.5f * (leaf_center - area.min) / area_extent;
            const auto leaf_index = int(leaf_norm[axis] * bucket_count);
            auto& leaf_bucket = bucket_array[std::max(0, std::min(bucket_count - 1, leaf_index))];

            leaf_bucket.count += 1;
            leaf_bucket.min = glm::min(leaf_bucket.min, leaf.min);
            leaf_bucket.max = glm::max(leaf_bucket.max, leaf.max);
          }

          std::array<float, bucket_count - 1> cost_array;
          for (size_t i = 0; i < cost_array.size(); ++i)
          {
            Box box0;
            for (size_t j = 0; j <= i; ++j)
            {
              const auto& bucket = bucket_array[j];
              box0.min = glm::min(box0.min, bucket.min);
              box0.max = glm::max(box0.max, bucket.max);
              box0.count += bucket.count;
            }
            const auto extent0 = 0.5f * (box0.max - box0.min);
            const auto area0 = std::min(FLT_MAX, 4.0f * std::abs(extent0.x * extent0.y + extent0.y * extent0.z + extent0.z * extent0.x));

            Box box1;
            for (size_t j = i + 1; j < cost_array.size(); ++j)
            {
              const auto& bucket = bucket_array[j];
              box1.min = glm::min(box1.min, bucket.min);
              box1.max = glm::max(box1.max, bucket.max);
              box1.count += bucket.count;
            }
            const auto extent1 = 0.5f * (box1.max - box1.min);
            const auto area1 = std::min(FLT_MAX, 4.0f * std::abs(extent1.x * extent1.y + extent1.y * extent1.z + extent1.z * extent1.x));

            cost_array[i] = 1.0f + (box0.count * area0 + box1.count * area1) / node_area;
          }

          const auto median_index = std::distance(cost_array.begin(), std::min_element(cost_array.begin(), cost_array.end()));

          if (cost_array[median_index] < end - begin)
          {
            const auto median_leaf = std::stable_partition(&leaves[begin], &leaves[end - 1] + 1, [&](const Box& leaf)
              {
                const auto leaf_center = 0.5f * (leaf.max + leaf.min);
                const auto leaf_extent = 0.5f * (leaf.max - leaf.min);

                const auto leaf_norm = 0.5f * (leaf_center - area.min) / area_extent;
                const auto leaf_index = int(leaf_norm[axis] * bucket_count);

                return leaf_index > median_index;
              });
            median = size_t(median_leaf - &leaves[0]);
          }
        }

        RecursiveBuild(leaves, begin, median, nodes, depth + 1);
        RecursiveBuild(leaves, median, end, nodes, depth + 1);
      }
    }

    void Mode::CalculateBoxes()
    {
      for (auto& mesh : mesh_items)
      {
        std::vector<Box> triangle_leaves(mesh.triangles.size());
        for (size_t j = 0; j < triangle_leaves.size(); ++j)
        {
          const auto& triangle = mesh.triangles[j];
          const auto& vertex0 = mesh.vertices[triangle.idx[0]];
          const auto& vertex1 = mesh.vertices[triangle.idx[1]];
          const auto& vertex2 = mesh.vertices[triangle.idx[2]];

          auto& leaf = triangle_leaves[j];
          leaf.offset = j;
          leaf.count = 1;
          leaf.min = glm::min(vertex0.pos, glm::min(vertex1.pos, vertex2.pos));
          leaf.max = glm::max(vertex0.pos, glm::max(vertex1.pos, vertex2.pos));
        }

        std::vector<Box> nodes;
        RecursiveBuild(triangle_leaves, 0u, triangle_leaves.size(), nodes, 0);

        uint32_t counter = 0;
        for (uint32_t j = 0; j < nodes.size(); ++j)
        {
          auto& node = nodes[j];
          if (node.count > 1) continue;

          node.offset = triangle_leaves[node.offset].offset;
          ++counter;
        }
        BLAST_ASSERT(counter == triangle_leaves.size());

        for (auto& node : nodes)
        {
          if (node.count == 1)
          {
            const auto& triangle = mesh.triangles[node.offset];
            const auto& vertex0 = mesh.vertices[triangle.idx[0]];
            const auto& vertex1 = mesh.vertices[triangle.idx[1]];
            const auto& vertex2 = mesh.vertices[triangle.idx[2]];

            auto center = (vertex0.pos + vertex1.pos + vertex2.pos) / 3.0f;
            center.x = glm::unpackHalf2x16(glm::packHalf2x16(glm::f32vec2(center.x, center.x))).x;
            center.y = glm::unpackHalf2x16(glm::packHalf2x16(glm::f32vec2(center.y, center.y))).x;
            center.z = glm::unpackHalf2x16(glm::packHalf2x16(glm::f32vec2(center.z, center.z))).x;

            const auto delta0 = (vertex0.pos - center);
            const auto delta1 = (vertex1.pos - center);
            const auto delta2 = (vertex2.pos - center);

            glm::uvec3 packed_min;
            packed_min.x = glm::packHalf2x16(glm::f32vec2(center.x, delta0.x));
            packed_min.y = glm::packHalf2x16(glm::f32vec2(center.y, delta0.y));
            packed_min.z = glm::packHalf2x16(glm::f32vec2(center.z, delta0.z));
            memcpy(&node.min, &packed_min, 3 * sizeof(uint32_t));

            glm::uvec3 packed_max;
            packed_max.x = glm::packHalf2x16(glm::f32vec2(delta1.x, delta2.x));
            packed_max.y = glm::packHalf2x16(glm::f32vec2(delta1.y, delta2.y));
            packed_max.z = glm::packHalf2x16(glm::f32vec2(delta1.z, delta2.z));
            memcpy(&node.max, &packed_max, 3 * sizeof(uint32_t));
          }
        }
        std::swap(mesh.boxes, nodes);
      }



      std::vector<Box> instance_leaves(mesh_items.size());
      for (size_t i = 0; i < instance_leaves.size(); ++i)
      {
        const auto& mesh = mesh_items[i];

        auto& leaf = instance_leaves[i];
        leaf.offset = i;
        leaf.count = 1;
        leaf.min = mesh.aabb_min;
        leaf.max = mesh.aabb_max;
      }

      std::vector<Box> nodes;
      RecursiveBuild(instance_leaves, 0, instance_leaves.size(), nodes, 0);

      uint32_t counter = 0;
      for (uint32_t i = 0; i < nodes.size(); ++i)
      {
        auto& node = nodes[i];
        if (node.count > 1) continue;

        node.offset = instance_leaves[node.offset].offset;
        ++counter;
      }
      BLAST_ASSERT(counter == instance_leaves.size())

        for (auto& node : nodes)
        {
          if (node.count == 1)
          {
            const auto center = 0.5f * (node.max + node.min);
            const auto extent = 0.5f * (node.max - node.min);

            glm::uvec3 packed_min;
            packed_min.x = glm::packHalf2x16(glm::f32vec2(center.x, extent.x));
            packed_min.y = glm::packHalf2x16(glm::f32vec2(center.y, extent.y));
            packed_min.z = glm::packHalf2x16(glm::f32vec2(center.z, extent.z));
            memcpy(&node.min, &packed_min, 3 * sizeof(uint32_t));

            glm::uvec3 packed_max;
            packed_max.x = glm::packHalf2x16(glm::f32vec2(extent.x, center.x));
            packed_max.y = glm::packHalf2x16(glm::f32vec2(extent.y, center.y));
            packed_max.z = glm::packHalf2x16(glm::f32vec2(extent.z, center.z));
            memcpy(&node.max, &packed_max, 3 * sizeof(uint32_t));
          }
        }

      std::swap(boxes, nodes);
    }
  }
}