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
    Mode::Mode(Scope& scope)
      : scope(scope)
    {
    }

    Mode::~Mode()
    {
    }

    std::tuple<std::vector<Vertex>, std::vector<Triangle>, glm::fvec3, glm::fvec3> PopulateInstance(size_t idx_count, uint32_t idx_align,
      const glm::uvec3& idx_order, const glm::fmat4x4& pos_transform, const glm::fmat3x3& nrm_transform, const glm::fmat2x2& tc0_transform,
      std::pair<const uint8_t*, size_t> pos_data, uint32_t pos_stride, std::pair<const uint8_t*, size_t> pos_idx_data, uint32_t pos_idx_stride,
      std::pair<const uint8_t*, size_t> nrm_data, uint32_t nrm_stride, std::pair<const uint8_t*, size_t> nrm_idx_data, uint32_t nrm_idx_stride,
      std::pair<const uint8_t*, size_t> tc0_data, uint32_t tc0_stride, std::pair<const uint8_t*, size_t> tc0_idx_data, uint32_t tc0_idx_stride)
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

        for (size_t k = 0; k < idx_count / 3; ++k)
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
  }
}