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

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#define OBJ_TRIANGULATE

#include "obj_converter.h"

namespace RayGene3D
{
  namespace IO
  {
    OBJConverter::OBJConverter(const Scope& scope)
      : Mode(scope)
    {
      std::tuple<std::vector<Vertex>, std::vector<Triangle>, glm::fvec3, glm::fvec3> PopulateInstance(uint32_t idx_count, uint32_t idx_align,
        const glm::uvec3 & idx_order, const glm::fmat4x4 & pos_transform, const glm::fmat3x3 & nrm_transform, const glm::fmat2x2 & tc0_transform,
        std::pair<const uint8_t*, uint32_t> pos_data, uint32_t pos_stride, std::pair<const uint8_t*, uint32_t> pos_idx_data, uint32_t pos_idx_stride,
        std::pair<const uint8_t*, uint32_t> nrm_data, uint32_t nrm_stride, std::pair<const uint8_t*, uint32_t> nrm_idx_data, uint32_t nrm_idx_stride,
        std::pair<const uint8_t*, uint32_t> tc0_data, uint32_t tc0_stride, std::pair<const uint8_t*, uint32_t> tc0_idx_data, uint32_t tc0_idx_stride)
      {
        const auto create_vertex_fn = [idx_align,
          pos_transform, nrm_transform, tc0_transform,
          pos_data, pos_stride, pos_idx_data, pos_idx_stride,
          nrm_data, nrm_stride, nrm_idx_data, nrm_idx_stride,
          tc0_data, tc0_stride, tc0_idx_data, tc0_idx_stride]
          (uint32_t index)
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
        auto bb_max = glm::fvec3{ -FLT_MAX,-FLT_MAX,-FLT_MAX };

        auto degenerated_geom_tris_count = 0u;
        auto degenerated_wrap_tris_count = 0u;

        for (uint32_t k = 0; k < idx_count / 3; ++k)
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

      void OBJConverter::ImportOBJM()
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
        std::vector<std::string> textures_4_names;
        std::vector<std::string> textures_5_names;
        std::vector<std::string> textures_6_names;
        std::vector<std::string> textures_7_names;

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

            const auto lhs_transform = glm::fmat3x3(
              -1.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f,
              0.0f, 0.0f, 1.0f
            );
            const auto nrm_transform = conversion_from_rhs ? lhs_transform : glm::identity<glm::fmat3x3>();
            const auto pos_transform = nrm_transform * position_scale;

            const auto flip_v_tranform = glm::fmat2x2(
              1.0f, 0.0f,
              0.0f, -1.0f
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
            instance.bb_min = instance_bb_min;
            instance.geom_idx = uint32_t(instances.size());
            instance.bb_max = instance_bb_max;
            instance.brdf_idx = 0;

            const auto& obj_material = obj_materials[material_id.first];

            const auto debug = false;
            if (debug)
            {
              instance.brdf_param0 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
              instance.brdf_param1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
              instance.brdf_param2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
              instance.brdf_param3 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            }
            else
            {
              instance.brdf_param0 = glm::vec4(obj_material.emission[0], obj_material.emission[1], obj_material.emission[2], obj_material.illum);
              instance.brdf_param1 = glm::vec4(obj_material.diffuse[0], obj_material.diffuse[1], obj_material.diffuse[2], obj_material.dissolve);
              instance.brdf_param2 = glm::vec4(obj_material.specular[0], obj_material.specular[1], obj_material.specular[2], obj_material.shininess);
              instance.brdf_param3 = glm::vec4(obj_material.transmittance[0], obj_material.transmittance[1], obj_material.transmittance[2], obj_material.ior);

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

              if (obj_material.illum != 7) // glass material
              {
                instance.brdf_param3 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

            auto raw = Raw(tex_x * tex_y * uint32_t(sizeof(glm::u8vec4)));

            for (uint32_t i = 0; i < tex_x * tex_y; ++i)
            {
              const auto r = tex_n > 0 ? tex_data[i * tex_n + 0] : 0; //0xFF;
              const auto g = tex_n > 1 ? tex_data[i * tex_n + 1] : r; //0xFF;
              const auto b = tex_n > 2 ? tex_data[i * tex_n + 2] : r; //0xFF;
              const auto a = tex_n > 3 ? tex_data[i * tex_n + 3] : r; //0xFF;
              raw.SetElement<glm::u8vec4>({ r, g, b, a }, i);
            }
            stbi_image_free(tex_data);

            return std::make_tuple(std::move(raw), tex_x, tex_y);
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

        if (!textures_4_names.empty())
        {
          textures_4.resize(textures_4_names.size());
          for (uint32_t i = 0; i < uint32_t(textures_4_names.size()); ++i)
          {
            const auto& texture_4_name = textures_4_names[i];
            textures_4[i] = std::move(load_texture_fn(texture_4_name));
          }
        }

        if (!textures_5_names.empty())
        {
          textures_5.resize(textures_5_names.size());
          for (uint32_t i = 0; i < uint32_t(textures_5_names.size()); ++i)
          {
            const auto& texture_5_name = textures_5_names[i];
            textures_5[i] = std::move(load_texture_fn(texture_5_name));
          }
        }

        if (!textures_6_names.empty())
        {
          textures_6.resize(textures_6_names.size());
          for (uint32_t i = 0; i < uint32_t(textures_6_names.size()); ++i)
          {
            const auto& texture_6_name = textures_6_names[i];
            textures_6[i] = std::move(load_texture_fn(texture_6_name));
          }
        }

        if (!textures_7_names.empty())
        {
          textures_7.resize(textures_7_names.size());
          for (uint32_t i = 0; i < uint32_t(textures_7_names.size()); ++i)
          {
            const auto& texture_7_name = textures_7_names[i];
            textures_7[i] = std::move(load_texture_fn(texture_7_name));
          }
        }
      }
    }

    OBJConverter::~OBJConverter()
    {
    }
  }
}