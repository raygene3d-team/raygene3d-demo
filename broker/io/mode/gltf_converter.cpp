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





      std::vector<std::vector<Vertex>> vtx_segments;
      std::vector<std::vector<Triangle>> trg_segments;

      const auto geometry_convert_fn = [&gltf_model, &vtx_segments, &trg_segments]
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
          const auto [vtx_segment, trg_segment, aabb_min, aabb_max] = PopulateInstance(idx_count, idx_stride,
            idx_order, pos_transform, nrm_transform, tc0_transform,
            pos_data, pos_stride, idx_data, idx_stride,
            nrm_data, nrm_stride, idx_data, idx_stride,
            tc0_data, tc0_stride, idx_data, idx_stride);

          const auto vtx_segment_idx = vtx_segment.empty() ? size_t(-1) : vtx_segments.size();
          vtx_segments.push_back(std::move(vtx_segment));

          const auto trg_segment_idx = trg_segment.empty() ? size_t(-1) : trg_segments.size();
          trg_segments.push_back(std::move(trg_segment));

          return std::make_tuple(vtx_segment_idx, trg_segment_idx, aabb_min, aabb_max);
        };

      //std::vector<uint32_t> texture_0_indices;
      //std::vector<uint32_t> texture_1_indices;
      //std::vector<uint32_t> texture_2_indices;
      //std::vector<uint32_t> texture_3_indices;
      //std::vector<uint32_t> texture_4_indices;
      //std::vector<uint32_t> texture_5_indices;
      //std::vector<uint32_t> texture_6_indices;
      //std::vector<uint32_t> texture_7_indices;

      std::unordered_map<uint32_t, std::tuple<Raw, uint32_t, uint32_t>> texture_items;

      std::unordered_map<std::string, uint32_t> texture_0_indices;
      std::unordered_map<std::string, uint32_t> texture_1_indices;
      std::unordered_map<std::string, uint32_t> texture_2_indices;
      std::unordered_map<std::string, uint32_t> texture_3_indices;
      //std::vector< std::tuple<Raw, uint32_t, uint32_t>> texture_items;

      //std::vector<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t>> instance_indices;

      //const auto tex_prepare_fn = [&gltf_model](uint32_t texture_index)
      //  {
      //    const auto image_index = gltf_model.textures[texture_index].source;

      //    const auto tex_x = gltf_model.images[image_index].width;
      //    const auto tex_y = gltf_model.images[image_index].height;
      //    const auto tex_n = gltf_model.images[image_index].component;
      //    const auto tex_data = gltf_model.images[image_index].image.data();

      //    auto raw = Raw(tex_x * tex_y * uint32_t(sizeof(glm::u8vec4)));

      //    for (uint32_t i = 0; i < tex_x * tex_y; ++i)
      //    {
      //      const auto r = tex_n > 0 ? tex_data[i * tex_n + 0] : 0; //0xFF;
      //      const auto g = tex_n > 1 ? tex_data[i * tex_n + 1] : r; //0xFF;
      //      const auto b = tex_n > 2 ? tex_data[i * tex_n + 2] : r; //0xFF;
      //      const auto a = tex_n > 3 ? tex_data[i * tex_n + 3] : r; //0xFF;
      //      raw.SetElement<glm::u8vec4>({ r, g, b, a }, i);
      //    }

      //    return std::make_tuple(std::move(raw), uint32_t(tex_x), uint32_t(tex_y));
      //  };

      std::unordered_map<uint32_t, Raw> texture_cache;
      std::vector<std::pair<glm::u32vec4, Raw>> textures_0;
      std::vector<std::pair<glm::u32vec4, Raw>> textures_1;

      const auto extent_x = 1u << scope.texture_level - 1;
      const auto extent_y = 1u << scope.texture_level - 1;



      const auto texture_pack_fn = [&gltf_model, &textures_0, &textures_1, &texture_cache, extent_x, extent_y]
      (const tinygltf::Primitive& gltf_primitive)
        {
          const auto find_by_id_fn = [&gltf_model, &texture_cache, extent_x, extent_y](uint32_t texture_id)
            {
              const auto res = texture_cache.find(texture_id);
              if (res != texture_cache.cend()) return std::forward<Raw&&>(res->second);

              const auto image_index = gltf_model.textures[texture_id].source;

              const auto tex_x = gltf_model.images[image_index].width;
              const auto tex_y = gltf_model.images[image_index].height;
              const auto tex_n = gltf_model.images[image_index].component;
              const auto tex_data = gltf_model.images[image_index].image.data();

              auto src_extent_x = gltf_model.images[image_index].width;
              auto src_extent_y = gltf_model.images[image_index].height;
              auto src_channels = gltf_model.images[image_index].component;
              auto src_data = gltf_model.images[image_index].image.data();

              auto raw = Raw(sizeof(glm::u8vec4) * extent_x * extent_y);

              auto dst_extent_x = extent_x;
              auto dst_extent_y = extent_y;
              auto dst_data = reinterpret_cast<uint8_t*>(raw.AccessBytes().first);

              stbir_resize_uint8(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, 4);

              return std::forward<Raw&&>(texture_cache[texture_id] = std::move(raw));
            };

          const auto& gltf_material = gltf_model.materials[gltf_primitive.material];

          const auto&& baseColor = find_by_id_fn(gltf_material.pbrMetallicRoughness.baseColorTexture.index);
          const auto&& emission = find_by_id_fn(gltf_material.emissiveTexture.index);
          const auto&& metallicRoughness = find_by_id_fn(gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index);
          const auto&& normal = find_by_id_fn(gltf_material.normalTexture.index);
          const auto&& occlusion = find_by_id_fn(gltf_material.occlusionTexture.index);

          const auto key_0 = glm::u32vec4(
            gltf_material.pbrMetallicRoughness.baseColorTexture.index,
            gltf_material.pbrMetallicRoughness.baseColorTexture.index,
            gltf_material.pbrMetallicRoughness.baseColorTexture.index,
            gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index
          );

            const auto find_by_key_fn = [](const glm::u32vec4& key)
            {
            };

          const auto& res_0 = std::find_if(textures_0.begin(), textures_0.end(),
            [&key_0](const std::pair<glm::u32vec4, Raw>& item) { return key_0 == item.first; });

          if(res_0 == textures_0.end())
          {


          {
            auto texture_0 = PopulateTextureLDR(extent_x, extent_y, [&gltf_material, &texture_items](uint32_t i)
              {
                const auto baseColor = gltf_material.pbrMetallicRoughness.baseColorTexture.index == -1 ? glm::u8vec4(255u, 255u, 255u, 0u) :
                  std::get<0>(texture_items.at(gltf_material.pbrMetallicRoughness.baseColorTexture.index)).GetElement<glm::u8vec4>(i);
                const auto metallicRoughness = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index == -1 ? glm::u8vec4(0u, 0u, 0u, 0u) :
                  std::get<0>(texture_items.at(gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index)).GetElement<glm::u8vec4>(i);

                return glm::u8vec4(baseColor.r, baseColor.g, baseColor.b, metallicRoughness.r);
              });
        }

      const auto&& t0 = texture_find_fn(1);
            

          return texture_cache.find(texture_id) == texture_cache.cend()

            return { std::move(raw), extent_x, extent_y };

            auto raw = Raw(tex_x * tex_y * sizeof(glm::u8vec4));

            for (size_t i = 0u; i < tex_x* tex_y; ++i)
            {
              const auto r = tex_n > 0 ? tex_data[i * tex_n + 0] : 0; //0xFF;
              const auto g = tex_n > 1 ? tex_data[i * tex_n + 1] : r; //0xFF;
              const auto b = tex_n > 2 ? tex_data[i * tex_n + 2] : r; //0xFF;
              const auto a = tex_n > 3 ? tex_data[i * tex_n + 3] : r; //0xFF;
              raw.SetElement<glm::u8vec4>({ r, g, b, a }, i);
            }

            texture_items.at(texture_id) = ResizeTextureLDR(extent_x, extent_y,
              { std::move(raw), uint32_t(tex_x), uint32_t(tex_y) });





      const auto texture_cache_fn = [&texture_items, &extent_x, &extent_y, &gltf_model](uint32_t texture_id)
        {
          if (texture_items.find(texture_id) == texture_items.cend())
          {
            const auto image_index = gltf_model.textures[texture_id].source;

            const auto tex_x = gltf_model.images[image_index].width;
            const auto tex_y = gltf_model.images[image_index].height;
            const auto tex_n = gltf_model.images[image_index].component;
            const auto tex_data = gltf_model.images[image_index].image.data();

            auto raw = Raw(tex_x * tex_y * sizeof(glm::u8vec4));

            for (size_t i = 0u; i < tex_x * tex_y; ++i)
            {
              const auto r = tex_n > 0 ? tex_data[i * tex_n + 0] : 0; //0xFF;
              const auto g = tex_n > 1 ? tex_data[i * tex_n + 1] : r; //0xFF;
              const auto b = tex_n > 2 ? tex_data[i * tex_n + 2] : r; //0xFF;
              const auto a = tex_n > 3 ? tex_data[i * tex_n + 3] : r; //0xFF;
              raw.SetElement<glm::u8vec4>({ r, g, b, a }, i);
            }

            texture_items.at(texture_id) = ResizeTextureLDR(extent_x, extent_y,
                { std::move(raw), uint32_t(tex_x), uint32_t(tex_y) });
          }

          return texture_id;
        };

      //const auto tex_reindex_fn = [](std::vector<uint32_t>& tex_ids, uint32_t tex_id)
      //  {
      //    if (tex_id == -1)
      //    {
      //      return uint32_t(-1);
      //    }

      //    const auto tex_iter = std::find_if(tex_ids.cbegin(), tex_ids.cend(), [&tex_id](const auto& index) { return tex_id == index; });
      //    const auto tex_index = tex_iter == tex_ids.cend() ? uint32_t(tex_ids.size()) : uint32_t(tex_iter - tex_ids.cbegin());
      //    if (tex_index == tex_ids.size())
      //    {
      //      tex_ids.push_back(tex_id);
      //    }
      //    return tex_index;
      //  };



      for (const auto& [mesh_id, transform] : mesh_relations)
      {
        const auto& gltf_mesh = gltf_model.meshes[mesh_id];
        for (size_t k = 0; k < gltf_mesh.primitives.size(); ++k)
        {
          const auto& gltf_primitive = gltf_mesh.primitives[k];
          const auto [vtx_segment_idx, trg_segment_idx, aabb_min, aabb_max] =
            geometry_convert_fn(gltf_primitive, transform, scope.position_scale, scope.conversion_rhs, false);

          if (vtx_segment_idx == -1 || trg_segment_idx == -1) continue;

          Instance instance;
          instance.transform = glm::identity<glm::fmat3x4>();
          instance.aabb_min = aabb_min;
          instance.geom_idx = uint32_t(scope.instances.size());
          instance.aabb_max = aabb_max;
          instance.brdf_idx = 1;
          instance.segment0_idx = vtx_segment_idx;
          instance.segment1_idx = trg_segment_idx;

          const auto& gltf_material = gltf_model.materials[gltf_primitive.material];

          const auto texture_0_id = texture_cache_fn(gltf_material.pbrMetallicRoughness.baseColorTexture.index);
          const auto texture_1_id = texture_cache_fn(gltf_material.emissiveTexture.index);
          const auto texture_2_id = texture_cache_fn(gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index);
          const auto texture_3_id = texture_cache_fn(gltf_material.normalTexture.index);
          const auto texture_4_id = texture_cache_fn(gltf_material.occlusionTexture.index);

          {
            auto texture_0 = PopulateTextureLDR(extent_x, extent_y, [&gltf_material, &texture_items](uint32_t i)
              {
                const auto baseColor = gltf_material.pbrMetallicRoughness.baseColorTexture.index == -1 ? glm::u8vec4(255u, 255u, 255u, 0u) :
                  std::get<0>(texture_items.at(gltf_material.pbrMetallicRoughness.baseColorTexture.index)).GetElement<glm::u8vec4>(i);
                const auto metallicRoughness = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index == -1 ? glm::u8vec4(0u, 0u, 0u, 0u) :
                  std::get<0>(texture_items.at(gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index)).GetElement<glm::u8vec4>(i);

                return glm::u8vec4(baseColor.r, baseColor.g, baseColor.b, metallicRoughness.r);
              });

            digestpp::md5 hash_provider;
            const auto [bytes, size] = std::get<0>(texture_0).GetBytes(0);
            const auto hash = hash_provider.absorb(bytes, size).hexdigest();

            if (const auto res = texture_0_indices.find(hash); res == texture_0_indices.cend())
            {
              instance.texture0_idx = scope.textures_0.size(); 
              texture_0_indices[hash] = scope.textures_0.size();
              scope.textures_0.push_back(std::move(texture_0));
            }
            else
            {
              instance.texture0_idx = res->second;
            }
          }

          {
            auto texture_1 = PopulateTextureLDR(extent_x, extent_y, [&gltf_material, &texture_items](uint32_t i)
              {
                const auto metallicRoughness = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index == -1 ? glm::u8vec4(0u, 0u, 0u, 0u) :
                  std::get<0>(texture_items.at(gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index)).GetElement<glm::u8vec4>(i);
                const auto normal = gltf_material.normalTexture.index == -1 ? glm::u8vec4(0u, 0u, 255u, 0u) :
                  std::get<0>(texture_items.at(gltf_material.normalTexture.index)).GetElement<glm::u8vec4>(i);
                const auto occlusion = gltf_material.occlusionTexture.index == -1 ? glm::u8vec4(255u, 0u, 0u, 0u) :
                  std::get<0>(texture_items.at(gltf_material.occlusionTexture.index)).GetElement<glm::u8vec4>(i);

                return glm::u8vec4(metallicRoughness.g, normal.r, normal.g, occlusion.r);
              });

            digestpp::md5 hash_provider;
            const auto [bytes, size] = std::get<0>(texture_1).GetBytes(0);
            const auto hash = hash_provider.absorb(bytes, size).hexdigest();

            if (const auto res = texture_1_indices.find(hash); res == texture_1_indices.cend())
            {
              instance.texture1_idx = scope.textures_1.size();
              texture_0_indices[hash] = scope.textures_1.size();
              scope.textures_1.push_back(std::move(texture_1));
            }
            else
            {
              instance.texture1_idx = res->second;
            }
          }

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