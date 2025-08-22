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
//#define TINYGLTF_NO_STB_IMAGE
//#define TINYGLTF_NO_STB_IMAGE_WRITE
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

      auto vert_buffer = StructureBuffer<Vertex>();
      auto trng_buffer = StructureBuffer<Triangle>();
      auto mlet_buffer = StructureBuffer<Meshlet>();
      auto bone_buffer = StructureBuffer<uint8_t>();

      const auto geometry_convert_fn = [&gltf_model, &vert_buffer, &trng_buffer, &mlet_buffer, &bone_buffer]
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

          auto geometry = Geometry(idx_count, idx_stride, idx_order,
            pos_data, pos_stride, idx_data, idx_stride, pos_transform,
            nrm_data, nrm_stride, idx_data, idx_stride, nrm_transform,
            tc0_data, tc0_stride, idx_data, idx_stride, tc0_transform);

          geometry.CalculateTangents();
          //geometry.CalculateMeshlets();

          const auto vert_count = geometry.vertices.size();
          const auto vert_items = geometry.vertices.data();
          vert_buffer.Resize(vert_buffer.Length() + vert_count);
          vert_buffer.Set({ vert_items, vert_count });

          const auto trng_count = geometry.triangles.size();
          const auto trng_items = geometry.triangles.data();
          trng_buffer.Resize(trng_buffer.Length() + trng_count);
          trng_buffer.Set({ trng_items, trng_count });

          const auto mlet_count = 0u;

          const auto bone_count = 0u;

          const auto aabb_min = geometry.aabb_min;
          const auto aabb_max = geometry.aabb_max;

          return std::make_tuple(vert_count, trng_count, mlet_count, bone_count, aabb_min, aabb_max);
        };

      std::unordered_map<int, Raw> texture_all_cache;
      std::unordered_map<glm::u32vec4, uint32_t> texture_0_indices;
      std::unordered_map<glm::u32vec4, uint32_t> texture_1_indices;
      std::unordered_map<glm::u32vec4, uint32_t> texture_2_indices;

      const auto material_convert_fn = [this, &gltf_model, &texture_all_cache, &texture_0_indices, &texture_1_indices, &texture_2_indices]
      (const tinygltf::Primitive& gltf_primitive, uint32_t texture_level)
      {
        const auto extent_x = 1u << texture_level - 1;
        const auto extent_y = 1u << texture_level - 1;

        const auto cache_texture_fn = [gltf_model, &texture_all_cache, extent_x, extent_y]
          (int index, bool srgb)
          {
            if (index == -1) return;

            const auto iterator = texture_all_cache.find(index);
            if (iterator != texture_all_cache.cend()) return;
            
            const auto& image = gltf_model.images[index];
            const auto src_extent_x = image.width;
            const auto src_extent_y = image.height;
            const auto src_data = image.image.data();

            auto dst_extent_x = extent_x;
            auto dst_extent_y = extent_y;
            auto raw = Raw(dst_extent_x * dst_extent_y, glm::zero<glm::u8vec4>());
            auto dst_data = raw.AccessBytes().first;

            if (srgb) { stbir_resize_uint8_srgb(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, 4, 3, 0); }
            else { stbir_resize_uint8(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, 4); }

            texture_all_cache[index] = std::move(raw);
          };

        const auto& material = gltf_model.materials[gltf_primitive.material];

        cache_texture_fn(material.pbrMetallicRoughness.baseColorTexture.index, true);
        cache_texture_fn(material.pbrMetallicRoughness.metallicRoughnessTexture.index, false);
        cache_texture_fn(material.normalTexture.index, false);
        cache_texture_fn(material.occlusionTexture.index, false);
        cache_texture_fn(material.emissiveTexture.index, true);

        const auto key_0 = glm::u32vec4(
          material.pbrMetallicRoughness.baseColorTexture.index,
          material.pbrMetallicRoughness.baseColorTexture.index,
          material.pbrMetallicRoughness.baseColorTexture.index,
          material.pbrMetallicRoughness.metallicRoughnessTexture.index
        );
        const auto res_0 = texture_0_indices.find(key_0);
        const auto idx_0 = res_0 != texture_0_indices.cend() ? res_0->second : texture_0_indices[key_0] = texture_0_indices.size();

        const auto key_1 = glm::u32vec4(
          material.pbrMetallicRoughness.metallicRoughnessTexture.index,
          material.normalTexture.index,
          material.normalTexture.index,
          material.occlusionTexture.index
        );
        const auto res_1 = texture_1_indices.find(key_1);
        const auto idx_1 = res_1 != texture_1_indices.cend() ? res_1->second : texture_1_indices[key_1] = texture_1_indices.size();

        const auto key_2 = glm::u32vec4(
          material.emissiveTexture.index,
          material.emissiveTexture.index,
          material.emissiveTexture.index,
          material.pbrMetallicRoughness.baseColorTexture.index
        );
        const auto res_2 = texture_2_indices.find(key_2);
        const auto idx_2 = res_2 != texture_2_indices.cend() ? res_2->second : texture_2_indices[key_2] = texture_2_indices.size();

        const auto idx_3 = uint32_t(-1);

        return std::make_tuple(idx_0, idx_1, idx_2, idx_3);
      };

      


      auto vert_offset{ 0ull };
      auto trng_offset{ 0ull };
      auto mlet_offset{ 0ull };
      auto bone_offset{ 0ull };

      for (const auto& [mesh_id, transform] : mesh_relations)
      {
        const auto& gltf_mesh = gltf_model.meshes[mesh_id];
        for (size_t k = 0; k < gltf_mesh.primitives.size(); ++k)
        {
          const auto& gltf_primitive = gltf_mesh.primitives[k];

          const auto [vert_count, trng_count, mlet_count, bone_count, aabb_min, aabb_max] =
            geometry_convert_fn(gltf_primitive, transform, scope.position_scale, scope.conversion_rhs, false);

          const auto [am_layer, snao_layer, et_layer, mask_layer] =
            material_convert_fn(gltf_primitive, scope.texture_level);

          if (vert_count == 0 || trng_count == 0) continue;

          Instance instance;
          instance.vert_offset = vert_offset;
          instance.vert_count = vert_count;
          instance.trng_offset = trng_offset;
          instance.trng_count = trng_count;
          instance.mlet_offset = mlet_offset;
          instance.mlet_count = mlet_count;
          instance.bone_offset = bone_offset;
          instance.bone_count = bone_count;

          instance.transform = glm::identity<glm::fmat3x4>();

          instance.aabb_min = aabb_min;
          instance.index = scope.inst_buffer.Length();
          instance.aabb_max = aabb_max;
          instance.flags = 0;

          instance.am_layer = am_layer;
          instance.snao_layer = snao_layer;
          instance.et_layer = et_layer;
          instance.mask_layer = mask_layer;

          scope.inst_buffer.Resize(scope.inst_buffer.Length() + 1, instance);

          vert_offset += vert_count;
          trng_offset += trng_count;
          mlet_offset += mlet_count;
          bone_offset += bone_count;
        }
      }

      std::swap(scope.vert_buffer, vert_buffer);
      std::swap(scope.trng_buffer, trng_buffer);
      std::swap(scope.mlet_buffer, mlet_buffer);
      std::swap(scope.bone_buffer, bone_buffer);


      const auto extent_x = 1u << scope.texture_level - 1;
      const auto extent_y = 1u << scope.texture_level - 1;

      auto am_array = TextureArrayLDR(FORMAT_R8G8B8A8_SRGB, extent_x, extent_y, texture_0_indices.size());
      for (const auto& [key, index] : texture_0_indices)
      {
        //am_array.Create(index);
        for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
        {
          const auto value = glm::u8vec4(
            key[0] == -1 ? 255u : texture_all_cache[key[0]].GetItem<glm::u8vec4>(i).r,
            key[1] == -1 ? 255u : texture_all_cache[key[1]].GetItem<glm::u8vec4>(i).g,
            key[2] == -1 ? 255u : texture_all_cache[key[2]].GetItem<glm::u8vec4>(i).b,
            key[3] == -1 ? 000u : texture_all_cache[key[3]].GetItem<glm::u8vec4>(i).r);
          am_array.Set(index, 0, { &value, 1 }, i);
        }
      }

      auto snao_array = TextureArrayLDR(FORMAT_R8G8B8A8_UNORM, extent_x, extent_y, texture_1_indices.size());
      for (const auto& [key, index] : texture_1_indices)
      {
        //snao_array.Create(index);
        for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
        {
          const auto value = glm::u8vec4(
            key[0] == -1 ? 0u : texture_all_cache[key[0]].GetItem<glm::u8vec4>(i).g,
            key[1] == -1 ? 0u : texture_all_cache[key[1]].GetItem<glm::u8vec4>(i).r,
            key[2] == -1 ? 0u : texture_all_cache[key[2]].GetItem<glm::u8vec4>(i).g,
            key[3] == -1 ? 0u : texture_all_cache[key[3]].GetItem<glm::u8vec4>(i).r);
          snao_array.Set(index, 0, { &value, 1 }, i);
        }
      }     

      auto et_array = TextureArrayLDR(FORMAT_R8G8B8A8_SRGB, extent_x, extent_y, texture_2_indices.size());
      for (const auto& [key, index] : texture_2_indices)
      {
        //et_array.Create(index);
        for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
        {
          const auto value = glm::u8vec4(
            key[0] == -1 ? 0u : texture_all_cache[key[0]].GetItem<glm::u8vec4>(i).r,
            key[1] == -1 ? 0u : texture_all_cache[key[1]].GetItem<glm::u8vec4>(i).g,
            key[2] == -1 ? 0u : texture_all_cache[key[2]].GetItem<glm::u8vec4>(i).b,
            key[3] == -1 ? 0u : texture_all_cache[key[3]].GetItem<glm::u8vec4>(i).a);
          et_array.Set(index, 0, { &value, 1 }, i);
        }
      }

      std::swap(scope.am_array, am_array);
      std::swap(scope.snao_array, snao_array);
      std::swap(scope.et_array, et_array);
      //std::swap(scope.array_3, array_3);
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