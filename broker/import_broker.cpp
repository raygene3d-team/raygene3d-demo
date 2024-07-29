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
#include "import_broker.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#define OBJ_TRIANGULATE

#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE
//#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tinygltf/tiny_gltf.h>

//#define TINYEXR_IMPLEMENTATION
//#include <tinyexr/tinyexr.h>

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#include <stb/stb_image.h>
//#include <stb/stb_image_write.h>
//#include <stb/stb_image_resize.h>

namespace RayGene3D
{
  std::tuple<std::vector<Vertex>, std::vector<Triangle>, glm::fvec3, glm::fvec3> PopulateInstance(uint32_t idx_count, uint32_t idx_align,
    const glm::fmat3x3& pos_transform, const glm::fmat3x3& nrm_transform, const glm::fmat2x2& tc0_transform,
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

      vertex.pos = pos_transform * glm::fvec3{ pos.x, pos.y, pos.z };
      vertex.nrm = nrm_transform * glm::normalize(glm::fvec3{ nrm.x, nrm.y, nrm.z });
      vertex.tc0 = tc0_transform * glm::f32vec2(tc0.x, tc0.y);

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

      triangle.idx[0] = remap_vertex_fn(vertices, vtx0);
      triangle.idx[1] = remap_vertex_fn(vertices, vtx1);
      triangle.idx[2] = remap_vertex_fn(vertices, vtx2);

      triangles.push_back(triangle);
    }

    return { vertices, triangles, bb_min, bb_max };
  }

  void ImportBroker::Initialize()
  {
    const auto extension = ExtractExtension(file_name);

    if (std::strcmp(extension.c_str(), "obm") == 0)
    {
      ImportOBJM();
    }
    else if (std::strcmp(extension.c_str(), "gltf") == 0)
    {
      ImportGLTF();
    }
  }

  void ImportBroker::Use()
  {}

  void ImportBroker::Discard()
  {
    instances.clear();
    triangles.clear();
    vertices.clear();
  }

  void ImportBroker::ImportGLTF()
  {
    tinygltf::TinyGLTF gltf_ctx;

    tinygltf::Model gltf_model;
    std::string err, warn;
    BLAST_ASSERT(gltf_ctx.LoadASCIIFromFile(&gltf_model, &err, &warn, (path_name + file_name).c_str()));

    const auto gltf_scene = gltf_model.scenes[0];
    std::stack<int, std::vector<int>> node_indices(gltf_scene.nodes);

    std::vector<int> mesh_indices;
    while (!node_indices.empty())
    {
      const auto& gltf_node = gltf_model.nodes[node_indices.top()]; node_indices.pop();
      for (const auto& node_index : gltf_node.children) { node_indices.push(node_index); }
      if (gltf_node.mesh != -1) { mesh_indices.push_back(gltf_node.mesh); }
    }

    const auto instance_convert_fn = [&gltf_model](const tinygltf::Primitive& gltf_primitive, float scale, bool z_up, bool flip_v)
    {
      BLAST_ASSERT(gltf_primitive.mode == TINYGLTF_MODE_TRIANGLES);

      const auto access_buffer_fn = [&gltf_model](const tinygltf::Accessor& accessor)
      {
        const auto& gltf_view = gltf_model.bufferViews[accessor.bufferView];
        const auto length = gltf_view.byteLength;
        const auto offset = gltf_view.byteOffset;

        const auto& gltf_buffer = gltf_model.buffers[gltf_view.buffer];
        const auto data = &gltf_buffer.data[accessor.byteOffset];

        return std::pair{ data + offset, uint32_t(length) };
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

      const auto z_up_transform = glm::fmat3x3(
       -1.0f, 0.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
      );
      const auto nrm_transform = z_up ? z_up_transform : glm::identity<glm::fmat3x3>();
      const auto pos_transform = nrm_transform * scale;

      const auto flip_v_tranform = glm::fmat2x2(
        0.0f, 1.0f,
        1.0f, 0.0f
      );
      const auto tc0_transform = flip_v ? flip_v_tranform : glm::identity<glm::fmat2x2>();

      auto degenerated_geom_tris_count = 0u;
      auto degenerated_wrap_tris_count = 0u;
      const auto [vertices, triangles, bb_min, bb_max] = PopulateInstance(idx_count, idx_stride,
        pos_transform, nrm_transform, tc0_transform,
        pos_data, pos_stride, idx_data, idx_stride,
        nrm_data, nrm_stride, idx_data, idx_stride,
        tc0_data, tc0_stride, idx_data, idx_stride);

      return std::make_tuple(vertices, triangles, bb_min, bb_max);
    };

    std::vector<uint32_t> texture_0_indices;
    std::vector<uint32_t> texture_1_indices;
    std::vector<uint32_t> texture_2_indices;
    std::vector<uint32_t> texture_3_indices;

    const auto tex_reindex_fn = [](std::vector<uint32_t>& tex_ids, uint32_t tex_id)
    {
      if (tex_id == -1)
      {
        return uint32_t(-1);
      }

      const auto tex_iter = std::find_if(tex_ids.cbegin(), tex_ids.cend(), [&tex_id](const auto& index) { return tex_id == index; });
      const auto tex_index = tex_iter == tex_ids.cend() ? uint32_t(tex_ids.size()) : uint32_t(tex_iter - tex_ids.cbegin());
      if (tex_index == tex_ids.size())
      {
        tex_ids.push_back(tex_id);
      }
      return tex_index;
    };

    for (uint32_t i = 0; i < uint32_t(mesh_indices.size()); ++i)
    {
      const auto& gltf_mesh = gltf_model.meshes[mesh_indices[i]];
      for (uint32_t k = 0; k < uint32_t(gltf_mesh.primitives.size()); ++k)
      {
        const auto& gltf_primitive = gltf_mesh.primitives[k];
        const auto [instance_vertices, instance_triangles, instance_bb_min, instance_bb_max] = 
          instance_convert_fn(gltf_primitive, position_scale, coordinate_flip, false);

        if (instance_vertices.empty() || instance_triangles.empty()) continue;

        Instance instance;
        instance.transform = glm::identity<glm::fmat3x4>();
        instance.geometry_idx = uint32_t(instances.size());
        instance.bb_min = instance_bb_min;
        instance.bb_max = instance_bb_max;
        //instance.debug_color{ 0.0f, 0.0f, 0.0f };

        const auto& gltf_material = gltf_model.materials[gltf_primitive.material];
        const auto texture_0_id = gltf_material.pbrMetallicRoughness.baseColorTexture.index;
        instance.texture0_idx = texture_0_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_0_indices, texture_0_id);
        const auto texture_1_id = gltf_material.emissiveTexture.index;
        instance.texture1_idx = texture_1_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_1_indices, texture_1_id);
        const auto texture_2_id = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
        instance.texture2_idx = texture_2_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_2_indices, texture_2_id);
        const auto texture_3_id = gltf_material.normalTexture.index;
        instance.texture3_idx = texture_3_id == -1 ? uint32_t(-1) : tex_reindex_fn(texture_3_indices, texture_3_id);

        instance.vert_count = uint32_t(instance_vertices.size());
        instance.vert_offset = uint32_t(vertices.size());
        instance.prim_count = uint32_t(instance_triangles.size());
        instance.prim_offset = uint32_t(triangles.size());

        std::copy(instance_vertices.begin(), instance_vertices.end(), std::back_inserter(vertices));
        std::copy(instance_triangles.begin(), instance_triangles.end(), std::back_inserter(triangles));

        instances.push_back(instance);

        BLAST_LOG("Instance %d: Added vert/prim: %d/%d", instance.geometry_idx, instance.vert_count, instance.prim_count);
      }
    }

    const auto tex_prepare_fn = [&gltf_model](uint32_t texture_index)
    {
      const auto image_index = gltf_model.textures[texture_index].source;

      const auto tex_x = gltf_model.images[image_index].width;
      const auto tex_y = gltf_model.images[image_index].height;
      const auto tex_n = gltf_model.images[image_index].component;
      const auto tex_data = gltf_model.images[image_index].image.data();

      auto raw = Raw(tex_x * tex_y * uint32_t(sizeof(glm::u8vec4)));

      for (uint32_t i = 0; i < tex_x * tex_y; ++i)
      {
        const auto r = tex_n > 0 ? tex_data[i * tex_n + 0] : 0; //0xFF;
        const auto g = tex_n > 1 ? tex_data[i * tex_n + 1] : r; //0xFF;
        const auto b = tex_n > 2 ? tex_data[i * tex_n + 2] : r; //0xFF;
        const auto a = tex_n > 3 ? tex_data[i * tex_n + 3] : r; //0xFF;
        raw.SetElement<glm::u8vec4>({r, g, b, a}, i);
      }

      return std::make_tuple(std::move(raw), uint32_t(tex_x), uint32_t(tex_y));
    };

    textures_0.resize(texture_0_indices.size());
    for (uint32_t i = 0; i < uint32_t(texture_0_indices.size()); ++i)
    {
      const auto texture_0_index = texture_0_indices[i];
      textures_0[i] = std::move(tex_prepare_fn(texture_0_index));
    }

    textures_1.resize(texture_1_indices.size());
    for (uint32_t i = 0; i < uint32_t(texture_1_indices.size()); ++i)
    {
      const auto texture_1_index = texture_1_indices[i];
      textures_1[i] = std::move(tex_prepare_fn(texture_1_index));
    }

    textures_2.resize(texture_2_indices.size());
    for (uint32_t i = 0; i < uint32_t(texture_2_indices.size()); ++i)
    {
      const auto texture_2_index = texture_2_indices[i];
      textures_2[i] = std::move(tex_prepare_fn(texture_2_index));
    }

    textures_3.resize(texture_3_indices.size());
    for (uint32_t i = 0; i < uint32_t(texture_3_indices.size()); ++i)
    {
      const auto texture_3_index = texture_3_indices[i];
      textures_3[i] = std::move(tex_prepare_fn(texture_3_index));
    }
  }

  void ImportBroker::ImportOBJM()
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

        const auto z_up_transform = glm::fmat3x3(
          1.0f, 0.0f, 0.0f,
          0.0f, 0.0f,-1.0f,
          0.0f, 1.0f, 0.0f
        );
        const auto nrm_transform = coordinate_flip ? z_up_transform : glm::identity<glm::fmat3x3>();
        const auto pos_transform = nrm_transform * position_scale;

        const auto flip_v_tranform = glm::fmat2x2(
          1.0f, 0.0f,
          0.0f,-1.0f
        );
        const auto tc0_transform = true ? flip_v_tranform : glm::identity<glm::fmat2x2>();

        const auto [instance_vertices, instance_triangles, instance_bb_min, instance_bb_max] = PopulateInstance(idx_count, idx_align,
          pos_transform, nrm_transform, tc0_transform,
          pos_data, pos_stride, pos_idx_data, pos_idx_stride,
          nrm_data, nrm_stride, nrm_idx_data, nrm_idx_stride,
          tc0_data, tc0_stride, tc0_idx_data, tc0_idx_stride);

        if (instance_vertices.empty() || instance_triangles.empty()) continue;

        const auto vertices_count = uint32_t(instance_vertices.size());
        const auto triangles_count = uint32_t(instance_triangles.size());
        BLAST_LOG("Instance %d: Added vert/prim: %d/%d", instances.size(), vertices_count, triangles_count);

        Instance instance;
        instance.transform = glm::identity<glm::fmat3x4>();
        instance.geometry_idx = uint32_t(instances.size());
        instance.bb_min = instance_bb_min;
        instance.bb_max = instance_bb_max;
        //instance.debug_color{ 0.0f, 0.0f, 0.0f };

        const auto& obj_material = obj_materials[material_id.first];

        const auto debug = false;
        if (debug)
        {
          instance.emission = glm::vec3(0.0f, 0.0f, 0.0f);
          instance.intensity = 0.0f;
          //material.ambient = glm::vec3(obj_material.ambient[0], obj_material.ambient[1], obj_material.ambient[2]);
          //material.dissolve = obj_material.dissolve;
          instance.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
          instance.shininess = 1.0f;
          instance.specular = glm::vec3(0.0f, 0.0f, 0.0f);
          instance.alpha = 1.0f;
          //instance.transmittance = glm::vec3(0.0f, 0.0f, 0.0f) * (1.0f - obj_material.dissolve);
          //instance.ior = 1.0f;
        }
        else
        {

          instance.emission = glm::vec3(obj_material.emission[0], obj_material.emission[1], obj_material.emission[2]);
          instance.intensity = 10.0f;
          //material.ambient = glm::vec3(obj_material.ambient[0], obj_material.ambient[1], obj_material.ambient[2]);
          //material.dissolve = obj_material.dissolve;
          instance.diffuse = glm::vec3(obj_material.diffuse[0], obj_material.diffuse[1], obj_material.diffuse[2]);
          instance.shininess = obj_material.shininess;
          instance.specular = glm::vec3(obj_material.specular[0], obj_material.specular[1], obj_material.specular[2]);
          instance.alpha = 1.0f; // -obj_material.dissolve;
          //instance.transmittance = glm::vec3(obj_material.transmittance[0], obj_material.transmittance[1], obj_material.transmittance[2]) * (1.0f - obj_material.dissolve);
          //instance.ior = obj_material.ior;

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

          switch (obj_material.illum)
          {
          case 3: // mirror
            instance.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
            instance.specular = glm::vec3(1.0f, 1.0f, 1.0f);
            instance.shininess = float(1 << 16);
            break;
          case 7: // glass
            instance.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
            instance.specular = glm::vec3(0.0f, 0.0f, 0.0f);
            instance.alpha = 1.5f;
            break;
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
  }

  void ImportBroker::Export(std::shared_ptr<Property>& property) const
  {
    auto root = std::shared_ptr<Property>(new Property(Property::TYPE_OBJECT));
    //property->setSetValue(Property::object());
  
    {
      const auto data = instances.data();
      const auto stride = uint32_t(sizeof(Instance));
      const auto count = uint32_t(instances.size());
      auto raw = Raw({ data, stride * count });
      const auto property = CreateBufferProperty({ &raw, 1u }, stride, count);
      root->SetObjectItem("instances", property);
    }

    {
      const auto data = triangles.data();
      const auto stride = uint32_t(sizeof(Triangle));
      const auto count = uint32_t(triangles.size());
      auto raw = Raw({ data, stride * count });
      const auto property = CreateBufferProperty({ &raw, 1u }, stride, count);
      root->SetObjectItem("triangles", property);
    }

    {
      const auto data = vertices.data();
      const auto stride = uint32_t(sizeof(Vertex));
      const auto count = uint32_t(vertices.size());
      auto raw = Raw({ data, stride * count });
      const auto property = CreateBufferProperty({ &raw, 1u }, stride, count);
      root->SetObjectItem("vertices", property);
    }
  
    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_0.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] = 
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_0[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty() 
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_0.size()));
      root->SetObjectItem("textures_0", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_1.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_1[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_1.size()));
      root->SetObjectItem("textures_1", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_2.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_2[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_2.size()));
      root->SetObjectItem("textures_2", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_3.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_3[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_3.size()));
      root->SetObjectItem("textures_3", property);
    }

    property = root;
  }


  //std::shared_ptr<Property> ImportAsPanoEXR(const std::string& path, const std::string& name, float exposure, uint32_t mipmaps)
  //{
  //  int32_t src_tex_x = 0;
  //  int32_t src_tex_y = 0;
  //  int32_t src_tex_n = 4;
  //  float* src_tex_data = nullptr;
  //  BLAST_ASSERT(0 == LoadEXR(&src_tex_data, &src_tex_x, &src_tex_y, (name).c_str(), nullptr));

  //  int32_t dst_tex_x = src_tex_x;
  //  int32_t dst_tex_y = src_tex_y;
  //  int32_t dst_tex_n = 4;
  //  float* dst_tex_data = new float[dst_tex_x * dst_tex_y * dst_tex_n];
  //  for (uint32_t j = 0; j < src_tex_x * src_tex_y; ++j)
  //  {
  //    const auto r = src_tex_n > 0 ? src_tex_data[j * src_tex_n + 0] : 0; //0xFF;
  //    const auto g = src_tex_n > 1 ? src_tex_data[j * src_tex_n + 1] : r; //0xFF;
  //    const auto b = src_tex_n > 2 ? src_tex_data[j * src_tex_n + 2] : r; //0xFF;
  //    const auto a = src_tex_n > 3 ? src_tex_data[j * src_tex_n + 3] : r; //0xFF;
  //    dst_tex_data[j * dst_tex_n + 0] = r * exposure;
  //    dst_tex_data[j * dst_tex_n + 1] = g * exposure;
  //    dst_tex_data[j * dst_tex_n + 2] = b * exposure;
  //    dst_tex_data[j * dst_tex_n + 3] = a * exposure;
  //  }
  //  delete[] src_tex_data;

  //  src_tex_data = dst_tex_data;
  //  src_tex_x = dst_tex_x;
  //  src_tex_y = dst_tex_y;
  //  src_tex_n = dst_tex_n;

  //  const auto get_pot_fn = [](int32_t value)
  //  {
  //    int32_t power = 0;
  //    while ((1 << power) < value) ++power;
  //    return power;
  //  };

  //  const auto pow_tex_x = get_pot_fn(src_tex_x);
  //  const auto pow_tex_y = get_pot_fn(src_tex_y);
  //  const auto pow_delta = std::abs(pow_tex_x - pow_tex_y);

  //  //const uint32_t tex_x = (1 << (pow_tex_x > pow_tex_y ? mipmap_count + pow_delta : mipmap_count)) - 1;
  //  //const uint32_t tex_y = (1 << (pow_tex_y > pow_tex_x ? mipmap_count + pow_delta : mipmap_count)) - 1;
  //  //const uint32_t tex_n = 4;

  //  const auto pow_x = pow_tex_x > pow_tex_y ? mipmaps + pow_delta : mipmaps;
  //  const auto pow_y = pow_tex_y > pow_tex_x ? mipmaps + pow_delta : mipmaps;
  //  const auto extent_x = 1 << pow_x;
  //  const auto extent_y = 1 << pow_y;
  //  const auto count = uint32_t((extent_x * extent_y - 1) / 3);
  //  const auto stride = uint32_t(sizeof(glm::f32vec4));
  //  const auto data = new float[count * 4];

  //  dst_tex_x = 1 << (pow_x - 1);
  //  dst_tex_y = 1 << (pow_y - 1);
  //  dst_tex_n = src_tex_n;
  //  dst_tex_data = data;
  //  stbir_resize_float(src_tex_data, src_tex_x, src_tex_y, 0, dst_tex_data, dst_tex_x, dst_tex_y, 0, dst_tex_n);
  //  delete[] src_tex_data;

  //  //const auto size_x_property = std::shared_ptr<Property>(new Property());
  //  //size_x_property->SetValue(uint32_t(dst_tex_x));

  //  //const auto size_y_property = std::shared_ptr<Property>(new Property());
  //  //size_y_property->SetValue(uint32_t(dst_tex_y));

  //  //const auto mipmaps_property = std::shared_ptr<Property>(new Property());
  //  //mipmaps_property->SetValue(uint32_t(mipmaps));

  //  src_tex_data = dst_tex_data;
  //  src_tex_x = dst_tex_x;
  //  src_tex_y = dst_tex_y;
  //  src_tex_n = dst_tex_n;

  //  for (uint32_t i = 1; i < mipmaps; ++i)
  //  {
  //    dst_tex_x = src_tex_x >> 1;
  //    dst_tex_y = src_tex_y >> 1;
  //    dst_tex_n = src_tex_n;
  //    dst_tex_data += src_tex_x * src_tex_y * src_tex_n;
  //    stbir_resize_float(src_tex_data, src_tex_x, src_tex_y, 0, dst_tex_data, dst_tex_x, dst_tex_y, 0, dst_tex_n);

  //    src_tex_data = dst_tex_data;
  //    src_tex_x = dst_tex_x;
  //    src_tex_y = dst_tex_y;
  //    src_tex_n = dst_tex_n;
  //  }

  //  const auto root = CreateTextureProperty({ data, stride * count }, extent_x, extent_y, 1, FORMAT_R32G32B32_FLOAT, mipmaps);
  //  delete[] data;

  //  return root;
  //}

  //std::shared_ptr<Property> ImportAsCubeMapEXR(const std::string& path, const std::string& name, float exposure, uint32_t mipmaps)
  //{
  //  const auto cubemap_layer_counter = 6u;
  //  const auto cubemap_layer_size = 1u << (mipmaps - 1);

  //  auto texture_property = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
  //  texture_property->SetArraySize(cubemap_layer_counter);

  //  enum CUBEMAP_LAYERS
  //  {
  //    CUBEMAP_POSITIVE_X,
  //    CUBEMAP_NEGATIVE_X,
  //    CUBEMAP_POSITIVE_Y,
  //    CUBEMAP_NEGATIVE_Y,
  //    CUBEMAP_POSITIVE_Z,
  //    CUBEMAP_NEGATIVE_Z
  //  };

  //  const auto uv_to_cube = [](float u, float v, uint32_t layer)
  //  {
  //    glm::fvec3 text_coord = { 0.0f, 0.0f, 0.0f };

  //    switch (layer)
  //    {
  //    case CUBEMAP_POSITIVE_X:
  //      text_coord = { 1.0f, -(2.0f * v - 1.0f), -(2.0f * u - 1.0f) };
  //      break;
  //    case CUBEMAP_NEGATIVE_X:
  //      text_coord = { -1.0f, -(2.0f * v - 1.0f), (2.0f * u - 1.0f) };
  //      break;
  //    case CUBEMAP_POSITIVE_Y:
  //      text_coord = { (2.0f * u - 1.0f), 1.0f, (2.0f * v - 1.0f) };
  //      break;
  //    case CUBEMAP_NEGATIVE_Y:
  //      text_coord = { (2.0f * u - 1.0f), -1.0f, -(2.0f * v - 1.0f) };
  //      break;
  //    case CUBEMAP_POSITIVE_Z:
  //      text_coord = { (2.0f * u - 1.0f), -(2.0f * v - 1.0f), 1.0f };
  //      break;
  //    case CUBEMAP_NEGATIVE_Z:
  //      text_coord = { -(2.0f * u - 1.0f), -(2.0f * v - 1.0f), -1.0f };
  //      break;
  //    }
  //    return glm::normalize(text_coord);
  //  };

  //  const auto cube_to_pano = [](const glm::fvec3& cube_tex_coord)
  //  {
  //    glm::fvec2 tex_coord = { 0.0f, 0.0f };
  //    tex_coord.x = 0.5f * atan2(cube_tex_coord.x, cube_tex_coord.z) / glm::pi<float>() + 0.5f;
  //    tex_coord.y = -asin(cube_tex_coord.y) / glm::pi<float>() + 0.5f;
  //    return tex_coord;
  //  };

  //  const auto src_tex_n = 4;
  //  const auto dst_tex_n = 4;
  //  auto pano_tex_x = 0;
  //  auto pano_tex_y = 0;
  //  float* pano_tex_data = nullptr;
  //  BLAST_ASSERT(0 == LoadEXR(&pano_tex_data, &pano_tex_x, &pano_tex_y, (name).c_str(), nullptr));

  //  for (uint32_t i = 0; i < cubemap_layer_counter; ++i)
  //  {
  //    auto dst_tex_x = cubemap_layer_size;
  //    auto dst_tex_y = cubemap_layer_size;
  //    const auto texel_pow = mipmaps;

  //    const auto texel_count = uint32_t(((1 << texel_pow) * (1 << texel_pow) - 1) / 3);
  //    float* texel_data = new float[texel_count * dst_tex_n];

  //    {
  //      auto dst_tex_data = texel_data;
  //      for (uint32_t j = 0; j < dst_tex_x; ++j)
  //      {
  //        for (uint32_t k = 0; k < dst_tex_y; ++k)
  //        {
  //          const float u = (float(j) + 0.5f) / dst_tex_x;
  //          const float v = (float(k) + 0.5f) / dst_tex_y;

  //          const auto cube_text_coord = uv_to_cube(u, v, i);
  //          const auto pano_text_coord = cube_to_pano(cube_text_coord);

  //          const auto x = uint32_t(pano_tex_x * pano_text_coord.x);
  //          const auto y = uint32_t(pano_tex_y * pano_text_coord.y);
  //          const auto texel_num = y * pano_tex_x + x;

  //          const auto index = j + k * dst_tex_x;

  //          dst_tex_data[index * dst_tex_n + 0] = pano_tex_data[texel_num * src_tex_n + 0] * exposure;
  //          dst_tex_data[index * dst_tex_n + 1] = pano_tex_data[texel_num * src_tex_n + 1] * exposure;
  //          dst_tex_data[index * dst_tex_n + 2] = pano_tex_data[texel_num * src_tex_n + 2] * exposure;
  //          dst_tex_data[index * dst_tex_n + 3] = pano_tex_data[texel_num * src_tex_n + 3] * exposure;
  //        }
  //      }
  //    }

  //    {
  //      auto src_tex_x = cubemap_layer_size;
  //      auto src_tex_y = cubemap_layer_size;
  //      auto dst_tex_data = texel_data;
  //      auto src_tex_data = dst_tex_data;

  //      auto offset = 0u;
  //      for (uint32_t i = 1; i < mipmaps; ++i)
  //      {
  //        dst_tex_x = src_tex_x >> 1;
  //        dst_tex_y = src_tex_y >> 1;
  //        dst_tex_data += src_tex_x * src_tex_y * dst_tex_n;
  //        offset += src_tex_x * src_tex_y * dst_tex_n;
  //        stbir_resize_float(src_tex_data, src_tex_x, src_tex_y, 0, dst_tex_data, dst_tex_x, dst_tex_y, 0, dst_tex_n);

  //        src_tex_data = dst_tex_data;
  //        src_tex_x = dst_tex_x;
  //        src_tex_y = dst_tex_y;
  //      }
  //    }

  //    const auto texels_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
  //    {
  //      const auto texel_stride = uint32_t(sizeof(float) * dst_tex_n);
  //      texels_property->RawAllocate(texel_count * texel_stride);
  //      texels_property->SetRawBytes({ texel_data, texel_count * texel_stride }, 0);
  //    }

  //    texture_property->SetArrayItem(i, texels_property);
  //    delete[] texel_data;
  //  }

  //  delete[] pano_tex_data;

  //  return texture_property;
  //}

  ImportBroker::ImportBroker(Wrap& wrap)
    : Broker("import_broker", wrap)
  {}

  ImportBroker::~ImportBroker()
  {}
}