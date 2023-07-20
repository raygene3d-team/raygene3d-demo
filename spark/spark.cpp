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
  std::shared_ptr<Resource> Spark::RegisterShadowMap(Device* device, const std::string& name)
  {
    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(shadow_resolution);
    resource->SetExtentY(shadow_resolution);
    resource->SetLayers(6);
    resource->SetMipmaps(1);
    resource->SetFormat(FORMAT_D16_UNORM);
    resource->SetHint(Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE));

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterColorTarget(Device* device, const std::string& name)
  {
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(extent_x);
    resource->SetExtentY(extent_y);
    resource->SetLayers(1);
    resource->SetMipmaps(1);
    resource->SetFormat(FORMAT_R11G11B10_FLOAT);
    
    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterDepthTarget(Device* device, const std::string& name)
  {
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(extent_x);
    resource->SetExtentY(extent_y);
    resource->SetLayers(1);
    resource->SetMipmaps(1);
    resource->SetFormat(FORMAT_D32_FLOAT);
    
    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterScreenData(Device* device, const std::string& name)
  {
    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Screen)));
    resource->SetCount(1);
    resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterCameraData(Device* device, const std::string& name)
  {
    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Frustum)));
    resource->SetCount(1);
    resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);
    
    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterShadowData(Device* device, const std::string& name)
  {
    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Frustum)));
    resource->SetCount(6);
    resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);
   
    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneInstances(Device* device, const std::string& name)
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Instance)));
    resource->SetCount(count);
    resource->SetInteropCount(1);
    resource->SetInteropItem(0, prop_instances->GetRawBytes(0));

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneTriangles(Device* device, const std::string& name)
  {
    const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);

    const auto resource = device->CreateResource(name);     

    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Triangle)));
    resource->SetCount(count);
    resource->SetInteropCount(1);
    resource->SetInteropItem(0, prop_triangles->GetRawBytes(0));

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneVertices0(Device* device, const std::string& name)
  {
    const auto [data, count] = prop_vertices0->GetTypedBytes<Vertex0>(0);

    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Vertex0)));
    resource->SetCount(count);
    resource->SetInteropCount(1);
    resource->SetInteropItem(0, prop_vertices0->GetRawBytes(0));

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneVertices1(Device* device, const std::string& name)
  {
    const auto [data, count] = prop_vertices1->GetTypedBytes<Vertex1>(0);

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Vertex1)));
    resource->SetCount(count);
    resource->SetInteropCount(1);
    resource->SetInteropItem(0, prop_vertices1->GetRawBytes(0));

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneVertices2(Device* device, const std::string& name)
  {
    const auto [data, count] = prop_vertices2->GetTypedBytes<Vertex2>(0);

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Vertex2)));
    resource->SetCount(count);
    resource->SetInteropCount(1);
    resource->SetInteropItem(0, prop_vertices2->GetRawBytes(0));

    return resource;
  }
  
  std::shared_ptr<Resource> Spark::RegisterSceneVertices3(Device* device, const std::string& name)
  {
    const auto [data, count] = prop_vertices3->GetTypedBytes<Vertex3>(0);

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Vertex3)));
    resource->SetCount(count);
    resource->SetInteropCount(1);
    resource->SetInteropItem(0, prop_vertices3->GetRawBytes(0));

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneTextures0(Device* device, const std::string& name)
  {
    const auto layers = prop_textures0->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures0->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(size_x);
    resource->SetExtentY(size_y);
    resource->SetExtentZ(1);
    resource->SetHint(Resource::HINT_LAYERED_IMAGE);
    resource->SetLayers(layers);
    resource->SetMipmaps(mipmaps);
    resource->SetFormat(format);
    resource->SetInteropCount(layers);
    for (uint32_t i = 0; i < layers; ++i) { resource->SetInteropItem(i, prop_textures0->GetArrayItem(i)->GetRawBytes(0)); }

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneTextures1(Device* device, const std::string& name)
  {
    const auto layers = prop_textures1->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures1->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(size_x);
    resource->SetExtentY(size_y);
    resource->SetExtentZ(1);
    resource->SetHint(Resource::HINT_LAYERED_IMAGE);
    resource->SetLayers(layers);
    resource->SetMipmaps(mipmaps);
    resource->SetFormat(format);
    resource->SetInteropCount(layers);
    for (uint32_t i = 0; i < layers; ++i) { resource->SetInteropItem(i, prop_textures1->GetArrayItem(i)->GetRawBytes(0)); }

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneTextures2(Device* device, const std::string& name)
  {
    const auto layers = prop_textures2->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures2->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(size_x);
    resource->SetExtentY(size_y);
    resource->SetExtentZ(1);
    resource->SetHint(Resource::HINT_LAYERED_IMAGE);
    resource->SetLayers(layers);
    resource->SetMipmaps(mipmaps);
    resource->SetFormat(format);
    resource->SetInteropCount(layers);
    for (uint32_t i = 0; i < layers; ++i) { resource->SetInteropItem(i, prop_textures2->GetArrayItem(i)->GetRawBytes(0)); }

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSceneTextures3(Device* device, const std::string& name)
  {
    const auto layers = prop_textures3->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_textures3->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(size_x);
    resource->SetExtentY(size_y);
    resource->SetExtentZ(1);
    resource->SetHint(Resource::HINT_LAYERED_IMAGE);
    resource->SetLayers(layers);
    resource->SetMipmaps(mipmaps);
    resource->SetFormat(format);
    resource->SetInteropCount(layers);
    for (uint32_t i = 0; i < layers; ++i) { resource->SetInteropItem(i, prop_textures3->GetArrayItem(i)->GetRawBytes(0)); }

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterLightMaps(Device* device, const std::string& name)
  {
    const auto layers = prop_lightmaps->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto size_x = 1u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_lightmaps->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(size_x);
    resource->SetExtentY(size_y);
    resource->SetExtentZ(1);
    resource->SetHint(Resource::HINT_LAYERED_IMAGE);
    resource->SetLayers(layers);
    resource->SetMipmaps(mipmaps);
    resource->SetFormat(format);
    resource->SetInteropCount(layers);
    for (uint32_t i = 0; i < layers; ++i) { resource->SetInteropItem(i, prop_lightmaps->GetArrayItem(i)->GetRawBytes(0)); }

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSkyboxVertices(Device* device, const std::string& name)
  {
    static const std::array<glm::f32vec4, 4> quad_vtx = {
      glm::f32vec4(-1.0f, 1.0f, 0.0f, 0.0f),
      glm::f32vec4(1.0f, 1.0f, 1.0f, 0.0f),
      glm::f32vec4(-1.0f,-1.0f, 0.0f, 1.0f),
      glm::f32vec4(1.0f,-1.0f, 1.0f, 1.0f),
    };

    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(glm::f32vec4)));
    resource->SetCount(uint32_t(quad_vtx.size()));
    resource->SetInteropCount(1);
    resource->SetInteropItem(0, std::pair(quad_vtx.data(), uint32_t(quad_vtx.size() * sizeof(glm::f32vec4))));

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSkyboxTriangles(Device* device, const std::string& name)
  {
    static const std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(glm::u32vec3)));
    resource->SetCount(uint32_t(quad_idx.size()));
    resource->SetInteropCount(1);
    resource->SetInteropItem(0, std::pair(quad_idx.data(), uint32_t(quad_idx.size() * sizeof(glm::u32vec3))));
    
    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterSkyboxTexture(Device* device, const std::string& name)
  {
    const auto layers = prop_skybox->GetArraySize();
    const auto format = FORMAT_R32G32B32A32_FLOAT;
    const auto bpp = 16u;

    auto mipmaps = 1u;
    auto size_x = 2u;
    auto size_y = 1u;
    auto size = 0u;
    while ((size += size_x * size_y * bpp) != prop_skybox->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_IMAGE2D);
    resource->SetExtentX(size_x);
    resource->SetExtentY(size_y);
    resource->SetExtentZ(1);
    resource->SetLayers(layers);
    resource->SetMipmaps(mipmaps);
    resource->SetFormat(format);
    resource->SetInteropCount(layers);
    for (uint32_t i = 0; i < layers; ++i) { resource->SetInteropItem(i, prop_skybox->GetArrayItem(i)->GetRawBytes(0)); }

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterGraphicArguments(Device* device, const std::string& name)
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

    const auto resource = device->CreateResource(name);

    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Pass::Argument)));
    resource->SetCount(uint32_t(count));
    resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);

    return resource;
  }

  std::shared_ptr<Resource> Spark::RegisterComputeArguments(Device* device, const std::string& name)
  {
    const auto resource = device->CreateResource(name);
    
    resource->SetType(Resource::TYPE_BUFFER);
    resource->SetStride(uint32_t(sizeof(Pass::Argument)));
    resource->SetCount(1);
    resource->SetHint(Resource::HINT_DYNAMIC_BUFFER);
    
    return resource;
  }

  std::shared_ptr<Layout> Spark::RegisterShadowmapLayout(Device* device, const std::string& name)
  {
    const auto layout = device->CreateLayout(name);

    const auto shadowmap_camera_data = shadow_data->CreateView("spark_shadowmap_shadow_data");
    {
      shadowmap_camera_data->SetBind(View::BIND_CONSTANT_DATA);
      shadowmap_camera_data->SetByteOffset(0);
      shadowmap_camera_data->SetByteCount(sizeof(Frustum));
    }

    const std::shared_ptr<View> sb_views[] = {
      shadowmap_camera_data,
    };
    layout->UpdateSBViews({ sb_views, uint32_t(std::size(sb_views)) });

    return layout;
  }

  std::shared_ptr<Config> Spark::RegisterShadowmapConfig(Device* device, const std::string& name)
  {
    const auto config = device->CreateConfig(name);

    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_shadow.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    config->SetSource(shader_ss.str());
    config->SetCompilation(Config::Compilation(Config::COMPILATION_VS));
    config->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
    config->SetIndexer(Config::INDEXER_32_BIT);

    const Config::Attribute attributes[] = {
      { 0,  0, 16, FORMAT_R32G32B32_FLOAT, false },
    };
    config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

    config->SetFillMode(Config::FILL_SOLID);
    config->SetCullMode(Config::CULL_FRONT);
    config->SetClipEnabled(false);

    const Config::Blend blends[] = {
      { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
    };
    config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
    config->SetATCEnabled(false);

    config->SetDepthEnabled(true);
    config->SetDepthWrite(true);
    config->SetDepthComparison(Config::COMPARISON_LESS);

    const Config::Viewport viewports[] = {
      { 0.0f, 0.0f, float(shadow_resolution), float(shadow_resolution), 0.0f, 1.0f },
    };
    config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });

    return config;
  }

  std::shared_ptr<Pass> Spark::RegisterShadowmapPass(Device* device, const std::string& name, uint32_t index)
  {
    const auto pass = device->CreatePass(name + "_" + std::to_string(index));

    pass->SetType(Pass::TYPE_GRAPHIC);
    pass->SetEnabled(true);

    const auto shadowmap_shadow_map = shadow_map->CreateView("spark_shadowmap_shadow_map_" + std::to_string(index));
    shadowmap_shadow_map->SetBind(View::BIND_DEPTH_STENCIL);
    shadowmap_shadow_map->SetLayerOffset(index);
    shadowmap_shadow_map->SetLayerCount(1);

    const std::shared_ptr<View> ds_views[] = {
      shadowmap_shadow_map,
    };
    pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

    const Pass::DSValue ds_values[] = {
      { 1.0f, std::nullopt },
    };
    pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

    pass->SetSubpassCount(1);

    const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    std::vector<Pass::Command> commands(instance_count);
    for (uint32_t j = 0; j < instance_count; ++j)
    {
      const auto shadowmap_graphic_argument = graphic_arguments->CreateView("spark_shadowmap_graphic_argument_" + std::to_string(index) + "_" + std::to_string(j));
      shadowmap_graphic_argument->SetBind(View::BIND_COMMAND_INDIRECT);
      shadowmap_graphic_argument->SetByteOffset(j * uint32_t(sizeof(Pass::Argument)));
      shadowmap_graphic_argument->SetByteCount(uint32_t(sizeof(Pass::Argument)));

      commands[j].view = shadowmap_graphic_argument;
      commands[j].offsets = { index * uint32_t(sizeof(Frustum)) };
    }
    pass->UpdateSubpassCommands(0, { commands.data(), uint32_t(commands.size()) });

    const auto shadowmap_scene_vertices0 = scene_vertices0->CreateView("spark_shadowmap_scene_vertices0");
    {
      shadowmap_scene_vertices0->SetBind(View::BIND_VERTEX_ARRAY);
      shadowmap_scene_vertices0->SetByteOffset(0);
      shadowmap_scene_vertices0->SetByteCount(uint32_t(-1));
    }

    const std::shared_ptr<View> va_views[] = {
      shadowmap_scene_vertices0,
    };
    pass->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });

    const auto shadowmap_scene_triangles = scene_triangles->CreateView("spark_shadowmap_scene_triangles");
    {
      shadowmap_scene_triangles->SetBind(View::BIND_INDEX_ARRAY);
      shadowmap_scene_triangles->SetByteOffset(0);
      shadowmap_scene_triangles->SetByteCount(uint32_t(-1));
    }

    const std::shared_ptr<View> ia_views[] = {
      shadowmap_scene_triangles,
    };
    pass->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });

    pass->SetSubpassLayout(0, shadowmap_layout);
    pass->SetSubpassConfig(0, shadowmap_config);
    
    return pass;
  }

  std::shared_ptr<Layout> Spark::RegisterUnshadowedLayout(Device* device, const std::string& name)
  {
    const auto layout = device->CreateLayout(name);

    const Layout::Sampler samplers[] = {
    { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };
    layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });

    const auto unshadowed_screen_data = screen_data->CreateView("spark_unshadowed_screen_data");
    {
      unshadowed_screen_data->SetBind(View::BIND_CONSTANT_DATA);
      unshadowed_screen_data->SetByteOffset(0);
      unshadowed_screen_data->SetByteCount(uint32_t(-1));
    }

    const auto unshadowed_camera_data = camera_data->CreateView("spark_unshadowed_camera_data");
    {
      unshadowed_camera_data->SetBind(View::BIND_CONSTANT_DATA);
      unshadowed_camera_data->SetByteOffset(0);
      unshadowed_camera_data->SetByteCount(uint32_t(-1));
    }

    const auto unshadowed_shadow_data = shadow_data->CreateView("spark_unshadowed_shadow_data");
    {
      unshadowed_shadow_data->SetBind(View::BIND_CONSTANT_DATA);
      unshadowed_shadow_data->SetByteOffset(0);
      unshadowed_shadow_data->SetByteCount(sizeof(Frustum));
    }

    const std::shared_ptr<View> ub_views[] = {
      unshadowed_screen_data,
      unshadowed_camera_data,
      unshadowed_shadow_data,
    };
    layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });


    const auto unshadowed_scene_instances = scene_instances->CreateView("spark_unshadowed_scene_instances");
    {
      unshadowed_scene_instances->SetBind(View::BIND_CONSTANT_DATA);
      unshadowed_scene_instances->SetByteOffset(0);
      unshadowed_scene_instances->SetByteCount(sizeof(Instance));
    }

    const std::shared_ptr<View> ue_views[] = {
      unshadowed_scene_instances
    };
    layout->UpdateSBViews({ ue_views, uint32_t(std::size(ue_views)) });


    const auto unshadowed_scene_textures0 = scene_textures0->CreateView("spark_unshadowed_scene_textures0");
    {
      unshadowed_scene_textures0->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_scene_textures1 = scene_textures1->CreateView("spark_unshadowed_scene_textures1");
    {
      unshadowed_scene_textures1->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_scene_textures2 = scene_textures2->CreateView("spark_unshadowed_scene_textures2");
    {
      unshadowed_scene_textures2->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_scene_textures3 = scene_textures3->CreateView("spark_unshadowed_scene_textures3");
    {
      unshadowed_scene_textures3->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_light_maps = light_maps->CreateView("spark_unshadowed_light_maps");
    {
      unshadowed_light_maps->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const std::shared_ptr<View> ri_views[] = {
      unshadowed_scene_textures0,
      unshadowed_scene_textures1,
      unshadowed_scene_textures2,
      unshadowed_scene_textures3,
      unshadowed_light_maps,
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    return layout;
  }

  std::shared_ptr<Config> Spark::RegisterUnshadowedConfig(Device* device, const std::string& name)
  {
    const auto config = device->CreateConfig(name);

    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_simple.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    config->SetSource(shader_ss.str());
    config->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));
    config->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
    config->SetIndexer(Config::INDEXER_32_BIT);
    config->SetDefineItem("TEST", "1");

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
    const Config::Attribute attributes[] = {
      { 0,  0, 16, FORMAT_R32G32B32_FLOAT, false },
      { 0, 12, 16, FORMAT_R8G8B8A8_UNORM, false },
      { 1,  0, 16, FORMAT_R32G32B32_FLOAT, false },
      { 1, 12, 16, FORMAT_R32_UINT, false },
      { 2,  0, 16, FORMAT_R32G32B32_FLOAT, false },
      { 2, 12, 16, FORMAT_R32_FLOAT, false },
      { 3,  0, 16, FORMAT_R32G32_FLOAT, false },
      { 3,  8, 16, FORMAT_R32G32_FLOAT, false },
    };
    config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

    config->SetFillMode(Config::FILL_SOLID);
    config->SetCullMode(Config::CULL_BACK);
    config->SetClipEnabled(false);

    const Config::Blend blends[] = {
      { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
    };
    config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
    config->SetATCEnabled(false);

    config->SetDepthEnabled(true);
    config->SetDepthWrite(true);
    config->SetDepthComparison(Config::COMPARISON_LESS);

    const Config::Viewport viewports[] = {
      { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f },
    };
    config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });

    return config;
  }

  std::shared_ptr<Pass> Spark::RegisterUnshadowedPass(Device* device, const std::string& name)
  {
    const auto pass = device->CreatePass(name);

    pass->SetType(Pass::TYPE_GRAPHIC);
    pass->SetEnabled(true);

    const auto unshadowed_color_target = color_target->CreateView("spark_unshadowed_color_target");
    {
      unshadowed_color_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const std::shared_ptr<View> rt_views[] = {
      unshadowed_color_target,
    };
    pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

    const auto unshadowed_depth_target = depth_target->CreateView("spark_unshadowed_depth_target");
    {
      unshadowed_depth_target->SetBind(View::BIND_DEPTH_STENCIL);
    }

    const std::shared_ptr<View> ds_views[] = {
      unshadowed_depth_target,
    };
    pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

    const Pass::RTValue rt_values[] = {
      std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    };
    pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

    const Pass::DSValue ds_values[] = {
      { 1.0f, std::nullopt },
    };
    pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

    pass->SetSubpassCount(ShadingSubpass::SUBPASS_MAX_COUNT);

    {
      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      std::vector<Pass::Command> commands(instance_count);
      for (uint32_t j = 0; j < instance_count; ++j)
      {
        const auto unshadowed_graphic_argument = graphic_arguments->CreateView("spark_unshadowed_graphic_argument_" + std::to_string(j));
        unshadowed_graphic_argument->SetBind(View::BIND_COMMAND_INDIRECT);
        unshadowed_graphic_argument->SetByteOffset(j * uint32_t(sizeof(Pass::Argument)));
        unshadowed_graphic_argument->SetByteCount(uint32_t(sizeof(Pass::Argument)));

        commands[j].view = unshadowed_graphic_argument;
        commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
      }

      pass->UpdateSubpassCommands(ShadingSubpass::SUBPASS_OPAQUE, { commands.data(), uint32_t(commands.size()) });

      const auto unshadowed_scene_vertices0 = scene_vertices0->CreateView("spark_unshadowed_scene_vertices0");
      {
        unshadowed_scene_vertices0->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const auto unshadowed_scene_vertices1 = scene_vertices1->CreateView("spark_unshadowed_scene_vertices1");
      {
        unshadowed_scene_vertices1->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const auto unshadowed_scene_vertices2 = scene_vertices2->CreateView("spark_unshadowed_scene_vertices2");
      {
        unshadowed_scene_vertices2->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const auto unshadowed_scene_vertices3 = scene_vertices3->CreateView("spark_unshadowed_scene_vertices3");
      {
        unshadowed_scene_vertices3->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const std::shared_ptr<View> va_views[] = {
        unshadowed_scene_vertices0,
        unshadowed_scene_vertices1,
        unshadowed_scene_vertices2,
        unshadowed_scene_vertices3,
      };
      pass->UpdateSubpassVAViews(ShadingSubpass::SUBPASS_OPAQUE, { va_views, uint32_t(std::size(va_views)) });

      const auto unshadowed_scene_triangles = scene_triangles->CreateView("spark_unshadowed_scene_triangles");
      {
        unshadowed_scene_triangles->SetBind(View::BIND_INDEX_ARRAY);
      }

      const std::shared_ptr<View> ia_views[] = {
        unshadowed_scene_triangles,
      };
      pass->UpdateSubpassIAViews(ShadingSubpass::SUBPASS_OPAQUE, { ia_views, uint32_t(std::size(ia_views)) });

      pass->SetSubpassLayout(ShadingSubpass::SUBPASS_OPAQUE, unshadowed_layout);
      pass->SetSubpassConfig(ShadingSubpass::SUBPASS_OPAQUE, unshadowed_config);
    }

    {
      Pass::Command commands[] = {
        {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
      };
      pass->UpdateSubpassCommands(ShadingSubpass::SUBPASS_SKYBOX, { commands, uint32_t(std::size(commands)) });

      const auto skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices");
      {
        skybox_skybox_vertices->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const std::shared_ptr<View> va_views[] = {
        skybox_skybox_vertices,
      };
      pass->UpdateSubpassVAViews(ShadingSubpass::SUBPASS_SKYBOX, { va_views, uint32_t(std::size(va_views)) });

      const auto skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles");
      {
        skybox_skybox_triangles->SetBind(View::BIND_INDEX_ARRAY);
      }

      const std::shared_ptr<View> ia_views[] = {
        skybox_skybox_triangles,
      };
      pass->UpdateSubpassIAViews(ShadingSubpass::SUBPASS_SKYBOX, { ia_views, uint32_t(std::size(ia_views)) });

      pass->SetSubpassLayout(ShadingSubpass::SUBPASS_SKYBOX, skybox_layout);
      pass->SetSubpassConfig(ShadingSubpass::SUBPASS_SKYBOX, skybox_config);
    }

    return pass;
  }

  std::shared_ptr<Layout> Spark::RegisterShadowedLayout(Device* device, const std::string& name)
  {
    const auto layout = device->CreateLayout(name);
    
    const Layout::Sampler samplers[] = {
    { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    { Layout::Sampler::FILTERING_NEAREST, 1, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_ALWAYS, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
    };
    layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });

    const auto shadowed_screen_data = screen_data->CreateView("spark_shadowed_screen_data");
    {
      shadowed_screen_data->SetBind(View::BIND_CONSTANT_DATA);
      shadowed_screen_data->SetByteOffset(0);
      shadowed_screen_data->SetByteCount(uint32_t(-1));
    }

    const auto shadowed_camera_data = camera_data->CreateView("spark_shadowed_camera_data");
    {
      shadowed_camera_data->SetBind(View::BIND_CONSTANT_DATA);
      shadowed_camera_data->SetByteOffset(0);
      shadowed_camera_data->SetByteCount(uint32_t(-1));
    }

    const auto shadowed_shadow_data = shadow_data->CreateView("spark_shadowed_shadow_data");
    {
      shadowed_shadow_data->SetBind(View::BIND_CONSTANT_DATA);
      shadowed_shadow_data->SetByteOffset(0);
      shadowed_shadow_data->SetByteCount(sizeof(Frustum));
    }

    const std::shared_ptr<View> ub_views[] = {
      shadowed_screen_data,
      shadowed_camera_data,
      shadowed_shadow_data,
    };
    layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

    const auto shadowed_scene_instances = scene_instances->CreateView("spark_shadowed_scene_instances");
    {
      shadowed_scene_instances->SetBind(View::BIND_CONSTANT_DATA);
      shadowed_scene_instances->SetByteOffset(0);
      shadowed_scene_instances->SetByteCount(sizeof(Instance));
    }

    const std::shared_ptr<View> ue_views[] = {
      shadowed_scene_instances
    };
    layout->UpdateSBViews({ ue_views, uint32_t(std::size(ue_views)) });

    const auto shadowed_scene_textures0 = scene_textures0->CreateView("spark_shadowed_scene_textures0");
    {
      shadowed_scene_textures0->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto shadowed_scene_textures1 = scene_textures1->CreateView("spark_shadowed_scene_textures1");
    {
      shadowed_scene_textures1->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto shadowed_scene_textures2 = scene_textures2->CreateView("spark_shadowed_scene_textures2");
    {
      shadowed_scene_textures2->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto shadowed_scene_textures3 = scene_textures3->CreateView("spark_shadowed_scene_textures3");
    {
      shadowed_scene_textures3->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto shadowed_shadow_map = shadow_map->CreateView("spark_shadowed_shadow_map");
    {
      shadowed_shadow_map->SetBind(View::BIND_SHADER_RESOURCE);
      shadowed_shadow_map->SetUsage(View::USAGE_CUBEMAP_LAYER);
    }

    const std::shared_ptr<View> ri_views[] = {
      shadowed_scene_textures0,
      shadowed_scene_textures1,
      shadowed_scene_textures2,
      shadowed_scene_textures3,
      shadowed_shadow_map,
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    return layout;
  }


  std::shared_ptr<Config> Spark::RegisterShadowedConfig(Device* device, const std::string& name)
  {
    const auto config = device->CreateConfig(name);
    
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_advanced.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    config->SetSource(shader_ss.str());
    config->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));
    config->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
    config->SetIndexer(Config::INDEXER_32_BIT);

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

    const Config::Attribute attributes[] = {
      { 0,  0, 16, FORMAT_R32G32B32_FLOAT, false },
      { 0, 12, 16, FORMAT_R8G8B8A8_UNORM, false },
      { 1,  0, 16, FORMAT_R32G32B32_FLOAT, false },
      { 1, 12, 16, FORMAT_R32_UINT, false },
      { 2,  0, 16, FORMAT_R32G32B32_FLOAT, false },
      { 2, 12, 16, FORMAT_R32_FLOAT, false },
      { 3,  0, 16, FORMAT_R32G32_FLOAT, false },
      { 3,  8, 16, FORMAT_R32G32_FLOAT, false },
    };
    config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

    config->SetFillMode(Config::FILL_SOLID);
    config->SetCullMode(Config::CULL_BACK);
    config->SetClipEnabled(false);

    const Config::Blend blends[] = {
      { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
    };
    config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
    config->SetATCEnabled(false);

    config->SetDepthEnabled(true);
    config->SetDepthWrite(true);
    config->SetDepthComparison(Config::COMPARISON_LESS);

    const Config::Viewport viewports[] = {
      { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f },
    };
    config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });

    return config;
  }


  std::shared_ptr<Pass> Spark::RegisterShadowedPass(Device* device, const std::string& name)
  {
    const auto pass = device->CreatePass(name);

    pass->SetType(Pass::TYPE_GRAPHIC);
    pass->SetEnabled(true);

    const auto shadowed_color_target = color_target->CreateView("spark_shadowed_color_target");
    {
      shadowed_color_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const std::shared_ptr<View> rt_views[] = {
      shadowed_color_target,
    };
    pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

    const auto shadowed_depth_target = depth_target->CreateView("spark_shadowed_depth_target");
    {
      shadowed_depth_target->SetBind(View::BIND_DEPTH_STENCIL);
    }

    const std::shared_ptr<View> ds_views[] = {
      shadowed_depth_target,
    };
    pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

    const Pass::RTValue rt_values[] = {
      std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    };
    pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

    const Pass::DSValue ds_values[] = {
      { 1.0f, std::nullopt },
    };
    pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

    pass->SetSubpassCount(ShadingSubpass::SUBPASS_MAX_COUNT);

    {
      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      std::vector<Pass::Command> commands(instance_count);
      for (uint32_t j = 0; j < instance_count; ++j)
      {
        const auto shadowed_graphic_arguments = graphic_arguments->CreateView("spark_shadowed_graphic_arguments_" + std::to_string(j));
        shadowed_graphic_arguments->SetBind(View::BIND_COMMAND_INDIRECT);
        shadowed_graphic_arguments->SetByteOffset(j * uint32_t(sizeof(Pass::Argument)));
        shadowed_graphic_arguments->SetByteCount(uint32_t(sizeof(Pass::Argument)));

        commands[j].view = shadowed_graphic_arguments;
        commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
      }

      pass->UpdateSubpassCommands(ShadingSubpass::SUBPASS_OPAQUE, { commands.data(), uint32_t(commands.size()) });

      const auto shadowed_scene_vertices0 = scene_vertices0->CreateView("spark_shadowed_scene_vertices0");
      {
        shadowed_scene_vertices0->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const auto shadowed_scene_vertices1 = scene_vertices1->CreateView("spark_shadowed_scene_vertices1");
      {
        shadowed_scene_vertices1->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const auto shadowed_scene_vertices2 = scene_vertices2->CreateView("spark_shadowed_scene_vertices2");
      {
        shadowed_scene_vertices2->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const auto shadowed_scene_vertices3 = scene_vertices3->CreateView("spark_shadowed_scene_vertices3");
      {
        shadowed_scene_vertices3->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const std::shared_ptr<View> va_views[] = {
        shadowed_scene_vertices0,
        shadowed_scene_vertices1,
        shadowed_scene_vertices2,
        shadowed_scene_vertices3,
      };
      pass->UpdateSubpassVAViews(ShadingSubpass::SUBPASS_OPAQUE, { va_views, uint32_t(std::size(va_views)) });

      const auto shadowed_scene_triangles = scene_triangles->CreateView("spark_shadowed_scene_triangles");
      {
        shadowed_scene_triangles->SetBind(View::BIND_INDEX_ARRAY);
      }

      const std::shared_ptr<View> ia_views[] = {
        shadowed_scene_triangles,
      };
      pass->UpdateSubpassIAViews(ShadingSubpass::SUBPASS_OPAQUE, { ia_views, uint32_t(std::size(ia_views)) });

      pass->SetSubpassLayout(ShadingSubpass::SUBPASS_OPAQUE, shadowed_layout);
      pass->SetSubpassConfig(ShadingSubpass::SUBPASS_OPAQUE, shadowed_config);
    }

    {
      Pass::Command commands[] = {
        {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
      };
      pass->UpdateSubpassCommands(ShadingSubpass::SUBPASS_SKYBOX, { commands, uint32_t(std::size(commands)) });

      const auto skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices");
      {
        skybox_skybox_vertices->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const std::shared_ptr<View> va_views[] = {
        skybox_skybox_vertices,
      };
      pass->UpdateSubpassVAViews(ShadingSubpass::SUBPASS_SKYBOX, { va_views, uint32_t(std::size(va_views)) });

      const auto skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles");
      {
        skybox_skybox_triangles->SetBind(View::BIND_INDEX_ARRAY);
      }

      const std::shared_ptr<View> ia_views[] = {
        skybox_skybox_triangles,
      };
      pass->UpdateSubpassIAViews(ShadingSubpass::SUBPASS_SKYBOX, { ia_views, uint32_t(std::size(ia_views)) });

      pass->SetSubpassLayout(ShadingSubpass::SUBPASS_SKYBOX, skybox_layout);
      pass->SetSubpassConfig(ShadingSubpass::SUBPASS_SKYBOX, skybox_config);
    }

    return pass;
  }

  std::shared_ptr<Layout> Spark::RegisterSkyboxLayout(Device* device, const std::string& name)
  {
    const auto layout = device->CreateLayout(name);
    
    const Layout::Sampler samplers[] = {
    { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };
    layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });


    const auto skybox_screen_data = screen_data->CreateView("spark_skybox_screen_data");
    {
      skybox_screen_data->SetBind(View::BIND_CONSTANT_DATA);
    }

    const auto skybox_camera_data = camera_data->CreateView("spark_skybox_camera_data");
    {
      skybox_camera_data->SetBind(View::BIND_CONSTANT_DATA);
    }

    const std::shared_ptr<View> ub_views[] = {
      skybox_screen_data,
      skybox_camera_data,
    };
    layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });


    const auto skybox_skybox_texture = skybox_texture->CreateView("spark_skybox_skybox_texture");
    {
      skybox_skybox_texture->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const std::shared_ptr<View> ri_views[] = {
      skybox_skybox_texture,
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    return layout;
  }

  std::shared_ptr<Config> Spark::RegisterSkyboxConfig(Device* device, const std::string& name)
  {
    const auto config = device->CreateConfig(name);
    
    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_environment.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    config->SetSource(shader_ss.str());
    config->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));
    config->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
    config->SetIndexer(Config::INDEXER_32_BIT);

    const Config::Attribute attributes[] = {
      { 0, 0, 16, FORMAT_R32G32_FLOAT, false },
      { 0, 8, 16, FORMAT_R32G32_FLOAT, false },
    };
    config->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

    config->SetFillMode(Config::FILL_SOLID);
    config->SetCullMode(Config::CULL_NONE);
    config->SetClipEnabled(false);

    const Config::Blend blends[] = {
      { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
    };
    config->UpdateBlends({ blends, uint32_t(std::size(blends)) });
    config->SetATCEnabled(false);

    config->SetDepthEnabled(true);
    config->SetDepthWrite(false);
    config->SetDepthComparison(Config::COMPARISON_EQUAL);

    const Config::Viewport viewports[] = {
      { 0.0f, 0.0f, float(prop_extent_x->GetUint()), float(prop_extent_y->GetUint()), 0.0f, 1.0f },
    };
    config->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });

    return config;
  }

  std::shared_ptr<Pass> Spark::RegisterSkyboxPass(Device* device, const std::string& name)
  {
    const auto pass = device->CreatePass(name);

    pass->SetType(Pass::TYPE_GRAPHIC);
    pass->SetEnabled(true);

    const auto skybox_color_target = color_target->CreateView("spark_skybox_color_target");
    {
      skybox_color_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const std::shared_ptr<View> rt_views[] = {
      skybox_color_target,
    };
    pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

    const auto skybox_depth_target = depth_target->CreateView("spark_skybox_depth_target");
    {
      skybox_depth_target->SetBind(View::BIND_DEPTH_STENCIL);
    }

    const std::shared_ptr<View> ds_views[] = {
      skybox_depth_target,
    };
    pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

    const Pass::RTValue rt_values[] = {
      std::nullopt,
    };
    pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

    const Pass::DSValue ds_values[] = {
      { std::nullopt, std::nullopt },
    };
    pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

    pass->SetSubpassCount(1);

    Pass::Command commands[] = {
      {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
    };
    pass->UpdateSubpassCommands(0, { commands, uint32_t(std::size(commands)) });

    const auto skybox_skybox_vertices = skybox_vertices->CreateView("spark_skybox_vertices");
    {
      skybox_skybox_vertices->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const std::shared_ptr<View> va_views[] = {
      skybox_skybox_vertices,
    };
    pass->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });

    const auto skybox_skybox_triangles = skybox_triangles->CreateView("spark_skybox_triangles");
    {
      skybox_skybox_triangles->SetBind(View::BIND_INDEX_ARRAY);
    }

    const std::shared_ptr<View> ia_views[] = {
      skybox_skybox_triangles,
    };
    pass->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });

    pass->SetSubpassLayout(0, skybox_layout);
    pass->SetSubpassConfig(0, skybox_config);

    return pass;
  }

  std::shared_ptr<Layout> Spark::RegisterPresentLayout(Device* device, const std::string& name)
  {
    const auto layout = device->CreateLayout(name);

    auto present_camera_data = camera_data->CreateView("spark_present_camera_data");
    {
      present_camera_data->SetBind(View::BIND_CONSTANT_DATA);
      present_camera_data->SetByteOffset(0);
      present_camera_data->SetByteCount(uint32_t(-1));
    }
    
    const std::shared_ptr<View> ub_views[] = {
      present_camera_data,
    };
    layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

    layout->UpdateRBViews({});

    layout->UpdateWBViews({});

    auto present_color_target = color_target->CreateView("spark_present_color_target");
    {
      present_color_target->SetBind(View::BIND_SHADER_RESOURCE);
      present_color_target->SetLayerOffset(0);
      present_color_target->SetLayerCount(uint32_t(-1));
    }

    const std::shared_ptr<View> ri_views[] = {
      present_color_target,
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    const std::shared_ptr<View> wi_views[] = {
      backbuffer_rtv,
    };
    layout->UpdateWIViews({ wi_views, uint32_t(std::size(wi_views)) });

    return layout;
  }

  std::shared_ptr<Config> Spark::RegisterPresentConfig(Device* device, const std::string& name)
  {
    const auto config = device->CreateConfig(name);

    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_present.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    config->SetSource(shader_ss.str());
    config->SetCompilation(Config::COMPILATION_CS);

    return config;
  }

  std::shared_ptr<Pass> Spark::RegisterPresentPass(Device* device, const std::string& name)
  {
    const auto pass = device->CreatePass(name);

    pass->SetType(Pass::TYPE_COMPUTE);
    pass->SetEnabled(true);

    pass->SetSubpassCount(1);

    const auto present_compute_arguments = compute_arguments->CreateView("spark_present_compute_arguments");
    {
      present_compute_arguments->SetBind(View::BIND_COMMAND_INDIRECT);
      present_compute_arguments->SetByteOffset(0);
      present_compute_arguments->SetByteCount(uint32_t(-1));
    }
    
    Pass::Command commands[] = {
      {present_compute_arguments},
    };
    pass->UpdateSubpassCommands(0, { commands, uint32_t(std::size(commands)) });

    pass->SetSubpassLayout(0, present_layout);
    pass->SetSubpassConfig(0, present_config);

    return pass;   
  }


  void Spark::Initialize()
  {
    color_target->Initialize();
    depth_target->Initialize();
    shadow_map->Initialize();

    screen_data->Initialize();
    camera_data->Initialize();
    shadow_data->Initialize();
    
    scene_instances->Initialize();
    scene_triangles->Initialize();
    scene_vertices0->Initialize();
    scene_vertices1->Initialize();
    scene_vertices2->Initialize();
    scene_vertices3->Initialize();
 
    scene_textures0->Initialize();
    scene_textures1->Initialize();
    scene_textures2->Initialize();
    scene_textures3->Initialize();
   
    light_maps->Initialize();

    skybox_vertices->Initialize();
    skybox_triangles->Initialize();
    skybox_texture->Initialize();

    graphic_arguments->Initialize();
    compute_arguments->Initialize();

    shadowmap_layout->Initialize();
    unshadowed_layout->Initialize();
    shadowed_layout->Initialize();
    skybox_layout->Initialize();
    present_layout->Initialize();

    shadowmap_config->Initialize();
    unshadowed_config->Initialize();
    shadowed_config->Initialize();
    skybox_config->Initialize();
    present_config->Initialize();

    shadowmap_passes[0]->Initialize();
    shadowmap_passes[1]->Initialize();
    shadowmap_passes[2]->Initialize();
    shadowmap_passes[3]->Initialize();
    shadowmap_passes[4]->Initialize();
    shadowmap_passes[5]->Initialize();    
    unshadowed_pass->Initialize();
    shadowed_pass->Initialize();
    //skybox_pass->Initialize();
    present_pass->Initialize();
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

    const auto find_prop_fn = [this](const std::shared_ptr<Property>& prop)
    {
      if (prop->HasObjectItem("scene"))
      {
        this->prop_scene = prop->GetObjectItem("scene");
      }

      if (prop->HasObjectItem("camera"))
      {
        this->prop_camera = prop->GetObjectItem("camera");
      }

      if (prop->HasObjectItem("environment"))
      {
        this->prop_environment = prop->GetObjectItem("environment");
      }
    };
    root.GetData()->VisitProperty(find_prop_fn);
     

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

    auto* device = root.GetCore()->GetDevice().get();

    shadow_map = RegisterShadowMap(device, "spark_shadow_map");
    color_target = RegisterColorTarget(device, "spark_color_target");
    depth_target = RegisterDepthTarget(device, "spark_depth_target");

    screen_data = RegisterScreenData(device, "spark_screen_data");
    camera_data = RegisterCameraData(device, "spark_camera_data");
    shadow_data = RegisterShadowData(device, "spark_shadow_data");

    scene_instances = RegisterSceneInstances(device, "spark_scene_instances");
    scene_triangles = RegisterSceneTriangles(device, "spark_scene_triangles");
    scene_vertices0 = RegisterSceneVertices0(device, "spark_scene_vertices0");
    scene_vertices1 = RegisterSceneVertices1(device, "spark_scene_vertices1");
    scene_vertices2 = RegisterSceneVertices2(device, "spark_scene_vertices2");
    scene_vertices3 = RegisterSceneVertices3(device, "spark_scene_vertices3");

    scene_textures0 = RegisterSceneTextures0(device, "spark_scene_textures0");
    scene_textures1 = RegisterSceneTextures1(device, "spark_scene_textures1");
    scene_textures2 = RegisterSceneTextures2(device, "spark_scene_textures2");
    scene_textures3 = RegisterSceneTextures3(device, "spark_scene_textures3");

    light_maps = RegisterLightMaps(device, "spark_light_maps");

    skybox_vertices = RegisterSkyboxVertices(device, "spark_skybox_vertices");
    skybox_triangles = RegisterSkyboxTriangles(device, "spark_skybox_triangles");
    skybox_texture = RegisterSkyboxTexture(device, "spark_skybox_textures");

    graphic_arguments = RegisterGraphicArguments(device, "spark_graphic_arguments");
    compute_arguments = RegisterComputeArguments(device, "spark_compute_arguments");

    shadowmap_layout = RegisterShadowmapLayout(device, "spark_shadowmap_layout");
    shadowed_layout = RegisterShadowedLayout(device, "spark_shadowed_layout");
    unshadowed_layout = RegisterUnshadowedLayout(device, "spark_unshadowed_layout");
    skybox_layout = RegisterSkyboxLayout(device, "spark_skybox_layout");
    present_layout = RegisterPresentLayout(device, "spark_present_layout");

    shadowmap_config = RegisterShadowmapConfig(device, "spark_shadowmap_config");
    shadowed_config = RegisterShadowedConfig(device, "spark_shadowed_config");
    unshadowed_config = RegisterUnshadowedConfig(device, "spark_unshadowed_config");
    skybox_config = RegisterSkyboxConfig(device, "spark_skybox_config");
    present_config = RegisterPresentConfig(device, "spark_present_config");

    shadowmap_passes[0] = RegisterShadowmapPass(device, "spark_shadowmap_pass", 0);
    shadowmap_passes[1] = RegisterShadowmapPass(device, "spark_shadowmap_pass", 1);
    shadowmap_passes[2] = RegisterShadowmapPass(device, "spark_shadowmap_pass", 2);
    shadowmap_passes[3] = RegisterShadowmapPass(device, "spark_shadowmap_pass", 3);
    shadowmap_passes[4] = RegisterShadowmapPass(device, "spark_shadowmap_pass", 4);
    shadowmap_passes[5] = RegisterShadowmapPass(device, "spark_shadowmap_pass", 5);
    shadowed_pass = RegisterShadowedPass(device, "spark_shadowed_pass");
    unshadowed_pass = RegisterUnshadowedPass(device, "spark_unshadowed_pass");
    //skybox_pass = RegisterSkyboxPass("spark_skybox_pass");
    present_pass = RegisterPresentPass(device, "spark_present_pass");
  }

  Spark::~Spark()
  {
  }
}