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

      const auto pos_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.vertices.data()), uint32_t(obj_attrib.vertices.size()) };
      const auto pos_stride = 12;

      const auto nrm_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.normals.data()), uint32_t(obj_attrib.normals.size()) };
      const auto nrm_stride = 12;

      const auto tc0_data = std::pair{ reinterpret_cast<const uint8_t*>(obj_attrib.texcoords.data()), uint32_t(obj_attrib.texcoords.size()) };
      const auto tc0_stride = 8;

      //std::vector<std::string> textures_0_names;
      //std::vector<std::string> textures_1_names;
      //std::vector<std::string> textures_2_names;
      //std::vector<std::string> textures_3_names;
      //std::vector<std::string> textures_4_names;
      //std::vector<std::string> textures_5_names;
      //std::vector<std::string> textures_6_names;
      //std::vector<std::string> textures_7_names;

      std::vector<std::string> texture_names;

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

          instance.buffer0_idx = uint32_t(scope.buffers_0.size());
          {
            const auto stride = sizeof(Vertex);
            const auto count = vertices.size();
            const auto data = reinterpret_cast<const uint8_t*>(vertices.data());
            scope.buffers_0.emplace_back(Raw({data, stride * count }), stride, count);
          }

          instance.buffer1_idx = uint32_t(scope.buffers_1.size());
          {
            const auto stride = sizeof(Triangle);
            const auto count = triangles.size();
            const auto data = reinterpret_cast<const uint8_t*>(triangles.data());
            scope.buffers_1.emplace_back(Raw({ data, stride * count }), stride, count);
          }
          

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
            instance.texture0_idx = texture_0_name.empty() ? uint32_t(-1) : tex_reindex_fn(texture_names, texture_0_name);
            const auto& texture_1_name = obj_material.alpha_texname;
            instance.texture1_idx = texture_1_name.empty() ? uint32_t(-1) : tex_reindex_fn(texture_names, texture_1_name);
            const auto& texture_2_name = obj_material.specular_texname;
            instance.texture2_idx = texture_2_name.empty() ? uint32_t(-1) : tex_reindex_fn(texture_names, texture_2_name);
            const auto& texture_3_name = obj_material.bump_texname;
            instance.texture3_idx = texture_3_name.empty() ? uint32_t(-1) : tex_reindex_fn(texture_names, texture_3_name);

            if (obj_material.illum != 7) // glass material
            {
              instance.brdf_param3 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
          }

          scope.instances.push_back(instance);
        }
      }


      const auto load_texture_fn = [this](const std::string& file_name)
      {
        int32_t tex_x = 0;
        int32_t tex_y = 0;
        int32_t tex_n = 0;
        unsigned char* tex_data = stbi_load((scope.path_name + scope.file_name).c_str(), &tex_x, &tex_y, &tex_n, STBI_default);

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

        return std::make_tuple(std::move(raw), uint32_t(tex_x), uint32_t(tex_y));
      };

      //if (!texture_names.empty())
      //{
      //  scope.textures.resize(texture_names.size());
      //  for (size_t i = 0; i < texture_names.size(); ++i)
      //  {
      //    scope.textures[i] = load_texture_fn(texture_names[i]);
      //  }
      //}
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