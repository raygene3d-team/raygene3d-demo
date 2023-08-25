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


#include "spark.h"

namespace RayGene3D
{
  void Spark::InitializeShadowMap()
  {
    root.GetCore()->GetDevice()->CreateResource("spark_shadow_map",
      Resource::Tex2DDesc
      {
        FORMAT_D16_UNORM,
        shadow_resolution,
        shadow_resolution,
        6,
        1,
        Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE)
      },
      Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE)
    );

    //shadow_map->SetType(Resource::TYPE_IMAGE2D);
    //shadow_map->SetExtentX(shadow_resolution);
    //shadow_map->SetExtentY(shadow_resolution);
    //shadow_map->SetLayers(6);
    //shadow_map->SetMipmaps(1);
    //shadow_map->SetFormat(FORMAT_D16_UNORM);
    //shadow_map->SetUsage(Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE));
    //shadow_map->SetHint(Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE));
    //shadow_map->Initialize();
  }

  void Spark::InitializeColorTarget()
  {
    root.GetCore()->GetDevice()->CreateResource("spark_color_target",
      Resource::Tex2DDesc
      {
        FORMAT_R11G11B10_FLOAT,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
        1,
        1,
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE)
      }
    );

    //const auto extent_x = prop_extent_x->GetUint();
    //const auto extent_y = prop_extent_y->GetUint();
    //
    //color_target->SetType(Resource::TYPE_IMAGE2D);
    //color_target->SetExtentX(extent_x);
    //color_target->SetExtentY(extent_y);
    //color_target->SetLayers(1);
    //color_target->SetMipmaps(1);
    //color_target->SetFormat(FORMAT_R11G11B10_FLOAT);
    //color_target->SetUsage(Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE));
    //color_target->Initialize();
  }

  void Spark::InitializeDepthTarget()
  {
    root.GetCore()->GetDevice()->CreateResource("spark_depth_target",
      Resource::Tex2DDesc
      {
        FORMAT_D32_FLOAT,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
        1,
        1,
        Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE)
      }
    );

    //const auto extent_x = prop_extent_x->GetUint();
    //const auto extent_y = prop_extent_y->GetUint();
    //
    //depth_target->SetType(Resource::TYPE_IMAGE2D);
    //depth_target->SetExtentX(extent_x);
    //depth_target->SetExtentY(extent_y);
    //depth_target->SetLayers(1);
    //depth_target->SetMipmaps(1);
    //depth_target->SetFormat(FORMAT_D32_FLOAT);
    //depth_target->SetUsage(Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE));
    //depth_target->Initialize();
  }

  void Spark::InitializeScreenData()
  {
    root.GetCore()->GetDevice()->CreateResource("spark_screen_data",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Screen)),
        1,
        Usage(USAGE_CONSTANT_DATA)
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );

    //screen_data->SetType(Resource::TYPE_BUFFER);
    //screen_data->SetStride(uint32_t(sizeof(Screen)));
    //screen_data->SetCount(1);
    //screen_data->SetUsage(USAGE_CONSTANT_DATA);
    //screen_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
    //screen_data->Initialize();
  }

  void Spark::InitializeCameraData()
  {
    root.GetCore()->GetDevice()->CreateResource("spark_camera_data",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Frustum)),
        1,
        Usage(USAGE_CONSTANT_DATA)
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );

    //camera_data->SetType(Resource::TYPE_BUFFER);
    //camera_data->SetStride(uint32_t(sizeof(Frustum)));
    //camera_data->SetCount(1);
    //camera_data->SetUsage(USAGE_CONSTANT_DATA);
    //camera_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
    //camera_data->Initialize();
  }

  void Spark::InitializeShadowData()
  {
    root.GetCore()->GetDevice()->CreateResource("spark_shadow_data",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Frustum)),
        6,
        Usage(USAGE_CONSTANT_DATA)
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
    );

    //shadow_data->SetType(Resource::TYPE_BUFFER);
    //shadow_data->SetStride(uint32_t(sizeof(Frustum)));
    //shadow_data->SetCount(6);
    //shadow_data->SetUsage(USAGE_CONSTANT_DATA);
    //shadow_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
    //shadow_data->Initialize();
  }

  void Spark::InitializeSceneInstances()
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_instances->GetRawBytes(0),
    };

    root.GetCore()->GetDevice()->CreateResource("spark_scene_instances",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Instance)),
        count,
        Usage(USAGE_CONSTANT_DATA)
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );

    //scene_instances->SetType(Resource::TYPE_BUFFER);
    //scene_instances->SetStride(uint32_t(sizeof(Instance)));
    //scene_instances->SetCount(count);
    //scene_instances->SetInteropCount(1);
    //scene_instances->SetInteropItem(0, prop_instances->GetRawBytes(0));
    //scene_instances->SetUsage(USAGE_CONSTANT_DATA);
    //scene_instances->Initialize();
  }

  void Spark::InitializeSceneTriangles()
  {
    const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_triangles->GetRawBytes(0),
    };

    root.GetCore()->GetDevice()->CreateResource("spark_scene_triangles",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Triangle)),
        count,
        Usage(USAGE_INDEX_ARRAY)
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );

    //scene_triangles->SetType(Resource::TYPE_BUFFER);
    //scene_triangles->SetStride(uint32_t(sizeof(Triangle)));
    //scene_triangles->SetCount(count);
    //scene_triangles->SetInteropCount(1);
    //scene_triangles->SetInteropItem(0, prop_triangles->GetRawBytes(0));
    //scene_triangles->SetUsage(USAGE_INDEX_ARRAY);
    //scene_triangles->Initialize();
  }

  void Spark::InitializeSceneVertices0()
  {
    const auto [data, count] = prop_vertices0->GetTypedBytes<Vertex0>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_vertices0->GetRawBytes(0),
    };

    root.GetCore()->GetDevice()->CreateResource("spark_scene_vertices0",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Vertex0)),
        count,
        Usage(USAGE_VERTEX_ARRAY)
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );

    //scene_vertices0->SetType(Resource::TYPE_BUFFER);
    //scene_vertices0->SetStride(uint32_t(sizeof(Vertex0)));
    //scene_vertices0->SetCount(count);
    //scene_vertices0->SetInteropCount(1);
    //scene_vertices0->SetInteropItem(0, prop_vertices0->GetRawBytes(0));
    //scene_vertices0->SetUsage(USAGE_VERTEX_ARRAY);
    //scene_vertices0->Initialize();
  }

  void Spark::InitializeSceneVertices1()
  {
    const auto [data, count] = prop_vertices1->GetTypedBytes<Vertex1>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_vertices1->GetRawBytes(0),
    };

    root.GetCore()->GetDevice()->CreateResource("spark_scene_vertices1",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Vertex1)),
        count,
        Usage(USAGE_VERTEX_ARRAY)
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );

    //scene_vertices1->SetType(Resource::TYPE_BUFFER);
    //scene_vertices1->SetStride(uint32_t(sizeof(Vertex1)));
    //scene_vertices1->SetCount(count);
    //scene_vertices1->SetInteropCount(1);
    //scene_vertices1->SetInteropItem(0, prop_vertices1->GetRawBytes(0));
    //scene_vertices1->SetUsage(USAGE_VERTEX_ARRAY);
    //scene_vertices1->Initialize();
  }

  void Spark::InitializeSceneVertices2()
  {
    const auto [data, count] = prop_vertices2->GetTypedBytes<Vertex2>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_vertices2->GetRawBytes(0),
    };

    root.GetCore()->GetDevice()->CreateResource("spark_scene_vertices2",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Vertex2)),
        count,
        Usage(USAGE_VERTEX_ARRAY)
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );

    //scene_vertices2->SetType(Resource::TYPE_BUFFER);
    //scene_vertices2->SetStride(uint32_t(sizeof(Vertex2)));
    //scene_vertices2->SetCount(count);
    //scene_vertices2->SetInteropCount(1);
    //scene_vertices2->SetInteropItem(0, prop_vertices2->GetRawBytes(0));
    //scene_vertices2->SetUsage(USAGE_VERTEX_ARRAY);
    //scene_vertices2->Initialize();
  }

  void Spark::InitializeSceneVertices3()
  {
    const auto [data, count] = prop_vertices3->GetTypedBytes<Vertex3>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_vertices3->GetRawBytes(0),
    };

    root.GetCore()->GetDevice()->CreateResource("spark_scene_vertices3",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Vertex3)),
        count,
        Usage(USAGE_VERTEX_ARRAY)
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );

    //scene_vertices3->SetType(Resource::TYPE_BUFFER);
    //scene_vertices3->SetStride(uint32_t(sizeof(Vertex3)));
    //scene_vertices3->SetCount(count);
    //scene_vertices3->SetInteropCount(1);
    //scene_vertices3->SetInteropItem(0, prop_vertices3->GetRawBytes(0));
    //scene_vertices3->SetUsage(USAGE_VERTEX_ARRAY);
    //scene_vertices3->Initialize();
  }

  void Spark::InitializeSceneTextures0()
  {
    const auto layers = prop_textures0->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures0->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures0->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures0->GetArrayItem(i)->GetRawBytes(0));
    }

    root.GetCore()->GetDevice()->CreateResource("spark_scene_textures0",
      Resource::Tex2DDesc
      {
        format,
        size_x,
        size_y,
        layers,
        mipmaps,
        Usage(USAGE_SHADER_RESOURCE)
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );

    //scene_textures0->SetType(Resource::TYPE_IMAGE2D);
    //scene_textures0->SetExtentX(size_x);
    //scene_textures0->SetExtentY(size_y);
    //scene_textures0->SetExtentZ(1);
    //scene_textures0->SetHint(Resource::HINT_LAYERED_IMAGE);
    //scene_textures0->SetLayers(layers);
    //scene_textures0->SetMipmaps(mipmaps);
    //scene_textures0->SetFormat(format);
    //scene_textures0->SetInteropCount(layers);
    //for (uint32_t i = 0; i < layers; ++i)
    //{ 
    //  scene_textures0->SetInteropItem(i, prop_textures0->GetArrayItem(i)->GetRawBytes(0));
    //}
    //scene_textures0->SetUsage(USAGE_SHADER_RESOURCE);
    //scene_textures0->Initialize();
  }

  void Spark::InitializeSceneTextures1()
  {
    const auto layers = prop_textures1->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures1->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures1->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures1->GetArrayItem(i)->GetRawBytes(0));
    }

    root.GetCore()->GetDevice()->CreateResource("spark_scene_textures1",
      Resource::Tex2DDesc
      {
        format,
        size_x,
        size_y,
        layers,
        mipmaps,
        Usage(USAGE_SHADER_RESOURCE)
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );

    //scene_textures1->SetType(Resource::TYPE_IMAGE2D);
    //scene_textures1->SetExtentX(size_x);
    //scene_textures1->SetExtentY(size_y);
    //scene_textures1->SetExtentZ(1);
    //scene_textures1->SetHint(Resource::HINT_LAYERED_IMAGE);
    //scene_textures1->SetLayers(layers);
    //scene_textures1->SetMipmaps(mipmaps);
    //scene_textures1->SetFormat(format);
    //scene_textures1->SetInteropCount(layers);
    //for (uint32_t i = 0; i < layers; ++i)
    //{ 
    //  scene_textures1->SetInteropItem(i, prop_textures1->GetArrayItem(i)->GetRawBytes(0));
    //}
    //scene_textures1->SetUsage(USAGE_SHADER_RESOURCE);
    //scene_textures1->Initialize();
  }

  void Spark::InitializeSceneTextures2()
  {
    const auto layers = prop_textures2->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures2->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures2->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures2->GetArrayItem(i)->GetRawBytes(0));
    }

    root.GetCore()->GetDevice()->CreateResource("spark_scene_textures2",
      Resource::Tex2DDesc
      {
        format,
        size_x,
        size_y,
        layers,
        mipmaps,
        Usage(USAGE_SHADER_RESOURCE)
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );

    //scene_textures2->SetType(Resource::TYPE_IMAGE2D);
    //scene_textures2->SetExtentX(size_x);
    //scene_textures2->SetExtentY(size_y);
    //scene_textures2->SetExtentZ(1);
    //scene_textures2->SetHint(Resource::HINT_LAYERED_IMAGE);
    //scene_textures2->SetLayers(layers);
    //scene_textures2->SetMipmaps(mipmaps);
    //scene_textures2->SetFormat(format);
    //scene_textures2->SetInteropCount(layers);
    //for (uint32_t i = 0; i < layers; ++i) 
    //{ 
    //  scene_textures2->SetInteropItem(i, prop_textures2->GetArrayItem(i)->GetRawBytes(0));
    //}
    //scene_textures2->SetUsage(USAGE_SHADER_RESOURCE);
    //scene_textures2->Initialize();
  }

  void Spark::InitializeSceneTextures3()
  {
    const auto layers = prop_textures3->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures3->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures3->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures3->GetArrayItem(i)->GetRawBytes(0));
    }

    root.GetCore()->GetDevice()->CreateResource("spark_scene_textures3",
      Resource::Tex2DDesc
      {
        format,
        size_x,
        size_y,
        layers,
        mipmaps,
        Usage(USAGE_SHADER_RESOURCE)
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );

    //scene_textures3->SetType(Resource::TYPE_IMAGE2D);
    //scene_textures3->SetExtentX(size_x);
    //scene_textures3->SetExtentY(size_y);
    //scene_textures3->SetExtentZ(1);
    //scene_textures3->SetHint(Resource::HINT_LAYERED_IMAGE);
    //scene_textures3->SetLayers(layers);
    //scene_textures3->SetMipmaps(mipmaps);
    //scene_textures3->SetFormat(format);
    //scene_textures3->SetInteropCount(layers);
    //for (uint32_t i = 0; i < layers; ++i) 
    //{
    //  scene_textures3->SetInteropItem(i, prop_textures3->GetArrayItem(i)->GetRawBytes(0));
    //}
    //scene_textures3->SetUsage(USAGE_SHADER_RESOURCE);
    //scene_textures3->Initialize();
  }

  void Spark::InitializeLightMaps()
  {
    const auto layers = prop_lightmaps->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_lightmaps->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_lightmaps->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_lightmaps->GetArrayItem(i)->GetRawBytes(0));
    }

    root.GetCore()->GetDevice()->CreateResource("spark_light_maps",
      Resource::Tex2DDesc
      {
        format,
        size_x,
        size_y,
        layers,
        mipmaps,
        Usage(USAGE_SHADER_RESOURCE)
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );

    //light_maps->SetType(Resource::TYPE_IMAGE2D);
    //light_maps->SetExtentX(size_x);
    //light_maps->SetExtentY(size_y);
    //light_maps->SetExtentZ(1);
    //light_maps->SetHint(Resource::HINT_LAYERED_IMAGE);
    //light_maps->SetLayers(layers);
    //light_maps->SetMipmaps(mipmaps);
    //light_maps->SetFormat(format);
    //light_maps->SetInteropCount(layers);
    //for (uint32_t i = 0; i < layers; ++i)
    //{ 
    //  light_maps->SetInteropItem(i, prop_lightmaps->GetArrayItem(i)->GetRawBytes(0));
    //}
    //light_maps->SetUsage(USAGE_SHADER_RESOURCE);
    //light_maps->Initialize();
  }

  void Spark::InitializeSkyboxVertices()
  {
    static const std::array<glm::f32vec4, 4> quad_vtx = {
      glm::f32vec4(-1.0f, 1.0f, 0.0f, 0.0f),
      glm::f32vec4(1.0f, 1.0f, 1.0f, 0.0f),
      glm::f32vec4(-1.0f,-1.0f, 0.0f, 1.0f),
      glm::f32vec4(1.0f,-1.0f, 1.0f, 1.0f),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_vtx.data(), uint32_t(quad_vtx.size() * sizeof(glm::f32vec4)) },
    };

    root.GetCore()->GetDevice()->CreateResource("spark_skybox_vertices",
      Resource::BufferDesc
      {
        uint32_t(sizeof(glm::f32vec4)),
        uint32_t(quad_vtx.size()),
        Usage(USAGE_VERTEX_ARRAY)
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );

    //skybox_vertices->SetType(Resource::TYPE_BUFFER);
    //skybox_vertices->SetStride(uint32_t(sizeof(glm::f32vec4)));
    //skybox_vertices->SetCount(uint32_t(quad_vtx.size()));
    //skybox_vertices->SetInteropCount(1);
    //skybox_vertices->SetInteropItem(0, std::pair(quad_vtx.data(), uint32_t(quad_vtx.size() * sizeof(glm::f32vec4))));
    //skybox_vertices->SetUsage(Usage::USAGE_VERTEX_ARRAY);
    //skybox_vertices->Initialize();
  }

  void Spark::InitializeSkyboxTriangles()
  {
    static const std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_idx.data(), uint32_t(quad_idx.size() * sizeof(glm::u32vec3)) },
    };

    root.GetCore()->GetDevice()->CreateResource("spark_skybox_triangles",
      Resource::BufferDesc
      {
        uint32_t(sizeof(glm::u32vec3)),
        uint32_t(quad_idx.size()),
        Usage(USAGE_INDEX_ARRAY)
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );

    //skybox_triangles->SetType(Resource::TYPE_BUFFER);
    //skybox_triangles->SetStride(uint32_t(sizeof(glm::u32vec3)));
    //skybox_triangles->SetCount(uint32_t(quad_idx.size()));
    //skybox_triangles->SetInteropCount(1);
    //skybox_triangles->SetInteropItem(0, std::pair(quad_idx.data(), uint32_t(quad_idx.size() * sizeof(glm::u32vec3))));
    //skybox_triangles->SetUsage(Usage::USAGE_INDEX_ARRAY);
    //skybox_triangles->Initialize();
  }

  void Spark::InitializeSkyboxTexture()
  {
    const auto layers = prop_skybox->GetArraySize();
    const auto format = FORMAT_R32G32B32A32_FLOAT;
    const auto bpp = 16u;

    auto mipmaps = 1u;
    auto size_x = 2u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_skybox->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; size_x <<= 1; size_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_skybox->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_skybox->GetArrayItem(i)->GetRawBytes(0));
    }

    root.GetCore()->GetDevice()->CreateResource("spark_skybox_textures",
      Resource::Tex2DDesc
      {
        format,
        size_x,
        size_y,
        layers,
        mipmaps,
        Usage(USAGE_SHADER_RESOURCE)
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );

    //skybox_texture->SetType(Resource::TYPE_IMAGE2D);
    //skybox_texture->SetExtentX(size_x);
    //skybox_texture->SetExtentY(size_y);
    //skybox_texture->SetExtentZ(1);
    //skybox_texture->SetLayers(layers);
    //skybox_texture->SetMipmaps(mipmaps);
    //skybox_texture->SetFormat(format);
    //skybox_texture->SetInteropCount(layers);
    //for (uint32_t i = 0; i < layers; ++i)
    //{ 
    //  skybox_texture->SetInteropItem(i, prop_skybox->GetArrayItem(i)->GetRawBytes(0));
    //}
    //skybox_texture->SetUsage(USAGE_SHADER_RESOURCE);
    //skybox_texture->Initialize();
  }

  void Spark::InitializeGraphicArguments()
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

    root.GetCore()->GetDevice()->CreateResource("spark_graphic_arguments",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Pass::Argument)),
        uint32_t(count),
        Usage(USAGE_COMMAND_INDIRECT)
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );

    //graphic_arguments->SetType(Resource::TYPE_BUFFER);
    //graphic_arguments->SetStride(uint32_t(sizeof(Pass::Argument)));
    //graphic_arguments->SetCount(uint32_t(count));
    //graphic_arguments->SetHint(Resource::HINT_DYNAMIC_BUFFER);
    //graphic_arguments->SetUsage(USAGE_COMMAND_INDIRECT);
    //graphic_arguments->Initialize();
  }

  void Spark::InitializeComputeArguments()
  {
    root.GetCore()->GetDevice()->CreateResource("spark_compute_arguments",
      Resource::BufferDesc
      {
        uint32_t(sizeof(Pass::Argument)),
        1u,
        Usage(USAGE_COMMAND_INDIRECT)
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
    );

    //compute_arguments->SetType(Resource::TYPE_BUFFER);
    //compute_arguments->SetStride(uint32_t(sizeof(Pass::Argument)));
    //compute_arguments->SetCount(1);
    //compute_arguments->SetHint(Resource::HINT_DYNAMIC_BUFFER);
    //compute_arguments->SetUsage(USAGE_COMMAND_INDIRECT);
    //compute_arguments->Initialize();
  }

  void Spark::InitializeShadowmapLayout()
  {
    shadowmap_camera_data = shadow_data->CreateView("spark_shadowmap_shadow_data",
      USAGE_CONSTANT_DATA,
      { 0, sizeof(Frustum) }
    );

    //shadowmap_camera_data = shadow_data->CreateView("spark_shadowmap_shadow_data");
    //{
    //  shadowmap_camera_data->SetUsage(USAGE_CONSTANT_DATA);
    //  shadowmap_camera_data->SetByteOffset(0);
    //  shadowmap_camera_data->SetByteCount(sizeof(Frustum));
    //}
    //shadowmap_camera_data->Initialize();

    const std::shared_ptr<View> sb_views[] = {
      shadowmap_camera_data,
    };

    shadowmap_layout = root.GetCore()->GetDevice()->CreateLayout("spark_shadowmap_layout",
      {},
      { sb_views, uint32_t(std::size(sb_views)) },
      {},
      {},
      {},
      {},
      {},
      {}
    );

    //shadowmap_layout->UpdateSBViews({ sb_views, uint32_t(std::size(sb_views)) });
    //shadowmap_layout->Initialize();
  }

  void Spark::InitializeShadowmapConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadow.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    const Config::IAState ia_state =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0,  0, 16, FORMAT_R32G32B32_FLOAT, false }
      }
    };

    const Config::RCState rc_state =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(shadow_resolution), float(shadow_resolution), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      Config::COMPARISON_LESS //depth_comparison
    };

    const Config::OMState om_state =
    {
      false,
      {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    shadowmap_config = root.GetCore()->GetDevice()->CreateConfig("spark_shadowmap_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS),
      {},
      ia_state,
      rc_state,
      ds_state,
      om_state
    );

    //shadowmap_config->SetSource(shader_ss.str());
    //shadowmap_config->SetCompilation(Config::Compilation(Config::COMPILATION_VS));
    //shadowmap_config->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
    //shadowmap_config->SetIndexer(Config::INDEXER_32_BIT);
    //
    //const Config::Attribute attributes[] = {
    //  { 0,  0, 16, FORMAT_R32G32B32_FLOAT, false },
    //};
    //shadowmap_config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });
    //
    //shadowmap_config->SetFillMode(Config::FILL_SOLID);
    //shadowmap_config->SetCullMode(Config::CULL_FRONT);
    //shadowmap_config->SetClipEnabled(false);
    //
    //const Config::Blend blends[] = {
    //  { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
    //};
    //shadowmap_config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
    //shadowmap_config->SetATCEnabled(false);
    //
    //shadowmap_config->SetDepthEnabled(true);
    //shadowmap_config->SetDepthWrite(true);
    //shadowmap_config->SetDepthComparison(Config::COMPARISON_LESS);
    //
    //const Config::Viewport viewports[] = {
    //  { 0.0f, 0.0f, float(shadow_resolution), float(shadow_resolution), 0.0f, 1.0f },
    //};
    //shadowmap_config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
    //shadowmap_config->Initialize();
  }

  void Spark::InitializeShadowmapPass(uint32_t index)
  {
    const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    shadowmap_graphic_arguments.resize(instance_count);
    for (uint32_t j = 0; j < uint32_t(shadowmap_graphic_arguments.size()); ++j)
    {
      shadowmap_graphic_arguments[j] = graphic_arguments->CreateView("spark_shadowmap_graphic_argument_" + std::to_string(index) + "_" + std::to_string(j),
        Usage(USAGE_COMMAND_INDIRECT),
        { j * uint32_t(sizeof(Pass::Argument)), uint32_t(sizeof(Pass::Argument)) }
      );
    }
    std::vector<Pass::Command> commands(instance_count);
    for (uint32_t j = 0; j < uint32_t(commands.size()); ++j)
    {
      commands[j].view = shadowmap_graphic_arguments[j];
      commands[j].offsets = { index * uint32_t(sizeof(Frustum)) };
    }

    shadowmap_scene_vertices0 = scene_vertices0->CreateView("spark_shadowmap_scene_vertices0",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> va_views[] = {
      shadowmap_scene_vertices0,
    };

    shadowmap_scene_triangles = scene_triangles->CreateView("spark_shadowmap_scene_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> ia_views[] = {
      shadowmap_scene_triangles,
    };

    const Pass::Subpass subpasses[] = {
      { shadowmap_config, shadowmap_layout, std::move(commands), {va_views, va_views + std::size(va_views)}, {ia_views, ia_views + std::size(ia_views)} }
    };

    shadowmap_shadow_map[index] = shadow_map->CreateView("spark_shadowmap_shadow_map_" + std::to_string(index),
      Usage(USAGE_DEPTH_STENCIL),
      { index, 1 }
    );

    //const std::shared_ptr<View> ds_views[] = {
    //  shadowmap_shadow_map[index],
    //};
    const Pass::DSAttachment ds_attachments[] = {
      { shadowmap_shadow_map[index], { 1.0f, std::nullopt }},
    };

    shadowmap_passes[index] = root.GetCore()->GetDevice()->CreatePass("spark_shadowmap_pass_" + std::to_string(index),
      Pass::TYPE_GRAPHIC,
      { subpasses, uint32_t(std::size(subpasses)) },
      {},
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );

    //  shadowmap_passes[index]->SetType(Pass::TYPE_GRAPHIC);
    //  shadowmap_passes[index]->SetEnabled(true);
    //
    //  shadowmap_shadow_map[index] = shadow_map->CreateView("spark_shadowmap_shadow_map_" + std::to_string(index));
    //  shadowmap_shadow_map[index]->SetUsage(USAGE_DEPTH_STENCIL);
    //  shadowmap_shadow_map[index]->SetLayerOffset(index);
    //  shadowmap_shadow_map[index]->SetLayerCount(1);
    //
    //  const std::shared_ptr<View> ds_views[] = {
    //    shadowmap_shadow_map[index],
    //  };
    //  shadowmap_passes[index]->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });
    //
    //  const Pass::DSValue ds_values[] = {
    //    { 1.0f, std::nullopt },
    //  };
    //  shadowmap_passes[index]->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });
    //
    //  shadowmap_passes[index]->SetSubpassCount(1);
    //
    //  const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    //  shadowmap_graphic_arguments.resize(instance_count);
    //  for (uint32_t j = 0; j < uint32_t(shadowmap_graphic_arguments.size()); ++j)
    //  {
    //    shadowmap_graphic_arguments[j] = graphic_arguments->CreateView("spark_shadowmap_graphic_argument_" + std::to_string(index) + "_" + std::to_string(j));
    //    shadowmap_graphic_arguments[j]->SetUsage(USAGE_COMMAND_INDIRECT);
    //    shadowmap_graphic_arguments[j]->SetByteOffset(j * uint32_t(sizeof(Pass::Argument)));
    //    shadowmap_graphic_arguments[j]->SetByteCount(uint32_t(sizeof(Pass::Argument)));
    //    shadowmap_graphic_arguments[j]->Initialize();
    //  }
    //  std::vector<Pass::Command> commands(instance_count);
    //  for (uint32_t j = 0; j < uint32_t(commands.size()); ++j)
    //  {
    //    commands[j].view = shadowmap_graphic_arguments[j];
    //    commands[j].offsets = { index * uint32_t(sizeof(Frustum)) };
    //  }
    //  shadowmap_passes[index]->UpdateSubpassCommands(0, { commands.data(), uint32_t(commands.size()) });
    //
    //  shadowmap_scene_vertices0 = scene_vertices0->CreateView("spark_shadowmap_scene_vertices0");
    //  {
    //    shadowmap_scene_vertices0->SetUsage(USAGE_VERTEX_ARRAY);
    //    shadowmap_scene_vertices0->SetByteOffset(0);
    //    shadowmap_scene_vertices0->SetByteCount(uint32_t(-1));
    //    shadowmap_scene_vertices0->Initialize();
    //  }
    //
    //  const std::shared_ptr<View> va_views[] = {
    //    shadowmap_scene_vertices0,
    //  };
    //  shadowmap_passes[index]->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });
    //
    //  shadowmap_scene_triangles = scene_triangles->CreateView("spark_shadowmap_scene_triangles");
    //  {
    //    shadowmap_scene_triangles->SetUsage(USAGE_INDEX_ARRAY);
    //    shadowmap_scene_triangles->SetByteOffset(0);
    //    shadowmap_scene_triangles->SetByteCount(uint32_t(-1));
    //    shadowmap_scene_triangles->Initialize();
    //  }
    //
    //  const std::shared_ptr<View> ia_views[] = {
    //    shadowmap_scene_triangles,
    //  };
    //  shadowmap_passes[index]->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });
    //
    //  shadowmap_passes[index]->SetSubpassLayout(0, shadowmap_layout);
    //  shadowmap_passes[index]->SetSubpassConfig(0, shadowmap_config);
    //  shadowmap_passes[index]->Initialize();
  }

  void Spark::InitializeUnshadowedLayout()
  {
    unshadowed_screen_data = screen_data->CreateView("spark_unshadowed_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );

    unshadowed_camera_data = camera_data->CreateView("spark_unshadowed_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );

    unshadowed_shadow_data = shadow_data->CreateView("spark_unshadowed_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Frustum)) }
    );

    const std::shared_ptr<View> ub_views[] = {
      unshadowed_screen_data,
      unshadowed_camera_data,
      unshadowed_shadow_data,
    };


    unshadowed_scene_instances = scene_instances->CreateView("spark_unshadowed_scene_instances",
      Usage(USAGE_CONSTANT_DATA),
      { 0, uint32_t(sizeof(Instance)) }
    );

    const std::shared_ptr<View> ue_views[] = {
      unshadowed_scene_instances
    };


    unshadowed_scene_textures0 = scene_textures0->CreateView("spark_unshadowed_scene_textures0",
      Usage(USAGE_SHADER_RESOURCE)
    );

    unshadowed_scene_textures1 = scene_textures1->CreateView("spark_unshadowed_scene_textures1",
      Usage(USAGE_SHADER_RESOURCE)
    );

    unshadowed_scene_textures2 = scene_textures2->CreateView("spark_unshadowed_scene_textures2",
      Usage(USAGE_SHADER_RESOURCE)
    );

    unshadowed_scene_textures3 = scene_textures3->CreateView("spark_unshadowed_scene_textures3",
      Usage(USAGE_SHADER_RESOURCE)
    );

    unshadowed_light_maps = light_maps->CreateView("spark_unshadowed_light_maps",
      Usage(USAGE_SHADER_RESOURCE)
    );

    const std::shared_ptr<View> ri_views[] = {
      unshadowed_scene_textures0,
      unshadowed_scene_textures1,
      unshadowed_scene_textures2,
      unshadowed_scene_textures3,
      unshadowed_light_maps,
    };

    const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };

    unshadowed_layout = root.GetCore()->GetDevice()->CreateLayout("spark_unshadowed_layout",
      { ub_views, uint32_t(std::size(ub_views)) },
      { ue_views, uint32_t(std::size(ue_views)) },
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {},
      { samplers, uint32_t(std::size(samplers)) },
      {}
    );
  }

  void Spark::InitializeUnshadowedConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_simple.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::pair<const std::string&, const  std::string&> defines[] =
    {
      {"TEST", "1"},
    };

    //unshadowed_config->SetSource(shader_ss.str());
    //unshadowed_config->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));
    //unshadowed_config->SetDefineItem("TEST", "1");

    const Config::IAState ia_state =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0,  0, 16, FORMAT_R32G32B32_FLOAT, false },
        { 0, 12, 16, FORMAT_R8G8B8A8_UNORM, false },
        { 1,  0, 16, FORMAT_R32G32B32_FLOAT, false },
        { 1, 12, 16, FORMAT_R32_UINT, false },
        { 2,  0, 16, FORMAT_R32G32B32_FLOAT, false },
        { 2, 12, 16, FORMAT_R32_FLOAT, false },
        { 3,  0, 16, FORMAT_R32G32_FLOAT, false },
        { 3,  8, 16, FORMAT_R32G32_FLOAT, false },
      }
    };
    

    //const Config::Attribute attributes[] = {
    //  { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false },
    //  { 0, 12, 64, FORMAT_R8G8B8A8_UINT, false },
    //  { 0, 16, 64, FORMAT_R32G32B32_FLOAT, false },
    //  { 0, 28, 64, FORMAT_R32_UINT, false },
    //  { 0, 32, 64, FORMAT_R32G32B32_FLOAT, false },
    //  { 0, 44, 64, FORMAT_R32_FLOAT, false },
    //  { 0, 48, 64, FORMAT_R32G32_FLOAT, false },
    //  { 0, 56, 64, FORMAT_R32G32_FLOAT, false },
    //};
    //const Config::Attribute attributes[] = {
    //  { 0,  0, 16, FORMAT_R32G32B32_FLOAT, false },
    //  { 0, 12, 16, FORMAT_R8G8B8A8_UNORM, false },
    //  { 1,  0, 16, FORMAT_R32G32B32_FLOAT, false },
    //  { 1, 12, 16, FORMAT_R32_UINT, false },
    //  { 2,  0, 16, FORMAT_R32G32B32_FLOAT, false },
    //  { 2, 12, 16, FORMAT_R32_FLOAT, false },
    //  { 3,  0, 16, FORMAT_R32G32_FLOAT, false },
    //  { 3,  8, 16, FORMAT_R32G32_FLOAT, false },
    //};
    //unshadowed_config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

    const Config::RCState rc_state =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    //unshadowed_config->SetFillMode(Config::FILL_SOLID);
    //unshadowed_config->SetCullMode(Config::CULL_BACK);
    //unshadowed_config->SetClipEnabled(false);

    const Config::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      Config::COMPARISON_LESS //depth_comparison
    };

    const Config::OMState om_state =
    {
      false,
      {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    unshadowed_config = root.GetCore()->GetDevice()->CreateConfig("spark_unshadowed_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines, uint32_t(std::size(defines)) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );

    //const Config::Blend blends[] = {
    //  { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
    //};
    //unshadowed_config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
    //unshadowed_config->SetATCEnabled(false);

    //unshadowed_config->SetDepthEnabled(true);
    //unshadowed_config->SetDepthWrite(true);
    //unshadowed_config->SetDepthComparison(Config::COMPARISON_LESS);

    //const Config::Viewport viewports[] = {
    //  { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f },
    //};
    //unshadowed_config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
    //unshadowed_config->Initialize();
  }

  void Spark::InitializeUnshadowedPass()
  {
    Pass::Subpass subpasses[ShadingSubpass::SUBPASS_MAX_COUNT] = {};
    {
      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      unshadowed_graphic_arguments.resize(instance_count);
      for (uint32_t j = 0; j < uint32_t(unshadowed_graphic_arguments.size()); ++j)
      {
        unshadowed_graphic_arguments[j] = graphic_arguments->CreateView("spark_unshadowed_graphic_argument_" + std::to_string(j),
          Usage(USAGE_COMMAND_INDIRECT),
          { j * uint32_t(sizeof(Pass::Argument)), uint32_t(sizeof(Pass::Argument)) }
        );
      }
      std::vector<Pass::Command> unshadowed_commands(instance_count);
      for (uint32_t j = 0; j < uint32_t(unshadowed_commands.size()); ++j)
      {
        unshadowed_commands[j].view = unshadowed_graphic_arguments[j];
        unshadowed_commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
      }

      //unshadowed_pass->UpdateSubpassCommands(ShadingSubpass::SUBPASS_OPAQUE, { commands.data(), uint32_t(commands.size()) });

      unshadowed_scene_vertices0 = scene_vertices0->CreateView("spark_unshadowed_scene_vertices0",
        Usage(USAGE_VERTEX_ARRAY)
      );
      unshadowed_scene_vertices1 = scene_vertices1->CreateView("spark_unshadowed_scene_vertices1",
        Usage(USAGE_VERTEX_ARRAY)
      );
      unshadowed_scene_vertices2 = scene_vertices2->CreateView("spark_unshadowed_scene_vertices2",
        Usage(USAGE_VERTEX_ARRAY)
      );
      unshadowed_scene_vertices3 = scene_vertices3->CreateView("spark_unshadowed_scene_vertices3",
        Usage(USAGE_VERTEX_ARRAY)
      );
      std::vector<std::shared_ptr<View>> unshadowed_va_views = {
        unshadowed_scene_vertices0,
        unshadowed_scene_vertices1,
        unshadowed_scene_vertices2,
        unshadowed_scene_vertices3,
      };
      //unshadowed_pass->UpdateSubpassVAViews(ShadingSubpass::SUBPASS_OPAQUE, { va_views, uint32_t(std::size(va_views)) });

      unshadowed_scene_triangles = scene_triangles->CreateView("spark_unshadowed_scene_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      std::vector<std::shared_ptr<View>> unshadowed_ia_views = {
        unshadowed_scene_triangles,
      };
      //unshadowed_pass->UpdateSubpassIAViews(ShadingSubpass::SUBPASS_OPAQUE, { ia_views, uint32_t(std::size(ia_views)) });

      //unshadowed_pass->SetSubpassLayout(ShadingSubpass::SUBPASS_OPAQUE, unshadowed_layout);
      //unshadowed_pass->SetSubpassConfig(ShadingSubpass::SUBPASS_OPAQUE, unshadowed_config);

      subpasses[ShadingSubpass::SUBPASS_OPAQUE] =
      {
        unshadowed_config, unshadowed_layout,
        std::move(unshadowed_commands),
        std::move(unshadowed_va_views),
        std::move(unshadowed_ia_views)
      };
    }
    {
      const Pass::Command skybox_commands[] = {
        {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
      };
      //unshadowed_pass->UpdateSubpassCommands(ShadingSubpass::SUBPASS_SKYBOX, { commands, uint32_t(std::size(commands)) });

      skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      const std::shared_ptr<View> skybox_va_views[] = {
        skybox_skybox_vertices,
      };
      //unshadowed_pass->UpdateSubpassVAViews(ShadingSubpass::SUBPASS_SKYBOX, { va_views, uint32_t(std::size(va_views)) });

      skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      const std::shared_ptr<View> skybox_ia_views[] = {
        skybox_skybox_triangles,
      };
      //unshadowed_pass->UpdateSubpassIAViews(ShadingSubpass::SUBPASS_SKYBOX, { ia_views, uint32_t(std::size(ia_views)) });

      //unshadowed_pass->SetSubpassLayout(ShadingSubpass::SUBPASS_SKYBOX, skybox_layout);
      //unshadowed_pass->SetSubpassConfig(ShadingSubpass::SUBPASS_SKYBOX, skybox_config);

      subpasses[ShadingSubpass::SUBPASS_SKYBOX] =
      {
        skybox_config, skybox_layout,
        { skybox_commands, skybox_commands + std::size(skybox_commands) },
        { skybox_va_views, skybox_va_views + std::size(skybox_va_views) },
        { skybox_ia_views, skybox_ia_views + std::size(skybox_ia_views) }
      };
    }

    unshadowed_color_target = color_target->CreateView("spark_unshadowed_color_target",
      Usage(USAGE_RENDER_TARGET)
    );

    //const std::shared_ptr<View> rt_views[] = {
    //  unshadowed_color_target,
    //};
    //unshadowed_pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });
    const Pass::RTAttachment rt_attachments[] = {
      unshadowed_color_target, std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    };

    unshadowed_depth_target = depth_target->CreateView("spark_unshadowed_depth_target",
      Usage(USAGE_DEPTH_STENCIL)
    );

    //const std::shared_ptr<View> ds_views[] = {
    //  unshadowed_depth_target,
    //};
    //unshadowed_pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

    //const Pass::RTValue rt_values[] = {
    //  std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    //};
    //unshadowed_pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

    //const Pass::DSValue ds_values[] = {
    //  { 1.0f, std::nullopt },
    //};
    //unshadowed_pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });
    const Pass::DSAttachment ds_attachments[] = {
      unshadowed_depth_target, { 1.0f, std::nullopt },
    };

    unshadowed_pass = root.GetCore()->GetDevice()->CreatePass("spark_unshadowed_pass",
      Pass::TYPE_GRAPHIC,
      { subpasses, uint32_t(std::size(subpasses)) },
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Spark::InitializeShadowedLayout()
  {
    shadowed_screen_data = screen_data->CreateView("spark_shadowed_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    shadowed_camera_data = camera_data->CreateView("spark_shadowed_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    shadowed_shadow_data = shadow_data->CreateView("spark_shadowed_shadow_data",
      Usage(USAGE_CONSTANT_DATA),
      { 0, sizeof(Frustum) }
    );
    const std::shared_ptr<View> ub_views[] = {
      shadowed_screen_data,
      shadowed_camera_data,
      shadowed_shadow_data,
    };
    //shadowed_layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

    shadowed_scene_instances = scene_instances->CreateView("spark_shadowed_scene_instances",
      Usage(USAGE_CONSTANT_DATA),
      { 0, sizeof(Instance) }
    );
    const std::shared_ptr<View> ue_views[] = {
      shadowed_scene_instances
    };
    //shadowed_layout->UpdateSBViews({ ue_views, uint32_t(std::size(ue_views)) });

    shadowed_scene_textures0 = scene_textures0->CreateView("spark_shadowed_scene_textures0",
      Usage(USAGE_SHADER_RESOURCE)
    );
    shadowed_scene_textures1 = scene_textures1->CreateView("spark_shadowed_scene_textures1",
      Usage(USAGE_SHADER_RESOURCE)
    );
    shadowed_scene_textures2 = scene_textures2->CreateView("spark_shadowed_scene_textures2",
      Usage(USAGE_SHADER_RESOURCE)
    );
    shadowed_scene_textures3 = scene_textures3->CreateView("spark_shadowed_scene_textures3",
      Usage(USAGE_SHADER_RESOURCE)
    );
    shadowed_shadow_map = shadow_map->CreateView("spark_shadowed_shadow_map",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      shadowed_scene_textures0,
      shadowed_scene_textures1,
      shadowed_scene_textures2,
      shadowed_scene_textures3,
      shadowed_shadow_map,
    };
    //shadowed_layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });
    //shadowed_layout->Initialize();

    const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      { Layout::Sampler::FILTERING_NEAREST, 1, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_ALWAYS, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
    };

    shadowed_layout = root.GetCore()->GetDevice()->CreateLayout("spark_shadowed_layout",
      { ub_views, uint32_t(std::size(ub_views)) },
      { ue_views, uint32_t(std::size(ue_views)) },
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {},
      { samplers, uint32_t(std::size(samplers)) },
      {}
    );
  }


  void Spark::InitializeShadowedConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_advanced.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::pair<const std::string&, const  std::string&> defines[] =
    {
      {"TEST", "1"},
    };

    const Config::IAState ia_state =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0,  0, 16, FORMAT_R32G32B32_FLOAT, false },
        { 0, 12, 16, FORMAT_R8G8B8A8_UNORM, false },
        { 1,  0, 16, FORMAT_R32G32B32_FLOAT, false },
        { 1, 12, 16, FORMAT_R32_UINT, false },
        { 2,  0, 16, FORMAT_R32G32B32_FLOAT, false },
        { 2, 12, 16, FORMAT_R32_FLOAT, false },
        { 3,  0, 16, FORMAT_R32G32_FLOAT, false },
        { 3,  8, 16, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Config::RCState rc_state =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_state =
    {
      true, //depth_enabled
      true, //depth_write
      Config::COMPARISON_LESS //depth_comparison
    };

    const Config::OMState om_state =
    {
      false,
      {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    shadowed_config = root.GetCore()->GetDevice()->CreateConfig("spark_shadowed_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines, uint32_t(std::size(defines)) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );
  }


  void Spark::InitializeShadowedPass()
  {
    Pass::Subpass subpasses[ShadingSubpass::SUBPASS_MAX_COUNT] = {};
    {
      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      shadowed_graphic_arguments.resize(instance_count);
      for (uint32_t j = 0; j < uint32_t(shadowed_graphic_arguments.size()); ++j)
      {
        shadowed_graphic_arguments[j] = graphic_arguments->CreateView("spark_shadowed_graphic_argument_" + std::to_string(j),
          Usage(USAGE_COMMAND_INDIRECT),
          { j * uint32_t(sizeof(Pass::Argument)), uint32_t(sizeof(Pass::Argument)) }
        );
      }
      std::vector<Pass::Command> shadowed_commands(instance_count);
      for (uint32_t j = 0; j < uint32_t(shadowed_commands.size()); ++j)
      {
        shadowed_commands[j].view = shadowed_graphic_arguments[j];
        shadowed_commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
      }

      shadowed_scene_vertices0 = scene_vertices0->CreateView("spark_shadowed_scene_vertices0",
        Usage(USAGE_VERTEX_ARRAY)
      );
      shadowed_scene_vertices1 = scene_vertices1->CreateView("spark_shadowed_scene_vertices1",
        Usage(USAGE_VERTEX_ARRAY)
      );
      shadowed_scene_vertices2 = scene_vertices2->CreateView("spark_shadowed_scene_vertices2",
        Usage(USAGE_VERTEX_ARRAY)
      );
      shadowed_scene_vertices3 = scene_vertices3->CreateView("spark_shadowed_scene_vertices3",
        Usage(USAGE_VERTEX_ARRAY)
      );
      std::vector<std::shared_ptr<View>> shadowed_va_views = {
        shadowed_scene_vertices0,
        shadowed_scene_vertices1,
        shadowed_scene_vertices2,
        shadowed_scene_vertices3,
      };

      shadowed_scene_triangles = scene_triangles->CreateView("spark_shadowed_scene_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      std::vector<std::shared_ptr<View>> shadowed_ia_views = {
        shadowed_scene_triangles,
      };

      subpasses[ShadingSubpass::SUBPASS_OPAQUE] =
      {
        shadowed_config, shadowed_layout,
        std::move(shadowed_commands),
        std::move(shadowed_va_views),
        std::move(shadowed_ia_views)
      };
    }
    {
      const Pass::Command skybox_commands[] = {
        {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
      };

      skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices",
        Usage(USAGE_VERTEX_ARRAY)
      );
      const std::shared_ptr<View> skybox_va_views[] = {
        skybox_skybox_vertices,
      };

      skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles",
        Usage(USAGE_INDEX_ARRAY)
      );
      const std::shared_ptr<View> skybox_ia_views[] = {
        skybox_skybox_triangles,
      };

      subpasses[ShadingSubpass::SUBPASS_SKYBOX] =
      {
        skybox_config, skybox_layout,
        { skybox_commands, skybox_commands + std::size(skybox_commands) },
        { skybox_va_views, skybox_va_views + std::size(skybox_va_views) },
        { skybox_ia_views, skybox_ia_views + std::size(skybox_ia_views) }
      };
    }

    shadowed_color_target = color_target->CreateView("spark_shadowed_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      shadowed_color_target, std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    };

    shadowed_depth_target = depth_target->CreateView("spark_shadowed_depth_target",
      Usage(USAGE_DEPTH_STENCIL)
    );
    const Pass::DSAttachment ds_attachments[] = {
      shadowed_depth_target, { 1.0f, std::nullopt },
    };

    shadowed_pass = root.GetCore()->GetDevice()->CreatePass("spark_shadowed_pass",
      Pass::TYPE_GRAPHIC,
      { subpasses, uint32_t(std::size(subpasses)) },
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Spark::InitializeSkyboxLayout()
  {
    skybox_screen_data = screen_data->CreateView("spark_skybox_screen_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    skybox_camera_data = camera_data->CreateView("spark_skybox_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    const std::shared_ptr<View> ub_views[] = {
      skybox_screen_data,
      skybox_camera_data,
    };
    //skybox_layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

    skybox_skybox_texture = skybox_texture->CreateView("spark_skybox_skybox_texture",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      skybox_skybox_texture,
    };
    //skybox_layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };
    //skybox_layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });

    shadowed_layout = root.GetCore()->GetDevice()->CreateLayout("spark_shadowed_layout",
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      {},
      {},
      {},
      { samplers, uint32_t(std::size(samplers)) },
      {}
    );
  }

  void Spark::InitializeSkyboxConfig()
  { 
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_environment.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    std::pair<const std::string&, const  std::string&> defines[] =
    {
      {"TEST", "1"},
    };

    const Config::IAState ia_state =
    {
      Config::TOPOLOGY_TRIANGLELIST,
      Config::INDEXER_32_BIT,
      {
        { 0, 0, 16, FORMAT_R32G32_FLOAT, false },
        { 0, 8, 16, FORMAT_R32G32_FLOAT, false },
      }
    };

    const Config::RCState rc_state =
    {
      Config::FILL_SOLID,
      Config::CULL_FRONT,
      {
        { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f }
      },
    };

    const Config::DSState ds_state =
    {
      true, //depth_enabled
      false, //depth_write
      Config::COMPARISON_EQUAL //depth_comparison
    };

    const Config::OMState om_state =
    {
      false,
      {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF }
      }
    };

    skybox_config = root.GetCore()->GetDevice()->CreateConfig("spark_skybox_config",
      shader_ss.str(),
      Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS),
      { defines, uint32_t(std::size(defines)) },
      ia_state,
      rc_state,
      ds_state,
      om_state
    );

    //skybox_config->SetSource(shader_ss.str());
    //skybox_config->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));
    //skybox_config->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
    //skybox_config->SetIndexer(Config::INDEXER_32_BIT);

    //const Config::Attribute attributes[] = {
    //  { 0, 0, 16, FORMAT_R32G32_FLOAT, false },
    //  { 0, 8, 16, FORMAT_R32G32_FLOAT, false },
    //};
    //skybox_config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

    //skybox_config->SetFillMode(Config::FILL_SOLID);
    //skybox_config->SetCullMode(Config::CULL_NONE);
    //skybox_config->SetClipEnabled(false);

    //const Config::Blend blends[] = {
    //  { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
    //};
    //skybox_config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
    //skybox_config->SetATCEnabled(false);

    //skybox_config->SetDepthEnabled(true);
    //skybox_config->SetDepthWrite(false);
    //skybox_config->SetDepthComparison(Config::COMPARISON_EQUAL);

    //const Config::Viewport viewports[] = {
    //  { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f },
    //};
    //skybox_config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
    //skybox_config->Initialize();
  }

  void Spark::InitializeSkyboxPass()
  {
    const Pass::Command skybox_commands[] = {
      {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
    };

    skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices",
      Usage(USAGE_VERTEX_ARRAY)
    );
    const std::shared_ptr<View> skybox_va_views[] = {
      skybox_skybox_vertices,
    };

    skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles",
      Usage(USAGE_INDEX_ARRAY)
    );
    const std::shared_ptr<View> skybox_ia_views[] = {
      skybox_skybox_triangles,
    };

    Pass::Subpass subpasses[] =
    {
      skybox_config, skybox_layout,
      { skybox_commands, skybox_commands + std::size(skybox_commands) },
      { skybox_va_views, skybox_va_views + std::size(skybox_va_views) },
      { skybox_ia_views, skybox_ia_views + std::size(skybox_ia_views) }
    };

    skybox_color_target = color_target->CreateView("spark_skybox_color_target",
      Usage(USAGE_RENDER_TARGET)
    );
    const Pass::RTAttachment rt_attachments[] = {
      skybox_color_target, std::nullopt,
    };

    skybox_depth_target = depth_target->CreateView("spark_skybox_depth_target",
      Usage(USAGE_DEPTH_STENCIL)
    );
    const Pass::DSAttachment ds_attachments[] = {
      skybox_depth_target, { std::nullopt, std::nullopt },
    };

    skybox_pass = root.GetCore()->GetDevice()->CreatePass("spark_skybox_pass",
      Pass::TYPE_GRAPHIC,
      { subpasses, uint32_t(std::size(subpasses)) },
      { rt_attachments, uint32_t(std::size(rt_attachments)) },
      { ds_attachments, uint32_t(std::size(ds_attachments)) }
    );
  }

  void Spark::InitializePresentLayout()
  {
    present_camera_data = camera_data->CreateView("spark_present_camera_data",
      Usage(USAGE_CONSTANT_DATA)
    );
    const std::shared_ptr<View> ub_views[] = {
      present_camera_data,
    };

    present_color_target = color_target->CreateView("spark_present_color_target",
      Usage(USAGE_SHADER_RESOURCE)
    );
    const std::shared_ptr<View> ri_views[] = {
      present_color_target,
    };

    const std::shared_ptr<View> wi_views[] = {
      backbuffer_rtv,
    };

    present_layout = root.GetCore()->GetDevice()->CreateLayout("spark_present_layout",
      { ub_views, uint32_t(std::size(ub_views)) },
      {},
      { ri_views, uint32_t(std::size(ri_views)) },
      { wi_views, uint32_t(std::size(wi_views)) },
      {},
      {},
      {},
      {}
    );
  }

  void Spark::InitializePresentConfig()
  {
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_present.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    present_config = root.GetCore()->GetDevice()->CreateConfig("spark_present_config",
      shader_ss.str(),
      Config::COMPILATION_CS,
      {},
      {},
      {},
      {},
      {}
    );
  }

  void Spark::InitializePresentPass()
  {
    present_compute_arguments = compute_arguments->CreateView("spark_present_compute_arguments",
      Usage(USAGE_COMMAND_INDIRECT)
    );
    Pass::Command present_commands[] = {
      { present_compute_arguments },
    };
    //present_pass->UpdateSubpassCommands(0, { commands, uint32_t(std::size(commands)) });

    //present_pass->SetSubpassLayout(0, present_layout);
    //present_pass->SetSubpassConfig(0, present_config);

    Pass::Subpass subpasses[] =
    {
      present_config, present_layout,
      { present_commands, present_commands + std::size(present_commands) },
      {},
      {}
    };

    present_pass = root.GetCore()->GetDevice()->CreatePass("spark_present_pass",
      Pass::TYPE_COMPUTE,
      { subpasses, uint32_t(std::size(subpasses)) },
      {},
      {}
    );

    //present_pass->SetType(Pass::TYPE_COMPUTE);
    //present_pass->SetEnabled(true);

    //present_pass->SetSubpassCount(1);


    


    //present_pass->Initialize();
  }


  void Spark::Initialize()
  {
    const auto find_view_fn = [this](const std::shared_ptr<View>& view)
    {
      if (view->GetName().compare("backbuffer_ua_view") == 0)
      {
        this->backbuffer_uav = view;
      }

      if (view->GetName().compare("backbuffer_rt_view") == 0)
      {
        this->backbuffer_rtv = view;
      }
    };
    root.GetCore()->VisitView(find_view_fn);

    
    const auto tree = root.GetData()->GetStorage()->GetTree();
    
    prop_scene = tree->GetObjectItem("scene");
    {
      prop_instances = prop_scene->GetObjectItem("instances");
      prop_triangles = prop_scene->GetObjectItem("triangles");

      prop_vertices0 = prop_scene->GetObjectItem("vertices0");
      prop_vertices1 = prop_scene->GetObjectItem("vertices1");
      prop_vertices2 = prop_scene->GetObjectItem("vertices2");
      prop_vertices3 = prop_scene->GetObjectItem("vertices3");

      prop_textures0 = prop_scene->GetObjectItem("textures0");
      prop_textures1 = prop_scene->GetObjectItem("textures1");
      prop_textures2 = prop_scene->GetObjectItem("textures2");
      prop_textures3 = prop_scene->GetObjectItem("textures3");

      prop_lightmaps = prop_scene->GetObjectItem("textures4");
    }

    prop_camera = tree->GetObjectItem("camera");
    {
      prop_eye = prop_camera->GetObjectItem("eye");
      prop_lookat = prop_camera->GetObjectItem("lookat");
      prop_up = prop_camera->GetObjectItem("up");

      prop_fov_x = prop_camera->GetObjectItem("fov_x");
      prop_fov_y = prop_camera->GetObjectItem("fov_y");

      prop_extent_x = prop_camera->GetObjectItem("extent_x");
      prop_extent_y = prop_camera->GetObjectItem("extent_y");

      prop_n_plane = prop_camera->GetObjectItem("n_plane");
      prop_f_plane = prop_camera->GetObjectItem("f_plane");

      prop_counter = prop_camera->GetObjectItem("counter");
    }

    prop_environment = tree->GetObjectItem("environment");


    InitializeColorTarget();
    InitializeDepthTarget();
    InitializeShadowMap();

    InitializeScreenData();
    InitializeCameraData();
    InitializeShadowData();
    
    InitializeSceneInstances();
    InitializeSceneTriangles();
    InitializeSceneVertices0();
    InitializeSceneVertices1();
    InitializeSceneVertices2();
    InitializeSceneVertices3();
 
    InitializeSceneTextures0();
    InitializeSceneTextures1();
    InitializeSceneTextures2();
    InitializeSceneTextures3();
   
    InitializeLightMaps();

    InitializeSkyboxVertices();
    InitializeSkyboxTriangles();
    InitializeSkyboxTexture();

    InitializeGraphicArguments();
    InitializeComputeArguments();

    InitializeShadowmapLayout();
    InitializeUnshadowedLayout();
    InitializeShadowedLayout();
    InitializeSkyboxLayout();
    InitializePresentLayout();

    InitializeSkyboxConfig();
    InitializeUnshadowedConfig();
    InitializeShadowedConfig();
    InitializeSkyboxConfig();
    InitializePresentConfig();

    InitializeShadowmapPass(0);
    InitializeShadowmapPass(1);
    InitializeShadowmapPass(2);
    InitializeShadowmapPass(3);
    InitializeShadowmapPass(4);
    InitializeShadowmapPass(5);    
    InitializeUnshadowedPass();
    InitializeShadowedPass();
    //skybox_pass->Initialize();
    InitializePresentPass();
  }


  void Spark::Use()
  {
    switch (shadows)
    {
    case NO_SHADOWS:
    {
      shadowmap_passes[0]->SetEnabled(false);
      shadowmap_passes[1]->SetEnabled(false);
      shadowmap_passes[2]->SetEnabled(false);
      shadowmap_passes[3]->SetEnabled(false);
      shadowmap_passes[4]->SetEnabled(false);
      shadowmap_passes[5]->SetEnabled(false);
      unshadowed_pass->SetEnabled(true);
      shadowed_pass->SetEnabled(false);
      //skybox_pass->SetEnabled(true);
      present_pass->SetEnabled(true);
      break;
    }
    case POINT_SHADOWS:
    {
      shadowmap_passes[0]->SetEnabled(true);
      shadowmap_passes[1]->SetEnabled(true);
      shadowmap_passes[2]->SetEnabled(true);
      shadowmap_passes[3]->SetEnabled(true);
      shadowmap_passes[4]->SetEnabled(true);
      shadowmap_passes[5]->SetEnabled(true);
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(true);
      //skybox_pass->SetEnabled(true);
      present_pass->SetEnabled(true);
      break;
    }
    default:
    {
      shadowmap_passes[0]->SetEnabled(false);
      shadowmap_passes[1]->SetEnabled(false);
      shadowmap_passes[2]->SetEnabled(false);
      shadowmap_passes[3]->SetEnabled(false);
      shadowmap_passes[4]->SetEnabled(false);
      shadowmap_passes[5]->SetEnabled(false);
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(false);
      //skybox_pass->SetEnabled(false);
      present_pass->SetEnabled(false);
      break;
    }
    }


    {
      auto graphic_arg = reinterpret_cast<Pass::Argument*>(graphic_arguments->Map());

      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      for (uint32_t i = 0; i < instance_count; ++i)
      {
        graphic_arg[i].idx_count = instance_data[i].prim_count * 3;
        graphic_arg[i].ins_count = 1;
        graphic_arg[i].idx_offset = instance_data[i].prim_offset * 3;
        graphic_arg[i].vtx_offset = instance_data[i].vert_offset * 1;
        graphic_arg[i].ins_offset = 0;
      }

      graphic_arguments->Unmap();
    }

    {
      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();

      auto compute_arg = reinterpret_cast<Pass::Argument*>(compute_arguments->Map());
      {
        compute_arg->grid_x = extent_x / 8;
        compute_arg->grid_y = extent_y / 8;
        compute_arg->grid_z = 1;
      }
      compute_arguments->Unmap();
    }

    {
      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();

      const auto counter = prop_counter->GetUint();

      Screen screen;
      screen.extent_x = extent_x;
      screen.extent_y = extent_y;
      screen.rnd_base = counter;
      screen.rnd_seed = rand();

      auto screen_mapped = screen_data->Map();
      memcpy(screen_mapped, &screen, sizeof(Screen));
      screen_data->Unmap();
    }

    {
      const auto eye = glm::f32vec3{
        prop_eye->GetArrayItem(0)->GetReal(),
        prop_eye->GetArrayItem(1)->GetReal(),
        prop_eye->GetArrayItem(2)->GetReal()
      };

      const auto lookat = glm::f32vec3{
        prop_lookat->GetArrayItem(0)->GetReal(),
        prop_lookat->GetArrayItem(1)->GetReal(),
        prop_lookat->GetArrayItem(2)->GetReal()
      };

      const auto up = glm::f32vec3{
        prop_up->GetArrayItem(0)->GetReal(),
        prop_up->GetArrayItem(1)->GetReal(),
        prop_up->GetArrayItem(2)->GetReal()
      };

      const auto fov_x = prop_fov_x->GetReal();
      const auto fov_y = prop_fov_y->GetReal();

      const auto n_plane = prop_n_plane->GetReal();
      const auto f_plane = prop_f_plane->GetReal();

      const auto extent_x = prop_extent_x->GetUint();
      const auto extent_y = prop_extent_y->GetUint();

      Frustum camera_frustum;
      camera_frustum.proj = glm::perspective(glm::radians(fov_y), float(extent_x) / float(extent_y), n_plane, f_plane);
      camera_frustum.proj_inv = glm::inverse(camera_frustum.proj);
      camera_frustum.view = glm::lookAt(eye, lookat, up);
      camera_frustum.view_inv = glm::inverse(camera_frustum.view);

      auto camera_mapped = camera_data->Map();
      memcpy(camera_mapped, &camera_frustum, sizeof(Frustum));
      camera_data->Unmap();
    }

    {
      const auto proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 100.0f);
      const auto proj_inv = glm::inverse(proj);

      const auto x = glm::f32vec3{ 1.0f, 0.0f, 0.0f };
      const auto y = glm::f32vec3{ 0.0f, 1.0f, 0.0f };
      const auto z = glm::f32vec3{ 0.0f, 0.0f, 1.0f };

      Frustum shadow_frustums[6];
      for (uint32_t i = 0; i < 6; ++i)
      {
        auto view = glm::f32mat4x4(1.0);

        switch (i)
        {
        case 0:
          view = glm::lookAt(light_position, light_position + x, y); break;
        case 1:
          view = glm::lookAt(light_position, light_position - x, y); break;
        case 2:
          view = glm::lookAt(light_position, light_position + y, -z); break;
        case 3:
          view = glm::lookAt(light_position, light_position - y, z); break;
        case 4:
          view = glm::lookAt(light_position, light_position + z, y); break;
        case 5:
          view = glm::lookAt(light_position, light_position - z, y); break;
        };

        const auto view_inv = glm::inverse(view);

        shadow_frustums[i].view = view;
        shadow_frustums[i].view_inv = view_inv;
        shadow_frustums[i].proj = proj;
        shadow_frustums[i].proj_inv = proj_inv;
      }

      auto shadow_mapped = shadow_data->Map();
      memcpy(shadow_mapped, &shadow_frustums, sizeof(Frustum) * 6);
      shadow_data->Unmap();
    }
  }

  void Spark::Discard()
  {
    color_target->Discard();
    depth_target->Discard();
    shadow_map->Discard();

    screen_data->Discard();
    camera_data->Discard();
    shadow_data->Discard();

    scene_instances->Discard();
    scene_triangles->Discard();
    scene_vertices0->Discard();
    scene_vertices1->Discard();
    scene_vertices2->Discard();
    scene_vertices3->Discard();

    scene_textures0->Discard();
    scene_textures1->Discard();
    scene_textures2->Discard();
    scene_textures3->Discard();

    light_maps->Discard();

    skybox_vertices->Discard();
    skybox_triangles->Discard();
    skybox_texture->Discard();

    graphic_arguments->Discard();
    compute_arguments->Discard();

    shadowmap_layout->Discard();
    unshadowed_layout->Discard();
    shadowed_layout->Discard();
    skybox_layout->Discard();
    present_layout->Discard();

    shadowmap_config->Discard();
    unshadowed_config->Discard();
    shadowed_config->Discard();
    skybox_config->Discard();
    present_config->Discard();

    shadowmap_passes[0]->Discard();
    shadowmap_passes[1]->Discard();
    shadowmap_passes[2]->Discard();
    shadowmap_passes[3]->Discard();
    shadowmap_passes[4]->Discard();
    shadowmap_passes[5]->Discard();
    unshadowed_pass->Discard();
    shadowed_pass->Discard();
    //skybox_pass->Discard();
    present_pass->Discard();
  }

  Spark::Spark(Root& root)
    : Broker("spark_broker", root)
  {
    auto* device = root.GetCore()->GetDevice().get();

    //shadow_map = device->CreateResource("spark_shadow_map");
    //color_target = device->CreateResource("spark_color_target");
    //depth_target = device->CreateResource("spark_depth_target");
    //
    //screen_data = device->CreateResource("spark_screen_data");
    //camera_data = device->CreateResource("spark_camera_data");
    //shadow_data = device->CreateResource("spark_shadow_data");
    //
    //scene_instances = device->CreateResource("spark_scene_instances");
    //scene_triangles = device->CreateResource("spark_scene_triangles");
    //scene_vertices0 = device->CreateResource("spark_scene_vertices0");
    //scene_vertices1 = device->CreateResource("spark_scene_vertices1");
    //scene_vertices2 = device->CreateResource("spark_scene_vertices2");
    //scene_vertices3 = device->CreateResource("spark_scene_vertices3");
    //
    //scene_textures0 = device->CreateResource("spark_scene_textures0");
    //scene_textures1 = device->CreateResource("spark_scene_textures1");
    //scene_textures2 = device->CreateResource("spark_scene_textures2");
    //scene_textures3 = device->CreateResource("spark_scene_textures3");
    //
    //light_maps = device->CreateResource("spark_light_maps");
    //
    //skybox_vertices = device->CreateResource("spark_skybox_vertices");
    //skybox_triangles = device->CreateResource("spark_skybox_triangles");
    //skybox_texture = device->CreateResource("spark_skybox_textures");
    //
    //graphic_arguments = device->CreateResource("spark_graphic_arguments");
    //compute_arguments = device->CreateResource("spark_compute_arguments");
    //
    //shadowmap_layout = device->CreateLayout("spark_shadowmap_layout");
    //shadowed_layout = device->CreateLayout("spark_shadowed_layout");
    //unshadowed_layout = device->CreateLayout("spark_unshadowed_layout");
    //skybox_layout = device->CreateLayout("spark_skybox_layout");
    //present_layout = device->CreateLayout("spark_present_layout");
    //
    //shadowmap_config = device->CreateConfig("spark_shadowmap_config");
    //shadowed_config = device->CreateConfig("spark_shadowed_config");
    //unshadowed_config = device->CreateConfig("spark_unshadowed_config");
    //skybox_config = device->CreateConfig("spark_skybox_config");
    //present_config = device->CreateConfig("spark_present_config");
    //
    //shadowmap_passes[0] = device->CreatePass("spark_shadowmap_pass_0");
    //shadowmap_passes[1] = device->CreatePass("spark_shadowmap_pass_1");
    //shadowmap_passes[2] = device->CreatePass("spark_shadowmap_pass_2");
    //shadowmap_passes[3] = device->CreatePass("spark_shadowmap_pass_3");
    //shadowmap_passes[4] = device->CreatePass("spark_shadowmap_pass_4");
    //shadowmap_passes[5] = device->CreatePass("spark_shadowmap_pass_5");
    //shadowed_pass = device->CreatePass("spark_shadowed_pass");
    //unshadowed_pass = device->CreatePass("spark_unshadowed_pass");
    ////skybox_pass = device->CreatePass("spark_skybox_pass");
    //present_pass = device->CreatePass("spark_present_pass");
  }

  Spark::~Spark()
  {
    auto* device = root.GetCore()->GetDevice().get();

    //device->DestroyPass(shadowmap_passes[0]);
    //device->DestroyPass(shadowmap_passes[1]);
    //device->DestroyPass(shadowmap_passes[2]);
    //device->DestroyPass(shadowmap_passes[3]);
    //device->DestroyPass(shadowmap_passes[4]);
    //device->DestroyPass(shadowmap_passes[5]);
    //device->DestroyPass(shadowed_pass);
    //device->DestroyPass(unshadowed_pass);
    ////device->DestroyPass(skybox_pass);
    //device->DestroyPass(present_pass);
    //
    //device->DestroyConfig(shadowmap_config);
    //device->DestroyConfig(shadowed_config);
    //device->DestroyConfig(unshadowed_config);
    //device->DestroyConfig(skybox_config);
    //device->DestroyConfig(present_config);
    //
    //device->DestroyLayout(shadowmap_layout);
    //device->DestroyLayout(shadowed_layout);
    //device->DestroyLayout(unshadowed_layout);
    //device->DestroyLayout(skybox_layout);
    //device->DestroyLayout(present_layout);
    //
    //device->DestroyResource(graphic_arguments);
    //device->DestroyResource(compute_arguments);
    //
    //device->DestroyResource(skybox_vertices);
    //device->DestroyResource(skybox_triangles);
    //device->DestroyResource(skybox_texture);
    //
    //device->DestroyResource(light_maps);
    //
    //device->DestroyResource(scene_textures0);
    //device->DestroyResource(scene_textures1);
    //device->DestroyResource(scene_textures2);
    //device->DestroyResource(scene_textures3);
    //
    //device->DestroyResource(scene_instances);
    //device->DestroyResource(scene_triangles);
    //device->DestroyResource(scene_vertices0);
    //device->DestroyResource(scene_vertices1);
    //device->DestroyResource(scene_vertices2);
    //device->DestroyResource(scene_vertices3);
    //
    //device->DestroyResource(screen_data);
    //device->DestroyResource(camera_data);
    //device->DestroyResource(shadow_data);
    //
    //device->DestroyResource(shadow_map);
    //device->DestroyResource(color_target);
    //device->DestroyResource(depth_target);
  }
}