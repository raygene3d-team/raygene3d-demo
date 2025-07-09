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

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tinygltf/tiny_gltf.h>

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_image_resize.h>

#include "gltf_converter.h"

namespace RayGene3D
{
  namespace IO
  {
    void GLTFConverter::Import()
    {
      tinygltf::TinyGLTF gltf_ctx;

      tinygltf::Model gltf_model;
      std::string err, warn;
      BLAST_ASSERT(gltf_ctx.LoadASCIIFromFile(&gltf_model, &err, &warn, (scope.path_name + scope.file_name).c_str()));

      const auto gltf_scene = gltf_model.scenes[0];
      std::stack<int, std::vector<int>> node_indices(gltf_scene.nodes);

      std::map<int, glm::fmat4x4> mesh_relations;

      auto parse_node_hierarchy = [&gltf_model, &mesh_relations](auto&& parse_node_hierarchy, const tinygltf::Node& node, glm::fmat4x4 parent_transform)
        {
          auto transform = glm::identity<glm::fmat4x4>();


          if (node.translation.size() == 3)
          {
            const auto translation = glm::fvec3(node.translation[0], node.translation[1], node.translation[2]);
            transform = glm::translate(transform, translation);
          }

          if (node.rotation.size() == 4)
          {
            const auto rotation = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
            transform = transform * glm::fmat4x4(rotation);
          }

          if (node.scale.size() == 3)
          {
            const auto scaling = glm::f32vec3(node.scale[0], node.scale[1], node.scale[2]);
            transform = glm::scale(transform, scaling);
          }

          if (node.matrix.size() == 16) {
            const auto matrix = glm::fmat4x4(
              node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
              node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
              node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
              node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]);
            transform = transform * matrix;
          };

          transform = parent_transform * transform;

          if (node.mesh != -1)
          {
            mesh_relations.insert({ node.mesh, transform });
          }

          if (node.children.size() == 0)
          {
            return;
          }

          for (const auto& child_index : node.children)
          {
            parse_node_hierarchy(parse_node_hierarchy, gltf_model.nodes[child_index], transform);
          }
        };

      for (auto node_index : gltf_scene.nodes)
      {
        const auto& node = gltf_model.nodes[node_index];
        parse_node_hierarchy(parse_node_hierarchy, node, glm::identity<glm::fmat4x4>());
      }





      //std::vector<Vertex> structures_0;
      //std::vector<Triangle> structures_1;

      const auto geometry_convert_fn = [&gltf_model, this]
        (const tinygltf::Primitive& gltf_primitive, const glm::fmat4x4 transform, float scale, bool to_lhs, bool flip_v)
        {
          BLAST_ASSERT(gltf_primitive.mode == TINYGLTF_MODE_TRIANGLES);

          const auto access_buffer_fn = [&gltf_model](const tinygltf::Accessor& accessor)
            {
              const auto& gltf_view = gltf_model.bufferViews[accessor.bufferView];
              const auto length = gltf_view.byteLength;
              const auto offset = gltf_view.byteOffset;

              const auto& gltf_buffer = gltf_model.buffers[gltf_view.buffer];
              const auto data = &gltf_buffer.data[accessor.byteOffset];

              return std::pair{ data + offset, length };
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

          const auto lhs_transform = glm::fmat3x3(
           -1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
          );

          const auto mdf_transform = to_lhs ? lhs_transform : glm::identity<glm::fmat3x3>();
          const auto nrm_transform = mdf_transform * glm::fmat3x3(transform);
          const auto pos_transform = glm::fmat4x4(mdf_transform * scale) * transform;

          const auto flip_v_tranform = glm::fmat2x2(
            0.0f, 1.0f,
            1.0f, 0.0f
          );
          const auto tc0_transform = flip_v ? flip_v_tranform : glm::identity<glm::fmat2x2>();
          const auto idx_order = to_lhs ? glm::uvec3{ 0u, 2u, 1u } : glm::uvec3{ 0u, 1u, 2u };

          auto degenerated_geom_tris_count = 0u;
          auto degenerated_wrap_tris_count = 0u;
          const auto [vert_structures, prim_structures, aabb_min, aabb_max] = PopulateInstance(idx_count, idx_stride,
            idx_order, pos_transform, nrm_transform, tc0_transform,
            pos_data, pos_stride, idx_data, idx_stride,
            nrm_data, nrm_stride, idx_data, idx_stride,
            tc0_data, tc0_stride, idx_data, idx_stride);

          const auto vert_offset = scope.buffer_0.Length();
          const auto vert_count = vert_structures.size();
          const auto vert_stride = sizeof(Vertex);
          const auto vert_data = reinterpret_cast<const uint8_t*>(vert_structures.data());
          scope.buffer_0.Append(Raw({ vert_data, vert_stride * vert_count }));

          const auto prim_offset = scope.buffer_1.Length();
          const auto prim_count = prim_structures.size();
          const auto prim_stride = sizeof(Triangle);
          const auto prim_data = reinterpret_cast<const uint8_t*>(prim_structures.data());
          scope.buffer_1.Append(Raw({ prim_data, prim_stride * prim_count }));

          const auto bone_offset = 0u;
          const auto bone_count = 0u;

          const auto mlet_offset = 0u;
          const auto mlet_count = 0u;

          return std::make_tuple(
            vert_offset, vert_count,
            prim_offset, prim_count,
            bone_offset, bone_count,
            mlet_offset, mlet_count,
            aabb_min, aabb_max);
        };



      auto texture_cache = std::vector<Raw>();
      for (const auto& image : gltf_model.images)
      {
        const auto src_extent_x = image.width;
        const auto src_extent_y = image.height;
        const auto src_channels = image.component;
        const auto src_data = image.image.data();

        auto dst_extent_x = 1u << scope.texture_level - 1;
        auto dst_extent_y = 1u << scope.texture_level - 1;
        auto raw = Raw(sizeof(glm::u8vec4) * dst_extent_x * dst_extent_y);
        auto dst_data = reinterpret_cast<uint8_t*>(raw.AccessBytes().first);

        stbir_resize_uint8(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, 4);
        texture_cache.push_back(std::move(raw));
      }

      std::unordered_map<glm::u32vec4, uint32_t> texture_0_indices;
      std::unordered_map<glm::u32vec4, uint32_t> texture_1_indices;
      const auto material_convert_fn = [&gltf_model, this, &texture_cache, &texture_0_indices, &texture_1_indices]
      (const tinygltf::Primitive& gltf_primitive, uint32_t texture_level)
        {
          const auto extent_x = 1u << texture_level - 1;
          const auto extent_y = 1u << texture_level - 1;

          const auto repack_texture_0_fn = [&gltf_model, this, &texture_cache, &texture_0_indices, extent_x, extent_y]
          (const tinygltf::Primitive& gltf_primitive)
            {
              const auto& gltf_material = gltf_model.materials[gltf_primitive.material];

              const auto key = glm::u32vec4(
                gltf_material.pbrMetallicRoughness.baseColorTexture.index,
                gltf_material.pbrMetallicRoughness.baseColorTexture.index,
                gltf_material.pbrMetallicRoughness.baseColorTexture.index,
                gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index
              );

              const auto res = texture_0_indices.find(key);
              if (res != texture_0_indices.cend()) return res->second;

              auto raw = Raw(sizeof(glm::u8vec4) * extent_x * extent_y);
              for (size_t i = 0; size_t(extent_x * extent_y); ++i)
              {
                raw.SetElement<glm::u8vec4>(glm::u8vec4(
                  key[0] == -1 ? 255u : texture_cache[key[0]].GetElement<glm::u8vec4>(i).r,
                  key[1] == -1 ? 255u : texture_cache[key[1]].GetElement<glm::u8vec4>(i).g,
                  key[2] == -1 ? 255u : texture_cache[key[2]].GetElement<glm::u8vec4>(i).b,
                  key[3] == -1 ? 000u : texture_cache[key[3]].GetElement<glm::u8vec4>(i).r),
                  i);
              }
              scope.array_0.Insert(texture_0_indices.size(), std::move(raw));
              texture_0_indices[key] = texture_0_indices.size();

              return uint32_t(texture_0_indices.size()) - 1;
            };

          const auto repack_texture_1_fn = [&gltf_model, &texture_cache, &texture_1_proxy, extent_x, extent_y]
          (const tinygltf::Primitive& gltf_primitive)
            {
              const auto& gltf_material = gltf_model.materials[gltf_primitive.material];

              const auto key = glm::u32vec4(
                gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index,
                gltf_material.normalTexture.index,
                gltf_material.normalTexture.index,
                gltf_material.occlusionTexture.index
              );

              for (size_t i = 0; i < textures_1.size(); ++i) { if (textures_1[i].first == key) return i; }

              auto raw = Raw(sizeof(glm::u8vec4) * extent_x * extent_y);
              for (size_t i = 0; size_t(extent_x * extent_y); ++i)
              {
                raw.SetElement<glm::u8vec4>(glm::u8vec4(
                  key[0] == -1 ? 0u : texture_cache[key[0]].GetElement<glm::u8vec4>(i).g,
                  key[1] == -1 ? 0u : texture_cache[key[1]].GetElement<glm::u8vec4>(i).r,
                  key[2] == -1 ? 0u : texture_cache[key[2]].GetElement<glm::u8vec4>(i).g,
                  key[3] == -1 ? 0u : texture_cache[key[3]].GetElement<glm::u8vec4>(i).r),
                  i);
              }
              textures_1.emplace_back(key, std::move(raw));

              return textures_1.size() - 1;
            };

          const auto tex_0_idx = repack_texture_0_fn(gltf_primitive);
          const auto tex_1_idx = repack_texture_0_fn(gltf_primitive);
          const auto tex_2_idx = -1;
          const auto tex_3_idx = -1;

          const auto param_0 = glm::f32vec4(0.0f, 0.0f, 0.0f, 0.0f);
          const auto param_1 = glm::f32vec4(0.0f, 0.0f, 0.0f, 0.0f);
          const auto param_2 = glm::f32vec4(0.0f, 0.0f, 0.0f, 0.0f);
          const auto param_3 = glm::f32vec4(0.0f, 0.0f, 0.0f, 0.0f);

          return std::make_tuple(
            tex_0_idx, tex_1_idx, tex_2_idx, tex_3_idx,
            param_0, param_1, param_2, param_3);
        };


      for (const auto& [mesh_id, transform] : mesh_relations)
      {
        const auto& gltf_mesh = gltf_model.meshes[mesh_id];
        for (size_t k = 0; k < gltf_mesh.primitives.size(); ++k)
        {
          const auto& gltf_primitive = gltf_mesh.primitives[k];

          const auto [vert_offset, vert_count, prim_offset, prim_count, bone_offset, bone_count, mlet_offset, mlet_count, aabb_min, aabb_max] =
            geometry_convert_fn(gltf_primitive, transform, scope.position_scale, scope.conversion_rhs, false);

          const auto [tex_0_layer, tex_1_layer, tex_2_layer, tex_3_layer, surface_param_0, surface_param_1, surface_param_2, surface_param_3] =
            material_convert_fn(gltf_primitive, scope.texture_level);

          if (vert_count == 0 || prim_count == 0) continue;

          Instance instance;
          instance.offset_0 = vert_offset;
          instance.count_0 = vert_count;
          instance.offset_1 = prim_offset;
          instance.count_1 = prim_count;
          instance.offset_2 = bone_offset;
          instance.count_2 = bone_count;
          instance.offset_3 = mlet_offset;
          instance.count_3 = mlet_count;

          instance.transform = glm::identity<glm::fmat3x4>();

          instance.aabb_min = aabb_min;
          instance.geom_idx = uint32_t(scope.instances.size());
          instance.aabb_max = aabb_max;
          instance.brdf_idx = 1;

          instance.layer_0 = tex_0_layer;
          instance.layer_1 = tex_1_layer;
          instance.layer_2 = tex_2_layer;
          instance.layer_3 = tex_3_layer;

          instance.param_0 = surface_param_0;
          instance.param_1 = surface_param_1;
          instance.param_2 = surface_param_2;
          instance.param_3 = surface_param_3;

          scope.instances.push_back(instance);
        }
      }
    }

    void GLTFConverter::Export()
    {
    }

    GLTFConverter::GLTFConverter(Scope& scope)
      : Mode(scope)
    {
    }

    GLTFConverter::~GLTFConverter()
    {
    }
  }
}