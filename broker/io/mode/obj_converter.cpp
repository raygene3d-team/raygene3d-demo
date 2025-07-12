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

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_image_resize.h>

#define OBJ_TRIANGULATE
#include "obj_converter.h"

namespace RayGene3D
{
  namespace IO
  {
    void OBJConverter::Import()
    {
      tinyobj::attrib_t obj_attrib;
      std::vector<tinyobj::shape_t> obj_shapes;
      std::vector<tinyobj::material_t> obj_materials;

      std::string err, warn;
      BLAST_ASSERT(true == tinyobj::LoadObj(&obj_attrib, &obj_shapes, &obj_materials, &warn, &err, (scope.path_name + scope.file_name).c_str(), scope.path_name.c_str(), true));

      const auto texture_load_resize_fn = [](const std::string& path, const std::string& name,  uint32_t level)
      {
        int32_t src_extent_x = 0;
        int32_t src_extent_y = 0;
        int32_t src_channels = 0;
        unsigned char* src_data = stbi_load((path + name).c_str(), &src_extent_x, &src_extent_y, &src_channels, STBI_default);

        auto dst_extent_x = 1u << level - 1;
        auto dst_extent_y = 1u << level - 1;
        auto dst_channels = 4;
        auto raw = Raw(sizeof(glm::u8vec4) * dst_extent_x * dst_extent_y);
        auto dst_data = reinterpret_cast<uint8_t*>(raw.AccessBytes().first);

        stbir_resize_uint8(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, dst_channels);

        stbi_image_free(src_data);

        for (uint32_t i = 0; i < dst_extent_x * dst_extent_y; ++i)
        {
          const auto r = dst_channels > 0 ? dst_data[i * dst_channels + 0] : 0; //0xFF;
          const auto g = dst_channels > 1 ? dst_data[i * dst_channels + 1] : r; //0xFF;
          const auto b = dst_channels > 2 ? dst_data[i * dst_channels + 2] : r; //0xFF;
          const auto a = dst_channels > 3 ? dst_data[i * dst_channels + 3] : r; //0xFF;
          raw.SetElement<glm::u8vec4>({ r, g, b, a }, i);
        }
        
        return raw;
      };

      const auto pos_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.vertices.data()), uint32_t(obj_attrib.vertices.size()) };
      const auto pos_stride = 12;

      const auto nrm_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.normals.data()), uint32_t(obj_attrib.normals.size()) };
      const auto nrm_stride = 12;

      const auto tc0_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.texcoords.data()), uint32_t(obj_attrib.texcoords.size()) };
      const auto tc0_stride = 8;

      std::vector<std::string> texture_0_names;
      std::vector<std::string> texture_1_names;
      std::vector<std::string> texture_2_names;
      std::vector<std::string> texture_3_names;


      size_t vert_offset = 0;
      size_t prim_offset = 0;
      size_t mlet_offset = 0;
      size_t bone_offset = 0;

      for (size_t i = 0; i < obj_shapes.size(); ++i)
      {
        const auto& obj_mesh = obj_shapes[i].mesh;

        std::unordered_map<int, std::vector<glm::uvec3>> material_id_map;
        for (size_t j = 0; j < obj_mesh.material_ids.size(); ++j)
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

          const auto rhs_transform = glm::fmat3x3(
           -1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
          );
          const auto nrm_transform = scope.conversion_rhs ? rhs_transform : glm::identity<glm::fmat3x3>();
          const auto pos_transform = nrm_transform * scope.position_scale;

          const auto flip_v_tranform = glm::fmat2x2(
            1.0f, 0.0f,
            0.0f,-1.0f
          );
          const auto tc0_transform = true ? flip_v_tranform : glm::identity<glm::fmat2x2>();
          const auto idx_order = scope.conversion_rhs ? glm::uvec3{ 0u, 2u, 1u } : glm::uvec3{ 0u, 1u, 2u };

          const auto [vertices, triangles, aabb_min, aabb_max] = PopulateInstance(idx_count, idx_align,
            idx_order, pos_transform, nrm_transform, tc0_transform,
            pos_data, pos_stride, pos_idx_data, pos_idx_stride,
            nrm_data, nrm_stride, nrm_idx_data, nrm_idx_stride,
            tc0_data, tc0_stride, tc0_idx_data, tc0_idx_stride);

          if (vertices.empty() || triangles.empty()) continue;

          Instance instance;
          instance.transform = glm::identity<glm::fmat3x4>();
          instance.aabb_min = aabb_min;
          instance.geom_idx = uint32_t(scope.instances.size());
          instance.aabb_max = aabb_max;
          instance.brdf_idx = 0;

          const auto vert_count = vertices.size();
          const auto vert_stride = sizeof(Vertex);
          const auto vert_data = reinterpret_cast<const uint8_t*>(vertices.data());
          scope.buffer_0.Append(Raw({ vert_data, vert_stride * vert_count }));

          const auto prim_count = triangles.size();
          const auto prim_stride = sizeof(Triangle);
          const auto prim_data = reinterpret_cast<const uint8_t*>(triangles.data());
          scope.buffer_1.Append(Raw({ prim_data, prim_stride * prim_count }));

          const auto mlet_count = 0u;

          const auto bone_count = 0u;

          instance.offset_0 = vert_offset;
          instance.count_0 = vert_count;
          instance.offset_1 = prim_offset;
          instance.count_1 = prim_count;
          instance.offset_2 = bone_offset;
          instance.count_2 = bone_count;
          instance.offset_3 = mlet_offset;
          instance.count_3 = mlet_count;
          

          const auto& obj_material = obj_materials[material_id.first];

          const auto debug = false;
          if (debug)
          {
            instance.param_0 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            instance.param_1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            instance.param_2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            instance.param_3 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
          }
          else
          {
            instance.param_0 = glm::vec4(obj_material.emission[0], obj_material.emission[1], obj_material.emission[2], obj_material.illum);
            instance.param_1 = glm::vec4(obj_material.diffuse[0], obj_material.diffuse[1], obj_material.diffuse[2], obj_material.dissolve);
            instance.param_2 = glm::vec4(obj_material.specular[0], obj_material.specular[1], obj_material.specular[2], obj_material.shininess);
            instance.param_3 = glm::vec4(obj_material.transmittance[0], obj_material.transmittance[1], obj_material.transmittance[2], obj_material.ior);

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
            instance.layer_0 = texture_0_name.empty() ? uint32_t(-1) : tex_reindex_fn(texture_0_names, texture_0_name);
            const auto& texture_1_name = obj_material.alpha_texname;
            instance.layer_1 = texture_1_name.empty() ? uint32_t(-1) : tex_reindex_fn(texture_1_names, texture_1_name);
            const auto& texture_2_name = obj_material.specular_texname;
            instance.layer_2 = texture_2_name.empty() ? uint32_t(-1) : tex_reindex_fn(texture_2_names, texture_2_name);
            const auto& texture_3_name = obj_material.bump_texname;
            instance.layer_3 = texture_3_name.empty() ? uint32_t(-1) : tex_reindex_fn(texture_3_names, texture_3_name);

            if (obj_material.illum != 7) // glass material
            {
              instance.param_3 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
          }

          scope.instances.push_back(instance);
        }
      }


      for (size_t i = 0; i < texture_0_names.size(); ++i)
      {
        scope.array_0.Insert(i, texture_load_resize_fn(scope.path_name, texture_0_names[i], scope.texture_level));
      }

      for (size_t i = 0; i < texture_1_names.size(); ++i)
      {
        scope.array_1.Insert(i, texture_load_resize_fn(scope.path_name, texture_1_names[i], scope.texture_level));
      }

      for (size_t i = 0; i < texture_2_names.size(); ++i)
      {
        scope.array_2.Insert(i, texture_load_resize_fn(scope.path_name, texture_2_names[i], scope.texture_level));
      }

      for (size_t i = 0; i < texture_3_names.size(); ++i)
      {
        scope.array_3.Insert(i, texture_load_resize_fn(scope.path_name, texture_3_names[i], scope.texture_level));
      }

    }

    void OBJConverter::Export()
    {
    }

    OBJConverter::OBJConverter(Scope& scope)
      : Mode(scope)
    {
    }

    OBJConverter::~OBJConverter()
    {
    }
  }
}