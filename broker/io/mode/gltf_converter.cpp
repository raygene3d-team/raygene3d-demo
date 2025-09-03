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
//#include <stb/stb_image.h>
//#include <stb/stb_image_write.h>
#include <stb/stb_image_resize.h>

#include "gltf_converter.h"

namespace RayGene3D
{
  namespace IO
  {
    void ParseNode(const tinygltf::Model& model, size_t node_index, std::unordered_map<int, glm::fmat4x4>& mesh_transforms,
      const glm::fmat4x4& parent_transform)
    {
      auto transform = glm::identity<glm::fmat4x4>();

      const auto& node = model.nodes[node_index];

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
        mesh_transforms.insert({ node.mesh, transform });
      }

      if (node.children.size() == 0)
      {
        return;
      }

      for (const auto& child_index : node.children)
      {
        ParseNode(model, child_index, mesh_transforms, transform);
      }
    }

    CByteData AccessBuffer(const tinygltf::Model& model, const tinygltf::Accessor& accessor)
    {
      const auto& gltf_view = model.bufferViews[accessor.bufferView];
      const auto length = gltf_view.byteLength;
      const auto offset = gltf_view.byteOffset;

      const auto& gltf_buffer = model.buffers[gltf_view.buffer];
      const auto data = &gltf_buffer.data[accessor.byteOffset];

      return std::pair{ data + offset, length };
    }


    int CacheTexture(const tinygltf::Model& model, int index, std::unordered_map<int, Raw>& texture_sources, 
      uint32_t extent_x, uint32_t extent_y, bool srgb)
    {
      if (index == -1) return -1;
      const auto source = model.textures[index].source;

      const auto iterator = texture_sources.find(source);
      if (iterator != texture_sources.cend()) return source;

      const auto& image = model.images[source];
      const auto src_extent_x = image.width;
      const auto src_extent_y = image.height;
      const auto src_data = image.image.data();

      auto dst_extent_x = extent_x;
      auto dst_extent_y = extent_y;
      auto raw = Raw(dst_extent_x * dst_extent_y, glm::zero<glm::u8vec4>());
      auto dst_data = raw.AccessBytes().first;

      if (srgb) { stbir_resize_uint8_srgb(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, 4, 3, 0); }
      else { stbir_resize_uint8(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, 4); }

      texture_sources[source] = std::move(raw);
      return source;
    }

    void GLTFConverter::Import()
    {
      tinygltf::TinyGLTF gltf_context;
      tinygltf::Model gltf_model;

      std::string err, warn;
      BLAST_ASSERT(gltf_context.LoadASCIIFromFile(&gltf_model, &err, &warn, (scope.path_name + scope.file_name).c_str()));

      auto extent_x = 1u << scope.texture_level - 1;
      auto extent_y = 1u << scope.texture_level - 1;

      ParseNode(gltf_model, gltf_model.scenes[0].nodes[0], mesh_transforms, glm::identity<glm::f32mat4x4>());

      for (const auto& [index, transform] : mesh_transforms)
      {
        const auto& gltf_mesh = gltf_model.meshes[index];
        for (size_t k = 0; k < gltf_mesh.primitives.size(); ++k)
        {
          const auto& gltf_primitive = gltf_mesh.primitives[k];

          BLAST_ASSERT(gltf_primitive.mode == TINYGLTF_MODE_TRIANGLES);

          const auto& gltf_positions = gltf_model.accessors[gltf_primitive.attributes.at("POSITION")];
          BLAST_ASSERT(gltf_positions.type == TINYGLTF_TYPE_VEC3 && gltf_positions.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
          const auto pos_data = AccessBuffer(gltf_model, gltf_positions);
          const auto pos_stride = 12;

          const auto& gltf_normals = gltf_model.accessors[gltf_primitive.attributes.at("NORMAL")];
          BLAST_ASSERT(gltf_normals.type == TINYGLTF_TYPE_VEC3 && gltf_normals.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
          const auto nrm_data = AccessBuffer(gltf_model, gltf_normals);
          const auto nrm_stride = 12;

          const auto& gltf_texcoords = gltf_model.accessors[gltf_primitive.attributes.at("TEXCOORD_0")];
          BLAST_ASSERT(gltf_texcoords.type == TINYGLTF_TYPE_VEC2 && gltf_texcoords.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
          const auto tc0_data = AccessBuffer(gltf_model,gltf_texcoords);
          const auto tc0_stride = 8;

          const auto& gltf_indices = gltf_model.accessors[gltf_primitive.indices];
          BLAST_ASSERT(gltf_indices.type == TINYGLTF_TYPE_SCALAR);
          const auto idx_data = AccessBuffer(gltf_model, gltf_indices);
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

          const auto mdf_transform = scope.conversion_rhs ? lhs_transform : glm::identity<glm::fmat3x3>();
          const auto nrm_transform = mdf_transform * glm::fmat3x3(transform);
          const auto pos_transform = glm::fmat4x4(mdf_transform * scope.position_scale) * transform;

          const auto flip_v_tranform = glm::fmat2x2(
            0.0f, 1.0f,
            1.0f, 0.0f
          );
          const auto tc0_transform = false ? flip_v_tranform : glm::identity<glm::fmat2x2>();
          const auto idx_order = scope.conversion_rhs ? glm::uvec3{ 0u, 2u, 1u } : glm::uvec3{ 0u, 1u, 2u };


          const auto& gltf_material = gltf_model.materials[gltf_primitive.material];

          

          const auto baseColorSource = CacheTexture(gltf_model, gltf_material.pbrMetallicRoughness.baseColorTexture.index,
            texture_sources, extent_x, extent_y, true);
          const auto metallicRoughnessSource = CacheTexture(gltf_model, gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index,
            texture_sources, extent_x, extent_y, false);
          const auto normalSource = CacheTexture(gltf_model, gltf_material.normalTexture.index,
            texture_sources, extent_x, extent_y, false);
          const auto occlusionSource = CacheTexture(gltf_model, gltf_material.occlusionTexture.index,
            texture_sources, extent_x, extent_y, false);
          const auto emissionSource = CacheTexture(gltf_model, gltf_material.emissiveTexture.index,
            texture_sources, extent_x, extent_y, true);

          const auto key_0 = glm::i32vec4(
            baseColorSource,
            baseColorSource,
            baseColorSource,
            metallicRoughnessSource
          );
          const auto res_0 = aaam_indices.find(key_0);
          const auto idx_0 = res_0 != aaam_indices.cend() ? res_0->second : aaam_indices[key_0] = aaam_indices.size();

          const auto key_1 = glm::i32vec4(
            metallicRoughnessSource,
            normalSource,
            normalSource,
            occlusionSource
          );
          const auto res_1 = snno_indices.find(key_1);
          const auto idx_1 = res_1 != snno_indices.cend() ? res_1->second : snno_indices[key_1] = snno_indices.size();

          const auto key_2 = glm::i32vec4(
            emissionSource,
            emissionSource,
            emissionSource,
            baseColorSource
          );
          const auto res_2 = eeet_indices.find(key_2);
          const auto idx_2 = res_2 != eeet_indices.cend() ? res_2->second : eeet_indices[key_2] = eeet_indices.size();

          const auto idx_3 = uint32_t(-1);

          mesh_items.emplace_back(idx_count, idx_stride, idx_order,
            pos_data, pos_stride, idx_data, idx_stride, pos_transform,
            nrm_data, nrm_stride, idx_data, idx_stride, nrm_transform,
            tc0_data, tc0_stride, idx_data, idx_stride, tc0_transform,
            idx_0, idx_1, idx_2, idx_3,
            glm::zero<glm::f32vec4>(), glm::zero<glm::f32vec4>(), glm::zero<glm::f32vec4>(), glm::zero<glm::f32vec4>());
        }
      }



      aaam_items.resize(aaam_indices.size());
      for (const auto& [key, index] : aaam_indices)
      {
        auto raw = Raw(extent_x * extent_y, glm::u8vec4(0u, 0u, 0u, 0u));
        for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
        {
          const auto value = glm::u8vec4(
            key[0] == -1 ? 255u : texture_sources[key[0]].GetItem<glm::u8vec4>(i).r,
            key[1] == -1 ? 255u : texture_sources[key[1]].GetItem<glm::u8vec4>(i).g,
            key[2] == -1 ? 255u : texture_sources[key[2]].GetItem<glm::u8vec4>(i).b,
            key[3] == -1 ? 000u : texture_sources[key[3]].GetItem<glm::u8vec4>(i).b);
          raw.SetItem(value, i);
        }
        aaam_items[index] = std::move(raw);
      }



      snno_items.resize(snno_indices.size());
      for (const auto& [key, index] : snno_indices)
      {
        auto raw = Raw(extent_x * extent_x, glm::u8vec4(0u, 0u, 0u, 0u));
        for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
        {
          const auto value = glm::u8vec4(
            key[0] == -1 ? 000u : texture_sources[key[0]].GetItem<glm::u8vec4>(i).g,
            key[1] == -1 ? 000u : texture_sources[key[1]].GetItem<glm::u8vec4>(i).r,
            key[2] == -1 ? 000u : texture_sources[key[2]].GetItem<glm::u8vec4>(i).g,
            key[3] == -1 ? 255u : texture_sources[key[3]].GetItem<glm::u8vec4>(i).b);
          raw.SetItem(value, i);
        }
        snno_items[index] = std::move(raw);
      }



      eeet_items.resize(eeet_indices.size());
      for (const auto& [key, index] : eeet_indices)
      {
        auto raw = Raw(extent_x * extent_x, glm::u8vec4(0u, 0u, 0u, 0u));
        for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
        {
          const auto value = glm::u8vec4(
            key[0] == -1 ? 0u : texture_sources[key[0]].GetItem<glm::u8vec4>(i).r,
            key[1] == -1 ? 0u : texture_sources[key[1]].GetItem<glm::u8vec4>(i).g,
            key[2] == -1 ? 0u : texture_sources[key[2]].GetItem<glm::u8vec4>(i).b,
            key[3] == -1 ? 0u : texture_sources[key[3]].GetItem<glm::u8vec4>(i).a);
          raw.SetItem(value, i);
        }
        eeet_items[index] = std::move(raw);
      }

      CalculateTangents();
      CalculateAtlas();
      CalculateMeshlets();
      CalculateBoxes();



      for (auto& mesh : mesh_items)
      {
        Instance instance;

        instance.vert_offset = scope.vert_buffer.Length();
        instance.vert_count = mesh.vertices.size();
        scope.vert_buffer.Resize(size_t(instance.vert_offset + instance.vert_count + instance.vert_count % 4));
        scope.vert_buffer.Set({ mesh.vertices.data(), mesh.vertices.size() }, instance.vert_offset);

        instance.trng_offset = scope.trng_buffer.Length();
        instance.trng_count = mesh.triangles.size();
        scope.trng_buffer.Resize(size_t(instance.trng_offset + instance.trng_count + instance.trng_count % 4));
        scope.trng_buffer.Set({ mesh.triangles.data(), mesh.triangles.size() }, instance.trng_offset);

        instance.mlet_offset = scope.mlet_buffer.Length();
        instance.mlet_count = mesh.meshlets.size();
        scope.mlet_buffer.Resize(size_t(instance.mlet_offset + instance.mlet_count + instance.mlet_count % 4));
        scope.mlet_buffer.Set({ mesh.meshlets.data(), mesh.meshlets.size() }, instance.mlet_offset);

        instance.bone_offset = 0;
        instance.bone_count = 0;

        instance.transform = glm::identity<glm::fmat3x4>();

        instance.aabb_min = mesh.aabb_min;
        instance.index = scope.inst_buffer.Length();
        instance.aabb_max = mesh.aabb_max;
        instance.flags = 0;

        instance.aaam_layer = mesh.aaam_texture_index;
        instance.snno_layer = mesh.snno_texture_index;
        instance.eeet_layer = mesh.eeet_texture_index;
        instance.mask_layer = mesh.mask_texture_index;

        scope.inst_buffer.Resize(scope.inst_buffer.Length() + 1, instance);

        const auto bbox_offset = scope.bbox_buffer.Length();
        const auto bbox_count = mesh.boxes.size();
        scope.bbox_buffer.Resize(bbox_offset + bbox_count);
        scope.bbox_buffer.Set({ mesh.boxes.data(), mesh.boxes.size() }, bbox_offset);
      }

      scope.tbox_buffer.Resize(boxes.size());
      scope.tbox_buffer.Set({ boxes.data(), boxes.size() });


      auto aaam_array = TextureArrayLDR(FORMAT_R8G8B8A8_SRGB, extent_x, extent_y, aaam_items.size(), scope.texture_level);
      for (auto i = 0ull; i < aaam_array.Layers(); ++i)
      {
        aaam_array.Set(i, 0, aaam_items[i].GetItems<glm::u8vec4>());
        for (auto j = 0ull; j < aaam_array.Levels() - 1; ++j)
        {
          auto src_offset = aaam_array.Offset(i, j);
          auto src_texels = aaam_array.Items(src_offset).first;
          auto src_size_x = Mip(extent_x, j);
          auto src_size_y = Mip(extent_y, j);

          auto dst_offset = aaam_array.Offset(i, j + 1);
          auto dst_texels = aaam_array.Items(dst_offset).first;
          auto dst_size_x = Mip(extent_x, j + 1);
          auto dst_size_y = Mip(extent_y, j + 1);

          ResizeLDR(src_texels, src_size_x, src_size_y, dst_texels, dst_size_x, dst_size_y, true);
        }
      }
      std::swap(scope.aaam_array, aaam_array);

      auto snno_array = TextureArrayLDR(FORMAT_R8G8B8A8_UNORM, extent_x, extent_y, snno_items.size(), scope.texture_level);
      for (auto i = 0ull; i < snno_array.Layers(); ++i)
      {
        snno_array.Set(i, 0, snno_items[i].GetItems<glm::u8vec4>());
        for (auto j = 0ull; j < snno_array.Levels() - 1; ++j)
        {
          auto src_texels = snno_array.Items(snno_array.Offset(i, j)).first;
          auto src_size_x = Mip(extent_x, j);
          auto src_size_y = Mip(extent_y, j);

          auto dst_texels = snno_array.Items(snno_array.Offset(i, j + 1)).first;
          auto dst_size_x = Mip(extent_x, j + 1);
          auto dst_size_y = Mip(extent_y, j + 1);

          ResizeLDR(src_texels, src_size_x, src_size_y, dst_texels, dst_size_x, dst_size_y, false);
        }
      }
      std::swap(scope.snno_array, snno_array);

      auto eeet_array = TextureArrayLDR(FORMAT_R8G8B8A8_SRGB, extent_x, extent_y, eeet_items.size(), scope.texture_level);
      for (auto i = 0ull; i < eeet_array.Layers(); ++i)
      {
        eeet_array.Set(i, 0, eeet_items[i].GetItems<glm::u8vec4>());
        for (auto j = 0ull; j < eeet_array.Levels() - 1; ++j)
        {
          auto src_texels = eeet_array.Items(eeet_array.Offset(i, j)).first;
          auto src_size_x = Mip(extent_x, j);
          auto src_size_y = Mip(extent_y, j);

          auto dst_texels = eeet_array.Items(eeet_array.Offset(i, j + 1)).first;
          auto dst_size_x = Mip(extent_x, j + 1);
          auto dst_size_y = Mip(extent_y, j + 1);

          ResizeLDR(src_texels, src_size_x, src_size_y, dst_texels, dst_size_x, dst_size_y, true);
        }
      }
      std::swap(scope.eeet_array, eeet_array);

      //    material_convert_fn(gltf_primitive, scope.texture_level);

      //    //if (vert_count == 0 || trng_count == 0) continue;

      //    //Instance instance;
      //    //instance.vert_offset = vert_offset;
      //    //instance.vert_count = vert_count;
      //    //instance.trng_offset = trng_offset;
      //    //instance.trng_count = trng_count;
      //    //instance.mlet_offset = mlet_offset;
      //    //instance.mlet_count = mlet_count;
      //    //instance.bone_offset = bone_offset;
      //    //instance.bone_count = bone_count;

      //    //instance.transform = glm::identity<glm::fmat3x4>();

      //    //instance.aabb_min = aabb_min;
      //    //instance.index = scope.inst_buffer.Length();
      //    //instance.aabb_max = aabb_max;
      //    //instance.flags = 0;

      //    //instance.aaam_layer = aaam_layer;
      //    //instance.snno_layer = snno_layer;
      //    //instance.eeet_layer = eeet_layer;
      //    //instance.mask_layer = mask_layer;

      //    //scope.inst_buffer.Resize(scope.inst_buffer.Length() + 1, instance);
      //  }
      //}

      //const auto geometry_convert_fn = [&gltf_model]
      //  (const tinygltf::Primitive& gltf_primitive, const glm::fmat4x4 transform, float scale, bool to_lhs, bool flip_v)
      //  {


      //    meshes.emplace_back(idx_count, idx_stride, idx_order,
      //      pos_data, pos_stride, idx_data, idx_stride, pos_transform,
      //      nrm_data, nrm_stride, idx_data, idx_stride, nrm_transform,
      //      tc0_data, tc0_stride, idx_data, idx_stride, tc0_transform);
      //  };

      //    //CalculateTangents(geometry);
      //    //CalculateMeshlets(geometry);

      //    //MainBuild(instance_items, triangle_items, vertex_items, t_boxes, b_boxes);

      //    
      //    
      //    //const auto vert_offset = vert_buffer.Length();
      //    //const auto vert_length = geometry.vertices.size();
      //    //const auto vert_items = geometry.vertices.data();
      //    //vert_buffer.Resize(vert_offset + vert_length + vert_length % 4);
      //    //vert_buffer.Set({ vert_items, vert_length }, vert_offset);

      //    //
      //    //const auto trng_offset = trng_buffer.Length();
      //    //const auto trng_length = geometry.triangles.size();
      //    //const auto trng_items = geometry.triangles.data();
      //    //trng_buffer.Resize(trng_offset + trng_length + trng_length % 4);
      //    //trng_buffer.Set({ trng_items, trng_length }, trng_offset);



      //    //const auto mlet_length = 0u;
      //    //const auto mlet_offset = 0u;

      //    //const auto bone_length = 0u;
      //    //const auto bone_offset = 0u;

      //    //const auto aabb_min = geometry.aabb_min;
      //    //const auto aabb_max = geometry.aabb_max;

      //    //return std::make_tuple(
      //    //  vert_length, vert_offset, 
      //    //  trng_length, trng_offset,
      //    //  mlet_length, mlet_offset,
      //    //  bone_length, bone_offset,
      //    //  aabb_min, aabb_max);
      // // };

      //std::unordered_map<std::string, Raw> texture_all_cache;
      //std::unordered_map<std::tuple<std::string, std::string, std::string, std::string>, uint32_t> texture_0_indices;
      //std::unordered_map<std::tuple<std::string, std::string, std::string, std::string>, uint32_t> texture_1_indices;
      //std::unordered_map<std::tuple<std::string, std::string, std::string, std::string>, uint32_t> texture_2_indices;

      //const auto material_convert_fn = [this, &gltf_model, &texture_all_cache, &texture_0_indices, &texture_1_indices, &texture_2_indices]
      //(const tinygltf::Primitive& gltf_primitive, uint32_t texture_level)
      //{
      //  const auto extent_x = 1u << texture_level - 1;
      //  const auto extent_y = 1u << texture_level - 1;

      //  const auto cache_texture_fn = [gltf_model, &texture_all_cache, extent_x, extent_y]
      //    (int index, bool srgb)
      //    {
      //      if (index == -1) return -1;

      //      const auto source = gltf_model.textures[index].source;

      //      const auto iterator = texture_all_cache.find(source);
      //      if (iterator != texture_all_cache.cend()) return source;            
      //      
      //      const auto& image = gltf_model.images[source];
      //      const auto src_extent_x = image.width;
      //      const auto src_extent_y = image.height;
      //      const auto src_data = image.image.data();

      //      auto dst_extent_x = extent_x;
      //      auto dst_extent_y = extent_y;
      //      auto raw = Raw(dst_extent_x * dst_extent_y, glm::zero<glm::u8vec4>());
      //      auto dst_data = raw.AccessBytes().first;

      //      //if (srgb) { stbir_resize_uint8_srgb(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, 4, 3, 0); }
      //      //else { stbir_resize_uint8(src_data, src_extent_x, src_extent_y, 0, dst_data, dst_extent_x, dst_extent_y, 0, 4); }

      //      texture_all_cache[source] = std::move(raw);
      //      return source;
      //    };

      //  const auto& material = gltf_model.materials[gltf_primitive.material];

      //  const auto baseColorSource = cache_texture_fn(material.pbrMetallicRoughness.baseColorTexture.index, true);
      //  const auto metallicRoughnessSource = cache_texture_fn(material.pbrMetallicRoughness.metallicRoughnessTexture.index, false);
      //  const auto normalSource = cache_texture_fn(material.normalTexture.index, false);
      //  const auto occlusionSource = cache_texture_fn(material.occlusionTexture.index, false);
      //  const auto emissionSource = cache_texture_fn(material.emissiveTexture.index, true);

      //  const auto key_0 = glm::u32vec4(
      //    baseColorSource,
      //    baseColorSource,
      //    baseColorSource,
      //    metallicRoughnessSource
      //  );
      //  const auto res_0 = texture_0_indices.find(key_0);
      //  const auto idx_0 = res_0 != texture_0_indices.cend() ? res_0->second : texture_0_indices[key_0] = texture_0_indices.size();

      //  const auto key_1 = glm::u32vec4(
      //    metallicRoughnessSource,
      //    normalSource,
      //    normalSource,
      //    occlusionSource
      //  );
      //  const auto res_1 = texture_1_indices.find(key_1);
      //  const auto idx_1 = res_1 != texture_1_indices.cend() ? res_1->second : texture_1_indices[key_1] = texture_1_indices.size();

      //  const auto key_2 = glm::u32vec4(
      //    emissionSource,
      //    emissionSource,
      //    emissionSource,
      //    baseColorSource
      //  );
      //  const auto res_2 = texture_2_indices.find(key_2);
      //  const auto idx_2 = res_2 != texture_2_indices.cend() ? res_2->second : texture_2_indices[key_2] = texture_2_indices.size();

      //  const auto idx_3 = uint32_t(-1);

      //  return std::make_tuple(idx_0, idx_1, idx_2, idx_3);
      //};

      //for (const auto& [mesh_id, transform] : mesh_relations)
      //{
      //  const auto& gltf_mesh = gltf_model.meshes[mesh_id];
      //  for (size_t k = 0; k < gltf_mesh.primitives.size(); ++k)
      //  {
      //    const auto& gltf_primitive = gltf_mesh.primitives[k];

      //    geometry_convert_fn(gltf_primitive, transform, scope.position_scale, scope.conversion_rhs, false);

      //    material_convert_fn(gltf_primitive, scope.texture_level);

      //    //if (vert_count == 0 || trng_count == 0) continue;

      //    //Instance instance;
      //    //instance.vert_offset = vert_offset;
      //    //instance.vert_count = vert_count;
      //    //instance.trng_offset = trng_offset;
      //    //instance.trng_count = trng_count;
      //    //instance.mlet_offset = mlet_offset;
      //    //instance.mlet_count = mlet_count;
      //    //instance.bone_offset = bone_offset;
      //    //instance.bone_count = bone_count;

      //    //instance.transform = glm::identity<glm::fmat3x4>();

      //    //instance.aabb_min = aabb_min;
      //    //instance.index = scope.inst_buffer.Length();
      //    //instance.aabb_max = aabb_max;
      //    //instance.flags = 0;

      //    //instance.aaam_layer = aaam_layer;
      //    //instance.snno_layer = snno_layer;
      //    //instance.eeet_layer = eeet_layer;
      //    //instance.mask_layer = mask_layer;

      //    //scope.inst_buffer.Resize(scope.inst_buffer.Length() + 1, instance);
      //  }
      //}

      ////std::swap(scope.vert_buffer, vert_buffer);
      ////std::swap(scope.trng_buffer, trng_buffer);
      ////std::swap(scope.mlet_buffer, mlet_buffer);
      ////std::swap(scope.bone_buffer, bone_buffer);

      //const auto extent_x = 1u << scope.texture_level - 1;
      //const auto extent_y = 1u << scope.texture_level - 1;

      //auto aaam_array = TextureArrayLDR(FORMAT_R8G8B8A8_SRGB, extent_x, extent_y, texture_0_indices.size(), scope.texture_level);
      //for (const auto& [key, index] : texture_0_indices)
      //{
      //  for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
      //  {
      //    const auto value = glm::u8vec4(
      //      key[0] == -1 ? 255u : texture_all_cache[key[0]].GetItem<glm::u8vec4>(i).r,
      //      key[1] == -1 ? 255u : texture_all_cache[key[1]].GetItem<glm::u8vec4>(i).g,
      //      key[2] == -1 ? 255u : texture_all_cache[key[2]].GetItem<glm::u8vec4>(i).b,
      //      key[3] == -1 ? 000u : texture_all_cache[key[3]].GetItem<glm::u8vec4>(i).b);
      //    aaam_array.Set(index, 0, { &value, 1 }, i);
      //  }
      //}

      //for (auto i = 0ull; i < aaam_array.Layers(); ++i)
      //{
      //  for (auto j = 0ull; j < aaam_array.Levels() - 1; ++j)
      //  {
      //    auto src_offset = aaam_array.Offset(i, j);
      //    auto src_texels = aaam_array.Items(src_offset).first;
      //    auto src_size_x = Mip(extent_x, j);
      //    auto src_size_y = Mip(extent_y, j);

      //    auto dst_offset = aaam_array.Offset(i, j + 1);
      //    auto dst_texels = aaam_array.Items(dst_offset).first;
      //    auto dst_size_x = Mip(extent_x, j + 1);
      //    auto dst_size_y = Mip(extent_y, j + 1);

      //    ResizeLDR(src_texels, src_size_x,  src_size_y, dst_texels,  dst_size_x,  dst_size_y, true);
      //  }
      //}

      //auto snno_array = TextureArrayLDR(FORMAT_R8G8B8A8_UNORM, extent_x, extent_y, texture_1_indices.size(), scope.texture_level);
      //for (const auto& [key, index] : texture_1_indices)
      //{
      //  for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
      //  {
      //    const auto value = glm::u8vec4(
      //      key[0] == -1 ? 000u : texture_all_cache[key[0]].GetItem<glm::u8vec4>(i).g,
      //      key[1] == -1 ? 000u : texture_all_cache[key[1]].GetItem<glm::u8vec4>(i).r,
      //      key[2] == -1 ? 000u : texture_all_cache[key[2]].GetItem<glm::u8vec4>(i).g,
      //      key[3] == -1 ? 255u : texture_all_cache[key[3]].GetItem<glm::u8vec4>(i).b);
      //    snno_array.Set(index, 0, { &value, 1 }, i);
      //  }
      //}

      //for (auto i = 0ull; i < snno_array.Layers(); ++i)
      //{
      //  for (auto j = 0ull; j < snno_array.Levels() - 1; ++j)
      //  {
      //    auto src_texels = snno_array.Items(snno_array.Offset(i, j)).first;
      //    auto src_size_x = Mip(extent_x, j);
      //    auto src_size_y = Mip(extent_y, j);

      //    auto dst_texels = snno_array.Items(snno_array.Offset(i, j + 1)).first;
      //    auto dst_size_x = Mip(extent_x, j + 1);
      //    auto dst_size_y = Mip(extent_y, j + 1);

      //    ResizeLDR(src_texels, src_size_x, src_size_y, dst_texels, dst_size_x, dst_size_y, false);
      //  }
      //}

      //auto eeet_array = TextureArrayLDR(FORMAT_R8G8B8A8_SRGB, extent_x, extent_y, texture_2_indices.size(), scope.texture_level);
      //for (const auto& [key, index] : texture_2_indices)
      //{
      //  for (auto i = 0u; i < size_t(extent_x * extent_y); ++i)
      //  {
      //    const auto value = glm::u8vec4(
      //      key[0] == -1 ? 0u : texture_all_cache[key[0]].GetItem<glm::u8vec4>(i).r,
      //      key[1] == -1 ? 0u : texture_all_cache[key[1]].GetItem<glm::u8vec4>(i).g,
      //      key[2] == -1 ? 0u : texture_all_cache[key[2]].GetItem<glm::u8vec4>(i).b,
      //      key[3] == -1 ? 0u : texture_all_cache[key[3]].GetItem<glm::u8vec4>(i).a);
      //    eeet_array.Set(index, 0, { &value, 1 }, i);
      //  }
      //}

      //for (auto i = 0ull; i < eeet_array.Layers(); ++i)
      //{
      //  for (auto j = 0ull; j < eeet_array.Levels() - 1; ++j)
      //  {
      //    auto src_texels = eeet_array.Items(eeet_array.Offset(i, j)).first;
      //    auto src_size_x = Mip(extent_x, j);
      //    auto src_size_y = Mip(extent_y, j);

      //    auto dst_texels = eeet_array.Items(eeet_array.Offset(i, j + 1)).first;
      //    auto dst_size_x = Mip(extent_x, j + 1);
      //    auto dst_size_y = Mip(extent_y, j + 1);

      //    ResizeLDR(src_texels, src_size_x, src_size_y, dst_texels, dst_size_x, dst_size_y, true);
      //  }
      //}

      //std::swap(scope.aaam_array, aaam_array);
      //std::swap(scope.snno_array, snno_array);
      //std::swap(scope.eeet_array, eeet_array);
      ////std::swap(scope.array_3, array_3);
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