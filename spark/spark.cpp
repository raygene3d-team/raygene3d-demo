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
  void Spark::CreateResources(const std::shared_ptr<Device>& device)
  {
    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    {
      shadow_map = device->CreateResource("spark_shadow_map");
      {
        shadow_map->SetType(Resource::TYPE_IMAGE2D);
        shadow_map->SetExtentX(shadow_resolution);
        shadow_map->SetExtentY(shadow_resolution);
        shadow_map->SetLayers(6);
        shadow_map->SetMipmaps(1);
        shadow_map->SetFormat(FORMAT_D16_UNORM);
        shadow_map->SetHint(Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE));
      }

      render_target = device->CreateResource("spark_render_target");
      {
        render_target->SetType(Resource::TYPE_IMAGE2D);
        render_target->SetExtentX(extent_x);
        render_target->SetExtentY(extent_y);
        render_target->SetLayers(1);
        render_target->SetMipmaps(1);
        render_target->SetFormat(FORMAT_R11G11B10_FLOAT);
      }

      depth_stencil = device->CreateResource("spark_depth_stencil");
      {
        depth_stencil->SetType(Resource::TYPE_IMAGE2D);
        depth_stencil->SetExtentX(extent_x);
        depth_stencil->SetExtentY(extent_y);
        depth_stencil->SetLayers(1);
        depth_stencil->SetMipmaps(1);
        depth_stencil->SetFormat(FORMAT_D32_FLOAT);
      }

      screen_data = device->CreateResource("spark_screen_data");
      {
        screen_data->SetType(Resource::TYPE_BUFFER);
        screen_data->SetStride(uint32_t(sizeof(Screen)));
        screen_data->SetCount(1);
        screen_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      camera_data = device->CreateResource("spark_camera_data");
      {
        camera_data->SetType(Resource::TYPE_BUFFER);
        camera_data->SetStride(uint32_t(sizeof(Frustum)));
        camera_data->SetCount(1);
        camera_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      shadow_data = device->CreateResource("spark_shadow_data");
      {
        shadow_data->SetType(Resource::TYPE_BUFFER);
        shadow_data->SetStride(uint32_t(sizeof(Frustum)));
        shadow_data->SetCount(6);
        shadow_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      instance_items = device->CreateResource("spark_instance_items");
      {
        const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

        instance_items->SetType(Resource::TYPE_BUFFER);
        instance_items->SetStride(uint32_t(sizeof(Instance)));
        instance_items->SetCount(count);
        instance_items->SetInteropCount(1);
        instance_items->SetInteropItem(0, prop_instances->GetRawBytes(0));
      }

      triangle_items = device->CreateResource("spark_triangle_items");
      {
        const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);

        triangle_items->SetType(Resource::TYPE_BUFFER);
        triangle_items->SetStride(uint32_t(sizeof(Triangle)));
        triangle_items->SetCount(count);
        triangle_items->SetInteropCount(1);
        triangle_items->SetInteropItem(0, prop_triangles->GetRawBytes(0));
      }

      vertex0_items = device->CreateResource("spark_vertex0_items");
      {
        const auto [data, count] = prop_vertices0->GetTypedBytes<Vertex0>(0);

        vertex0_items->SetType(Resource::TYPE_BUFFER);
        vertex0_items->SetStride(uint32_t(sizeof(Vertex0)));
        vertex0_items->SetCount(count);
        vertex0_items->SetInteropCount(1);
        vertex0_items->SetInteropItem(0, prop_vertices0->GetRawBytes(0));
      }

      vertex1_items = device->CreateResource("spark_vertex1_items");
      {
        const auto [data, count] = prop_vertices1->GetTypedBytes<Vertex1>(0);

        vertex1_items->SetType(Resource::TYPE_BUFFER);
        vertex1_items->SetStride(uint32_t(sizeof(Vertex1)));
        vertex1_items->SetCount(count);
        vertex1_items->SetInteropCount(1);
        vertex1_items->SetInteropItem(0, prop_vertices1->GetRawBytes(0));
      }

      vertex2_items = device->CreateResource("spark_vertex2_items");
      {
        const auto [data, count] = prop_vertices2->GetTypedBytes<Vertex2>(0);

        vertex2_items->SetType(Resource::TYPE_BUFFER);
        vertex2_items->SetStride(uint32_t(sizeof(Vertex2)));
        vertex2_items->SetCount(count);
        vertex2_items->SetInteropCount(1);
        vertex2_items->SetInteropItem(0, prop_vertices2->GetRawBytes(0));
      }

      vertex3_items = device->CreateResource("spark_vertex3_items");
      {
        const auto [data, count] = prop_vertices3->GetTypedBytes<Vertex3>(0);

        vertex3_items->SetType(Resource::TYPE_BUFFER);
        vertex3_items->SetStride(uint32_t(sizeof(Vertex3)));
        vertex3_items->SetCount(count);
        vertex3_items->SetInteropCount(1);
        vertex3_items->SetInteropItem(0, prop_vertices3->GetRawBytes(0));
      }

      raster_arguments = device->CreateResource("spark_raster_arguments");
      {
        const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

        raster_arguments->SetType(Resource::TYPE_BUFFER);
        raster_arguments->SetStride(uint32_t(sizeof(Pass::Argument)));
        raster_arguments->SetCount(uint32_t(count));
        raster_arguments->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      compute_arguments = device->CreateResource("spark_compute_arguments");
      {
        compute_arguments->SetType(Resource::TYPE_BUFFER);
        compute_arguments->SetStride(uint32_t(sizeof(Pass::Argument)));
        compute_arguments->SetCount(1);
        compute_arguments->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      texture0_items = device->CreateResource("spark_texture0_items");
      {
        const auto layers = prop_textures0->GetArraySize();
        const auto format = FORMAT_R8G8B8A8_SRGB;
        const auto bpp = 4u;

        auto mipmaps = 1u;
        auto size_x = 1u;
        auto size_y = 1u;
        auto size = 0u;
        while ((size += size_x * size_y * bpp) != prop_textures0->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

        texture0_items->SetType(Resource::TYPE_IMAGE2D);
        texture0_items->SetExtentX(size_x);
        texture0_items->SetExtentY(size_y);
        texture0_items->SetExtentZ(1);
        texture0_items->SetHint(Resource::HINT_LAYERED_IMAGE);
        texture0_items->SetLayers(layers);
        texture0_items->SetMipmaps(mipmaps);
        texture0_items->SetFormat(format);
        texture0_items->SetInteropCount(layers);
        for (uint32_t i = 0; i < layers; ++i)
        { 
          texture0_items->SetInteropItem(i, prop_textures0->GetArrayItem(i)->GetRawBytes(0));
        }
      }

      texture1_items = device->CreateResource("spark_texture1_items");
      {
        const auto layers = prop_textures1->GetArraySize();
        const auto format = FORMAT_R8G8B8A8_UNORM;
        const auto bpp = 4u;

        auto mipmaps = 1u;
        auto size_x = 1u;
        auto size_y = 1u;
        auto size = 0u;
        while ((size += size_x * size_y * bpp) != prop_textures1->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

        texture1_items->SetType(Resource::TYPE_IMAGE2D);
        texture1_items->SetExtentX(size_x);
        texture1_items->SetExtentY(size_y);
        texture1_items->SetExtentZ(1);
        texture1_items->SetHint(Resource::HINT_LAYERED_IMAGE);
        texture1_items->SetLayers(layers);
        texture1_items->SetMipmaps(mipmaps);
        texture1_items->SetFormat(format);
        texture1_items->SetInteropCount(layers);
        for (uint32_t i = 0; i < layers; ++i)
        {
          texture1_items->SetInteropItem(i, prop_textures1->GetArrayItem(i)->GetRawBytes(0));
        }
      }

      texture2_items = device->CreateResource("spark_texture2_items");
      {
        const auto layers = prop_textures2->GetArraySize();
        const auto format = FORMAT_R8G8B8A8_UNORM;
        const auto bpp = 4u;

        auto mipmaps = 1u;
        auto size_x = 1u;
        auto size_y = 1u;
        auto size = 0u;
        while ((size += size_x * size_y * bpp) != prop_textures2->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

        texture2_items->SetType(Resource::TYPE_IMAGE2D);
        texture2_items->SetExtentX(size_x);
        texture2_items->SetExtentY(size_y);
        texture2_items->SetExtentZ(1);
        texture2_items->SetHint(Resource::HINT_LAYERED_IMAGE);
        texture2_items->SetLayers(layers);
        texture2_items->SetMipmaps(mipmaps);
        texture2_items->SetFormat(format);
        texture2_items->SetInteropCount(layers);
        for (uint32_t i = 0; i < layers; ++i)
        {
          texture2_items->SetInteropItem(i, prop_textures2->GetArrayItem(i)->GetRawBytes(0));
        }
      }

      texture3_items = device->CreateResource("spark_texture3_items");
      {
        const auto layers = prop_textures3->GetArraySize();
        const auto format = FORMAT_R8G8B8A8_UNORM;
        const auto bpp = 4u;

        auto mipmaps = 1u;
        auto size_x = 1u;
        auto size_y = 1u;
        auto size = 0u;
        while ((size += size_x * size_y * bpp) != prop_textures3->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

        texture3_items->SetType(Resource::TYPE_IMAGE2D);
        texture3_items->SetExtentX(size_x);
        texture3_items->SetExtentY(size_y);
        texture3_items->SetExtentZ(1);
        texture3_items->SetHint(Resource::HINT_LAYERED_IMAGE);
        texture3_items->SetLayers(layers);
        texture3_items->SetMipmaps(mipmaps);
        texture3_items->SetFormat(format);
        texture3_items->SetInteropCount(layers);
        for (uint32_t i = 0; i < layers; ++i)
        {
          texture3_items->SetInteropItem(i, prop_textures3->GetArrayItem(i)->GetRawBytes(0));
        }
      }

      texture4_items = device->CreateResource("spark_texture4_items");
      {
        const auto layers = prop_textures4->GetArraySize();
        const auto format = FORMAT_R8G8B8A8_SRGB;
        const auto bpp = 4u;

        auto mipmaps = 1u;
        auto size_x = 1u;
        auto size_y = 1u;
        auto size = 0u;
        while ((size += size_x * size_y * bpp) != prop_textures4->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

        texture4_items->SetType(Resource::TYPE_IMAGE2D);
        texture4_items->SetExtentX(size_x);
        texture4_items->SetExtentY(size_y);
        texture4_items->SetExtentZ(1);
        texture4_items->SetHint(Resource::HINT_LAYERED_IMAGE);
        texture4_items->SetLayers(layers);
        texture4_items->SetMipmaps(mipmaps);
        texture4_items->SetFormat(format);
        texture4_items->SetInteropCount(layers);
        for (uint32_t i = 0; i < layers; ++i)
        {
          texture4_items->SetInteropItem(i, prop_textures4->GetArrayItem(i)->GetRawBytes(0));
        }
      }

      environment_item = device->CreateResource("spark_environment_item");
      {
        const auto textures = root_property->GetObjectItem("environment");
        const auto layers = textures->GetArraySize();
        const auto format = FORMAT_R32G32B32A32_FLOAT;
        const auto bpp = 16u;

        auto mipmaps = 1u;
        auto size_x = 2u;
        auto size_y = 1u;
        auto size = 0u;
        while ((size += size_x * size_y * bpp) != textures->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u) { mipmaps += 1; size_x <<= 1; size_y <<= 1; }

        environment_item->SetType(Resource::TYPE_IMAGE2D);
        environment_item->SetExtentX(size_x);
        environment_item->SetExtentY(size_y);
        environment_item->SetExtentZ(1);
        environment_item->SetLayers(layers);
        environment_item->SetMipmaps(mipmaps);
        environment_item->SetFormat(format);
        environment_item->SetInteropCount(layers);
        for (uint32_t i = 0; i < layers; ++i)
        {
          environment_item->SetInteropItem(i, textures->GetArrayItem(i)->GetRawBytes(0));
        }
      }

      environment_vtx_data = device->CreateResource("spark_environment_vtx_data");
      {
        environment_vtx_data->SetType(Resource::TYPE_BUFFER);
        environment_vtx_data->SetStride(uint32_t(sizeof(glm::f32vec4)));
        environment_vtx_data->SetCount(uint32_t(environment_vtx.size()));
        environment_vtx_data->SetInteropCount(1);
        environment_vtx_data->SetInteropItem(0, std::pair(environment_vtx.data(), uint32_t(environment_vtx.size() * sizeof(glm::f32vec4))));
      }

      environment_idx_data = device->CreateResource("spark_environment_idx_data");
      {
        environment_idx_data->SetType(Resource::TYPE_BUFFER);
        environment_idx_data->SetStride(uint32_t(sizeof(glm::u32vec3)));
        environment_idx_data->SetCount(uint32_t(environment_idx.size()));
        environment_idx_data->SetInteropCount(1);
        environment_idx_data->SetInteropItem(0, std::pair(environment_idx.data(), uint32_t(environment_idx.size() * sizeof(glm::u32vec3))));
      }
    }
  }


  void Spark::CreateShadowmap(const std::shared_ptr<Device>& device)
  {
    shadowmap_config = CreateShadowmapConfig(device);
    shadowmap_layout = CreateShadowmapLayout(device);

    for (uint32_t i = 0; i < 6; ++i)
    {
      shadowmap_passes[i] = device->CreatePass("spark_shadowmap_" + std::to_string(i));
      shadowmap_passes[i]->SetType(Pass::TYPE_GRAPHIC);
      shadowmap_passes[i]->SetEnabled(true);

      const auto shadowmap_depth_stencil = shadow_map->CreateView("spark_shadowmap_depth_stencil_" + std::to_string(i));
      shadowmap_depth_stencil->SetBind(View::BIND_DEPTH_STENCIL);
      shadowmap_depth_stencil->SetLayerOffset(i);
      shadowmap_depth_stencil->SetLayerCount(1);

      const std::shared_ptr<View> ds_views[] = {
        shadowmap_depth_stencil,
      };
      shadowmap_passes[i]->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

      const Pass::DSValue ds_values[] = {
        { 1.0f, std::nullopt },
      };
      shadowmap_passes[i]->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

      shadowmap_passes[i]->SetSubpassCount(1);

      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      std::vector<Pass::Command> commands(instance_count);
      for (uint32_t j = 0; j < instance_count; ++j)
      {
        const auto argument_view = raster_arguments->CreateView("spark_shadowmap_raster_argument_" + std::to_string(i) + "_" + std::to_string(j));
        argument_view->SetBind(View::BIND_COMMAND_INDIRECT);
        argument_view->SetByteOffset(j * uint32_t(sizeof(Pass::Argument)));
        argument_view->SetByteCount(uint32_t(sizeof(Pass::Argument)));

        commands[j].view = argument_view;
        commands[j].offsets = { i * uint32_t(sizeof(Frustum)) };
      }
      shadowmap_passes[i]->UpdateSubpassCommands(0, { commands.data(), uint32_t(commands.size()) });

      const auto shadowmap_vertex0_items = vertex0_items->CreateView("spark_shadowmap_vertex0_items");
      {
        shadowmap_vertex0_items->SetBind(View::BIND_VERTEX_ARRAY);
        shadowmap_vertex0_items->SetByteOffset(0);
        shadowmap_vertex0_items->SetByteCount(uint32_t(-1));
      }

      const std::shared_ptr<View> va_views[] = {
        shadowmap_vertex0_items,
      };
      shadowmap_passes[i]->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });

      const auto shadowmap_index_items = triangle_items->CreateView("spark_shadowmap_index_items");
      {
        shadowmap_index_items->SetBind(View::BIND_INDEX_ARRAY);
        shadowmap_index_items->SetByteOffset(0);
        shadowmap_index_items->SetByteCount(uint32_t(-1));
      }

      const std::shared_ptr<View> ia_views[] = {
        shadowmap_index_items,
      };
      shadowmap_passes[i]->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });

      shadowmap_passes[i]->SetSubpassLayout(0, shadowmap_layout);
      shadowmap_passes[i]->SetSubpassConfig(0, shadowmap_config);
    }
  }

  void Spark::CreateUnshadowed(const std::shared_ptr<Device>& device)
  {
    unshadowed_config = CreateUnshadowedConfig(device);
    unshadowed_layout = CreateUnshadowedLayout(device);

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    const auto unshadowed_render_target = render_target->CreateView("spark_unshadowed_render_target");
    {
      unshadowed_render_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const auto unshadowed_depth_stencil = depth_stencil->CreateView("spark_unshadowed_depth_stencil");
    {
      unshadowed_depth_stencil->SetBind(View::BIND_DEPTH_STENCIL);
    }

    unshadowed_pass = device->CreatePass("spark_unshadowed");
    unshadowed_pass->SetType(Pass::TYPE_GRAPHIC);

    const std::shared_ptr<View> rt_views[] = {
      unshadowed_render_target,
    };
    unshadowed_pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

    const std::shared_ptr<View> ds_views[] = {
      unshadowed_depth_stencil,
    };
    unshadowed_pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

    const Pass::RTValue rt_values[] = {
      std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    };
    unshadowed_pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

    const Pass::DSValue ds_values[] = {
      { 1.0f, std::nullopt },
    };
    unshadowed_pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

    unshadowed_pass->SetSubpassCount(1);

    const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    std::vector<Pass::Command> commands(instance_count);
    for (uint32_t j = 0; j < instance_count; ++j)
    {
      const auto argument_view = raster_arguments->CreateView("spark_unshadowed_raster_argument_" + std::to_string(j));
      argument_view->SetBind(View::BIND_COMMAND_INDIRECT);
      argument_view->SetByteOffset(j * uint32_t(sizeof(Pass::Argument)));
      argument_view->SetByteCount(uint32_t(sizeof(Pass::Argument)));

      commands[j].view = argument_view;
      commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
    }

    unshadowed_pass->UpdateSubpassCommands(0, { commands.data(), uint32_t(commands.size()) });

    const auto unshadowed_vertex0_items = vertex0_items->CreateView("spark_unshadowed_vertex0_items");
    {
      unshadowed_vertex0_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto unshadowed_vertex1_items = vertex1_items->CreateView("spark_unshadowed_vertex1_items");
    {
      unshadowed_vertex1_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto unshadowed_vertex2_items = vertex2_items->CreateView("spark_unshadowed_vertex2_items");
    {
      unshadowed_vertex2_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto unshadowed_vertex3_items = vertex3_items->CreateView("spark_unshadowed_vertex3_items");
    {
      unshadowed_vertex3_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const std::shared_ptr<View> va_views[] = {
      unshadowed_vertex0_items,
      unshadowed_vertex1_items,
      unshadowed_vertex2_items,
      unshadowed_vertex3_items,
    };
    unshadowed_pass->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });

    const auto unshadowed_index_items = triangle_items->CreateView("spark_unshadowed_index_items");
    {
      unshadowed_index_items->SetBind(View::BIND_INDEX_ARRAY);
    }

    const std::shared_ptr<View> ia_views[] = {
      unshadowed_index_items,
    };
    unshadowed_pass->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });

    unshadowed_pass->SetSubpassLayout(0, unshadowed_layout);
    unshadowed_pass->SetSubpassConfig(0, unshadowed_config);
    
    unshadowed_pass->SetEnabled(true);
  }

  std::shared_ptr<Layout> Spark::CreateShadowmapLayout(const std::shared_ptr<Device>& device)
  {
    const auto layout = device->CreateLayout("spark_shadowmap_layout");

    const auto shadow_camera_data = shadow_data->CreateView("spark_shadowmap_shadow_data");
    {
      shadow_camera_data->SetBind(View::BIND_CONSTANT_DATA);
      shadow_camera_data->SetByteOffset(0);
      shadow_camera_data->SetByteCount(sizeof(Frustum));
    }

    const std::shared_ptr<View> sb_views[] = {
      shadow_camera_data,
    };
    layout->UpdateSBViews({ sb_views, uint32_t(std::size(sb_views)) });

    return layout;
  }

  std::shared_ptr<Config> Spark::CreateShadowmapConfig(const std::shared_ptr<Device>& device)
  {
    const auto config = device->CreateConfig("spark_shadowmap_config");
    
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

    //const std::shared_ptr<View> va_views[] = {
    //  shadowmap_vertex0_items,
    //};
    //config->SetVAViews({ va_views, uint32_t(std::size(va_views)) });

    //const std::shared_ptr<View> ia_views[] = {
    //  shadowmap_index_items,
    //};
    //config->SetIAViews({ia_views, uint32_t(std::size(ia_views)) });

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



  std::shared_ptr<Layout> Spark::CreateShadowedLayout(const std::shared_ptr<Device>& device)
  {
    const auto layout = device->CreateLayout("spark_shadowed_layout");
    
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

    const auto shadowed_instance_items = instance_items->CreateView("spark_shadowed_instance_items");
    {
      shadowed_instance_items->SetBind(View::BIND_CONSTANT_DATA);
      shadowed_instance_items->SetByteOffset(0);
      shadowed_instance_items->SetByteCount(sizeof(Instance));
    }

    const std::shared_ptr<View> ue_views[] = {
      shadowed_instance_items
    };
    layout->UpdateSBViews({ ue_views, uint32_t(std::size(ue_views)) });

    const auto shadowed_texture0_items = texture0_items->CreateView("spark_shadowed_texture0_view");
    {
      shadowed_texture0_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto shadowed_texture1_items = texture1_items->CreateView("spark_shadowed_texture1_view");
    {
      shadowed_texture1_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto shadowed_texture2_items = texture2_items->CreateView("spark_shadowed_texture2_view");
    {
      shadowed_texture2_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto shadowed_texture3_items = texture3_items->CreateView("spark_shadowed_texture3_view");
    {
      shadowed_texture3_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto shadowed_shadow_map = shadow_map->CreateView("spark_shadowed_shadow_map");
    {
      shadowed_shadow_map->SetBind(View::BIND_SHADER_RESOURCE);
      shadowed_shadow_map->SetUsage(View::USAGE_CUBEMAP_LAYER);
    }

    const std::shared_ptr<View> ri_views[] = {
      shadowed_texture0_items,
      shadowed_texture1_items,
      shadowed_texture2_items,
      shadowed_texture3_items,
      shadowed_shadow_map,
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    return layout;
  }


  std::shared_ptr<Config> Spark::CreateShadowedConfig(const std::shared_ptr<Device>& device)
  {
    const auto config = device->CreateConfig("spark_shadowed_config");
    
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

  std::shared_ptr<Layout> Spark::CreateUnshadowedLayout(const std::shared_ptr<Device>& device)
  {
    const auto layout = device->CreateLayout("unshadowed_layout");

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


    const auto unshadowed_instance_items = instance_items->CreateView("spark_unshadowed_instance_items");
    {
      unshadowed_instance_items->SetBind(View::BIND_CONSTANT_DATA);
      unshadowed_instance_items->SetByteOffset(0);
      unshadowed_instance_items->SetByteCount(sizeof(Instance));
    }

    const std::shared_ptr<View> ue_views[] = {
      unshadowed_instance_items
    };
    layout->UpdateSBViews({ ue_views, uint32_t(std::size(ue_views)) });


    const auto unshadowed_texture0_items = texture0_items->CreateView("spark_unshadowed_texture0_view");
    {
      unshadowed_texture0_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_texture1_items = texture1_items->CreateView("spark_unshadowed_texture1_view");
    {
      unshadowed_texture1_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_texture2_items = texture2_items->CreateView("spark_unshadowed_texture2_view");
    {
      unshadowed_texture2_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_texture3_items = texture3_items->CreateView("spark_unshadowed_texture3_view");
    {
      unshadowed_texture3_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_texture4_items = texture4_items->CreateView("spark_unshadowed_texture4_view");
    {
      unshadowed_texture4_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto unshadowed_environment_item = environment_item->CreateView("spark_unshadowed_environment_view");
    {
      unshadowed_environment_item->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const std::shared_ptr<View> ri_views[] = {
      unshadowed_texture0_items,
      unshadowed_texture1_items,
      unshadowed_texture2_items,
      unshadowed_texture3_items,
      unshadowed_texture4_items,
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    return layout;
  }

  std::shared_ptr<Config> Spark::CreateUnshadowedConfig(const std::shared_ptr<Device>& device)
  {
    const auto config = device->CreateConfig("spark_unshadowed_config");
    
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

  std::shared_ptr<Layout> Spark::CreateEnvironmentLayout(const std::shared_ptr<Device>& device)
  {
    const auto layout = device->CreateLayout("spark_environment_layout");
    
    const Layout::Sampler samplers[] = {
    { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
    };
    layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });


    const auto environment_screen_data = screen_data->CreateView("spark_environment_screen_data");
    {
      environment_screen_data->SetBind(View::BIND_CONSTANT_DATA);
    }

    const auto environment_camera_data = camera_data->CreateView("spark_environment_camera_data");
    {
      environment_camera_data->SetBind(View::BIND_CONSTANT_DATA);
    }

    const std::shared_ptr<View> ub_views[] = {
      environment_screen_data,
      environment_camera_data,
    };
    layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });


    const auto environment_item_view = environment_item->CreateView("spark_environment_item_view");
    {
      environment_item_view->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const std::shared_ptr<View> ri_views[] = {
      environment_item_view,
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    return layout;
  }

  std::shared_ptr<Config> Spark::CreateEnvironmentConfig(const std::shared_ptr<Device>& device)
  {
    const auto config = device->CreateConfig("spark_environment_config");
    
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

  std::shared_ptr<Layout> Spark::CreatePresentLayout(const std::shared_ptr<Device>& device)
  {
    const auto layout = device->CreateLayout("spark_present_layout");

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

    auto present_render_target = render_target->CreateView("spark_present_render_target");
    {
      present_render_target->SetBind(View::BIND_SHADER_RESOURCE);
      present_render_target->SetLayerOffset(0);
      present_render_target->SetLayerCount(uint32_t(-1));
    }

    const std::shared_ptr<View> ri_views[] = {
      present_render_target,
    };
    layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

    const std::shared_ptr<View> wi_views[] = {
      output_view,
    };
    layout->UpdateWIViews({ wi_views, uint32_t(std::size(wi_views)) });

    return layout;
  }

  std::shared_ptr<Config> Spark::CreatePresentConfig(const std::shared_ptr<Device>& device)
  {
    const auto config = device->CreateConfig("spark_present_config");

    std::fstream shader_fs;
    shader_fs.open("./asset/shaders/spark_present.hlsl", std::fstream::in);
    std::stringstream shader_ss;
    shader_ss << shader_fs.rdbuf();

    config->SetSource(shader_ss.str());
    config->SetCompilation(Config::COMPILATION_CS);

    return config;
  }

  void Spark::CreateShadowed(const std::shared_ptr<Device>& device)
  {
    shadowed_config = CreateShadowedConfig(device);
    shadowed_layout = CreateShadowedLayout(device);

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    const auto shadowed_render_target = render_target->CreateView("spark_shadowed_render_target");
    {
      shadowed_render_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const auto shadowed_depth_stencil = depth_stencil->CreateView("spark_shadowed_depth_stencil");
    {
      shadowed_depth_stencil->SetBind(View::BIND_DEPTH_STENCIL);
    }


    shadowed_pass = device->CreatePass("spark_shadowed");
    shadowed_pass->SetType(Pass::TYPE_GRAPHIC);

    const std::shared_ptr<View> rt_views[] = {
      shadowed_render_target,
    };
    shadowed_pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

    const std::shared_ptr<View> ds_views[] = {
      shadowed_depth_stencil,
    };
    shadowed_pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

    const Pass::RTValue rt_values[] = {
      std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
    };
    shadowed_pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

    const Pass::DSValue ds_values[] = {
      { 1.0f, std::nullopt },
    };
    shadowed_pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

    shadowed_pass->SetSubpassCount(1);

    const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    std::vector<Pass::Command> commands(instance_count);
    for (uint32_t j = 0; j < instance_count; ++j)
    {
      const auto argument_view = raster_arguments->CreateView("spark_shadowed_raster_argument_" + std::to_string(j) + "_" + std::to_string(j));
      argument_view->SetBind(View::BIND_COMMAND_INDIRECT);
      argument_view->SetByteOffset(j * uint32_t(sizeof(Pass::Argument)));
      argument_view->SetByteCount(uint32_t(sizeof(Pass::Argument)));

      commands[j].view = argument_view;
      commands[j].offsets = { j * uint32_t(sizeof(Frustum)) };
    }

    shadowed_pass->UpdateSubpassCommands(0, { commands.data(), uint32_t(commands.size()) });

    const auto shadowed_vertex0_items = vertex0_items->CreateView("spark_shadowed_vertex0_items");
    {
      shadowed_vertex0_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto shadowed_vertex1_items = vertex1_items->CreateView("spark_shadowed_vertex1_items");
    {
      shadowed_vertex1_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto shadowed_vertex2_items = vertex2_items->CreateView("spark_shadowed_vertex2_items");
    {
      shadowed_vertex2_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto shadowed_vertex3_items = vertex3_items->CreateView("spark_shadowed_vertex3_items");
    {
      shadowed_vertex3_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const std::shared_ptr<View> va_views[] = {
      shadowed_vertex0_items,
      shadowed_vertex1_items,
      shadowed_vertex2_items,
      shadowed_vertex3_items,
    };
    shadowed_pass->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });

    const auto shadowed_index_items = triangle_items->CreateView("spark_shadowed_index_items");
    {
      shadowed_index_items->SetBind(View::BIND_INDEX_ARRAY);
    }

    const std::shared_ptr<View> ia_views[] = {
      shadowed_index_items,
    };
    shadowed_pass->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });

    shadowed_pass->SetSubpassLayout(0, shadowed_layout);
    shadowed_pass->SetSubpassConfig(0, shadowed_config);

    shadowed_pass->SetEnabled(true);
    
  }

  void Spark::CreateEnvironment(const std::shared_ptr<Device>& device)
  {
    environment_config = CreateEnvironmentConfig(device);
    environment_layout = CreateEnvironmentLayout(device);

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    const auto environment_render_target = render_target->CreateView("spark_environment_render_target");
    {
      environment_render_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const auto environment_depth_stencil = depth_stencil->CreateView("spark_environment_depth_stencil");
    {
      environment_depth_stencil->SetBind(View::BIND_DEPTH_STENCIL);
    }

    environment_pass = device->CreatePass("spark_environment");
    {
      environment_pass->SetType(Pass::TYPE_GRAPHIC);
      environment_pass->SetSubpassCount(1);
      environment_pass->SetEnabled(true);

      const std::shared_ptr<View> rt_views[] = {
        environment_render_target,
      };
      environment_pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

      const std::shared_ptr<View> ds_views[] = {
        environment_depth_stencil,
      };
      environment_pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

      const Pass::RTValue rt_values[] = {
        std::nullopt,
      };
      environment_pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

      const Pass::DSValue ds_values[] = {
        { std::nullopt, std::nullopt },
      };
      environment_pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

      Pass::Command commands[] = {
        {nullptr, {6, 1, 0, 0, 0, 0, 0, 0}},
      };

      environment_pass->UpdateSubpassCommands(0, { commands, uint32_t(std::size(commands)) });

      const auto environment_vtx_items = environment_vtx_data->CreateView("spark_environment_vtx_items");
      {
        environment_vtx_items->SetBind(View::BIND_VERTEX_ARRAY);
      }

      const std::shared_ptr<View> va_views[] = {
        environment_vtx_items,
      };
      environment_pass->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });


      const auto environment_idx_items = environment_idx_data->CreateView("spark_environment_idx_items");
      {
        environment_idx_items->SetBind(View::BIND_INDEX_ARRAY);
      }

      const std::shared_ptr<View> ia_views[] = {
        environment_idx_items,
      };
      environment_pass->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });

      environment_pass->SetSubpassLayout(0, environment_layout);
      environment_pass->SetSubpassConfig(0, environment_config);
    }
  }

  void Spark::CreatePresent(const std::shared_ptr<Device>& device)
  {
    present_config = CreatePresentConfig(device);
    present_layout = CreatePresentLayout(device);

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    //auto present_frame_output = frame_output->CreateView("present_frame_output");
    //{
    //  present_frame_output->SetBind(View::BIND_UNORDERED_ACCESS);
    //  present_frame_output->SetLayerOffset(0);
    //  present_frame_output->SetLayerCount(uint32_t(-1));
    //}



    auto present_compute_arguments = compute_arguments->CreateView("spark_present_compute_arguments");
    {
      present_compute_arguments->SetBind(View::BIND_COMMAND_INDIRECT);
      present_compute_arguments->SetByteOffset(0);
      present_compute_arguments->SetByteCount(uint32_t(-1));
    }





    present_pass = device->CreatePass("spark_present_screen");
    {
      present_pass->SetType(Pass::TYPE_COMPUTE);
      present_pass->SetSubpassCount(1);
      present_pass->SetEnabled(true);

      Pass::Command commands[] = {
        {present_compute_arguments},
      };
      present_pass->UpdateSubpassCommands(0, { commands, uint32_t(std::size(commands)) });

      present_pass->SetSubpassLayout(0, present_layout);
      present_pass->SetSubpassConfig(0, present_config);
    }
  }


  void Spark::Initialize()
  {
    const auto core = &this->GetCore();

    const auto prop_camera = root_property->GetObjectItem("camera");
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

    const auto prop_scene = root_property->GetObjectItem("scene");
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

      prop_textures4 = prop_scene->GetObjectItem("textures4");
    }



    const auto device = core->AccessDevice();

    CreateResources(device);
    //CreateAttachments(device);
    //CreateConstants(device);
    //CreateGeometries(device);
    //CreateTextures(device);
    //CreateArguments(device);

    //CreatePasses(device);
    CreateShadowmap(device);
    CreateUnshadowed(device);
    CreateShadowed(device);
    CreateEnvironment(device);
    CreatePresent(device);

    
    render_target->Initialize();
    depth_stencil->Initialize();
    shadow_map->Initialize();

    screen_data->Initialize();
    camera_data->Initialize();
    shadow_data->Initialize();
    
    instance_items->Initialize();
    triangle_items->Initialize();
    vertex0_items->Initialize();
    vertex1_items->Initialize();
    vertex2_items->Initialize();
    vertex3_items->Initialize();
 
    texture0_items->Initialize();
    texture1_items->Initialize();
    texture2_items->Initialize();
    texture3_items->Initialize();
   
    texture4_items->Initialize();

    environment_vtx_data->Initialize();
    environment_idx_data->Initialize();
    environment_item->Initialize();

    
    raster_arguments->Initialize();
    compute_arguments->Initialize();

    shadowmap_layout->Initialize();
    shadowmap_config->Initialize();
    for (auto& pass : shadowmap_passes) pass->Initialize();
    
    unshadowed_layout->Initialize();
    unshadowed_config->Initialize();
    unshadowed_pass->Initialize();
    
    shadowed_layout->Initialize();
    shadowed_config->Initialize();
    shadowed_pass->Initialize();
    
    environment_layout->Initialize();
    environment_config->Initialize();
    environment_pass->Initialize();
    
    present_layout->Initialize();
    present_config->Initialize();
    present_pass->Initialize();
  }


  void Spark::Use()
  {
    switch (shadows)
    {
    case NO_SHADOWS:
    {
      for (auto& pass : shadowmap_passes)
      {
        pass->SetEnabled(false);
      }
      unshadowed_pass->SetEnabled(true);
      shadowed_pass->SetEnabled(false);
      environment_pass->SetEnabled(true);
      present_pass->SetEnabled(true);
      break;
    }
    case POINT_SHADOWS:
    {
      for (auto& pass : shadowmap_passes)
      {
        pass->SetEnabled(true);
      }
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(true);
      environment_pass->SetEnabled(true);
      present_pass->SetEnabled(true);
      break;
    }
    default:
    {
      for (auto& pass : shadowmap_passes)
      {
        pass->SetEnabled(false);
      }
      unshadowed_pass->SetEnabled(false);
      shadowed_pass->SetEnabled(false);
      environment_pass->SetEnabled(false);
      present_pass->SetEnabled(false);
      break;
    }
    }


    {
      auto graphic_arg = reinterpret_cast<Pass::Argument*>(raster_arguments->Map());

      const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
      for (uint32_t i = 0; i < instance_count; ++i)
      {
        graphic_arg[i].idx_count = instance_data[i].prim_count * 3;
        graphic_arg[i].ins_count = 1;
        graphic_arg[i].idx_offset = instance_data[i].prim_offset * 3;
        graphic_arg[i].vtx_offset = instance_data[i].vert_offset * 1;
        graphic_arg[i].ins_offset = 0;
      }
      raster_arguments->Unmap();

      //for (uint32_t i = 0; i < 6; ++i)
      //{
      //  shadowmap_passes[i]->SetSubpassCount(1);
      //  for (uint32_t j = 0; j < instance_count; ++j)
      //  {
      //    const uint32_t ue_offsets[] = {
      //      i * uint32_t(sizeof(Frustum)),
      //    };
      //    shadowmap_passes[i]->UpdateSubpassSBOffsets(j, { ue_offsets, uint32_t(std::size(ue_offsets)) });

      //    Pass::Graphic shadowmap_tasks[] = {
      //      {instance_data[i].prim_count * 3, 1, instance_data[i].prim_offset * 3, instance_data[i].vert_offset * 1, 0},
      //    };
      //    shadowmap_passes[i]->UpdateSubpassGraphicTasks(j, { shadowmap_tasks, uint32_t(std::size(shadowmap_tasks)) });

      //    shadowmap_passes[i]->SetSubpassLayout(j, shadowmap_layout);
      //    shadowmap_passes[i]->SetSubpassConfig(j, shadowmap_config);
      //  }
      //}
    }

    //auto environment_arg = reinterpret_cast<Pass::Graphic*>(environment_arguments->Map());
    //for (uint32_t i = 0; i < 1; ++i)
    //{
    //  environment_arg[0].idx_count = 6;
    //  environment_arg[0].ins_count = 1;
    //  environment_arg[0].idx_offset = 0;
    //  environment_arg[0].vtx_offset = 0;
    //  environment_arg[0].ins_offset = 0;
    //}
    //environment_arguments->Unmap();

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
  }

  Spark::Spark(Core& core) 
    : Usable("raygene")
    , core(core)
  {
  }

  Spark::~Spark()
  {
  }
}