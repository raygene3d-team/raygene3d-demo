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
  void Spark::InitializeResources()
  {
    const auto core = &this->GetCore();
    const auto device = core->AccessDevice();

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    {
      shadow_map = device->CreateResource("shadow_map");
      {
        shadow_map->SetType(Resource::TYPE_IMAGE2D);
        shadow_map->SetExtentX(shadow_map_size);
        shadow_map->SetExtentY(shadow_map_size);
        shadow_map->SetLayers(6);
        shadow_map->SetMipmaps(1);
        shadow_map->SetFormat(FORMAT_D16_UNORM);
        shadow_map->SetHint(Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE));
      }

      render_target = device->CreateResource("render_target");
      {
        render_target->SetType(Resource::TYPE_IMAGE2D);
        render_target->SetExtentX(extent_x);
        render_target->SetExtentY(extent_y);
        render_target->SetLayers(1);
        render_target->SetMipmaps(1);
        render_target->SetFormat(FORMAT_R11G11B10_FLOAT);
      }

      depth_stencil = device->CreateResource("depth_stencil");
      {
        depth_stencil->SetType(Resource::TYPE_IMAGE2D);
        depth_stencil->SetExtentX(extent_x);
        depth_stencil->SetExtentY(extent_y);
        depth_stencil->SetLayers(1);
        depth_stencil->SetMipmaps(1);
        depth_stencil->SetFormat(FORMAT_D32_FLOAT);
      }

      screen_data = device->CreateResource("screen_data");
      {
        screen_data->SetType(Resource::TYPE_BUFFER);
        screen_data->SetStride(uint32_t(sizeof(Screen)));
        screen_data->SetCount(1);
        screen_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      camera_data = device->CreateResource("camera_data");
      {
        camera_data->SetType(Resource::TYPE_BUFFER);
        camera_data->SetStride(uint32_t(sizeof(Frustum)));
        camera_data->SetCount(1);
        camera_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      shadow_data = device->CreateResource("shadow_data");
      {
        shadow_data->SetType(Resource::TYPE_BUFFER);
        shadow_data->SetStride(uint32_t(sizeof(Frustum)));
        shadow_data->SetCount(6);
        shadow_data->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      frame_output = device->ShareResource("screen_backbuffer");
      {
        //frame_output->SetType(Resource::TYPE_IMAGE2D);
        //frame_output->SetExtentX(extent_x);
        //frame_output->SetExtentY(extent_y);
        //frame_output->SetLayers(1);
        //frame_output->SetMipmaps(1);
        //frame_output->SetFormat(FORMAT_B8G8R8A8_UNORM);
      }

      instance_items = device->CreateResource("instance_items");
      {
        const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

        instance_items->SetType(Resource::TYPE_BUFFER);
        instance_items->SetStride(uint32_t(sizeof(Instance)));
        instance_items->SetCount(count);
        instance_items->SetInteropCount(1);
        instance_items->SetInteropItem(0, prop_instances->GetRawBytes(0));
      }

      triangle_items = device->CreateResource("triangle_items");
      {
        const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);

        triangle_items->SetType(Resource::TYPE_BUFFER);
        triangle_items->SetStride(uint32_t(sizeof(Triangle)));
        triangle_items->SetCount(count);
        triangle_items->SetInteropCount(1);
        triangle_items->SetInteropItem(0, prop_triangles->GetRawBytes(0));
      }

      vertex_items = device->CreateResource("vertex_items");
      {
        const auto [data, count] = prop_vertices->GetTypedBytes<Vertex>(0);

        vertex_items->SetType(Resource::TYPE_BUFFER);
        vertex_items->SetStride(uint32_t(sizeof(Vertex)));
        vertex_items->SetCount(count);
        vertex_items->SetInteropCount(1);
        vertex_items->SetInteropItem(0, prop_vertices->GetRawBytes(0));
      }

      raster_arguments = device->CreateResource("raster_arguments");
      {
        const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

        raster_arguments->SetType(Resource::TYPE_BUFFER);
        raster_arguments->SetStride(uint32_t(sizeof(Pass::Graphic)));
        raster_arguments->SetCount(uint32_t(count));
        raster_arguments->SetHint(Resource::HINT_DYNAMIC_BUFFER);
      }

      texture0_items = device->CreateResource("texture0_items");
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

      texture1_items = device->CreateResource("texture1_items");
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

      texture2_items = device->CreateResource("texture2_items");
      {
        const auto layers = prop_textures2->GetArraySize();
        const auto format = FORMAT_R8G8B8A8_SRGB;
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

      texture3_items = device->CreateResource("texture3_items");
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

      texture4_items = device->CreateResource("texture4_items");
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

      environment_item = device->CreateResource("environment_item");
      {
        const auto textures = property->GetObjectItem("environment");
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
    }
  }


  void Spark::InitializeShadow()
  {
    const auto core = &this->GetCore();
    const auto device = core->AccessDevice();

    const auto extent_x = shadow_map_size;
    const auto extent_y = shadow_map_size;

    const auto shadow_instance_items = instance_items->CreateView("shadow_instance_items");
    {
      shadow_instance_items->SetBind(View::BIND_CONSTANT_DATA);
      shadow_instance_items->SetByteOffset(0);
      shadow_instance_items->SetByteCount(sizeof(Instance));
    }

    const auto shadow_vertex_items = vertex_items->CreateView("shadow_vertex_items");
    {
      shadow_vertex_items->SetBind(View::BIND_VERTEX_ARRAY);
      shadow_vertex_items->SetByteOffset(0);
      shadow_vertex_items->SetByteCount(uint32_t(-1));
    }

    const auto shadow_index_items = triangle_items->CreateView("shadow_index_items");
    {
      shadow_index_items->SetBind(View::BIND_INDEX_ARRAY);
      shadow_index_items->SetByteOffset(0);
      shadow_index_items->SetByteCount(uint32_t(-1));
    }

    shadow_shader = device->CreateShader("shadow_shader");
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_shadow.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      const std::string source = shader_ss.str();
      shadow_shader->SetSource(source);

      shadow_shader->SetCompilation(Config::Compilation(Config::COMPILATION_VS));
      shadow_shader->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
      shadow_shader->SetIndexer(Config::INDEXER_32_BIT);

      const Config::Attribute attributes[] = {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false },
      };
      shadow_shader->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

      shadow_shader->SetFillMode(Config::FILL_SOLID);
      shadow_shader->SetCullMode(Config::CULL_FRONT);
      shadow_shader->SetClipEnabled(false);

      const Config::Blend blends[] = {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
      };
      shadow_shader->UpdateBlends({ blends, uint32_t(std::size(blends)) });
      shadow_shader->SetATCEnabled(false);

      shadow_shader->SetDepthEnabled(true);
      shadow_shader->SetDepthWrite(true);
      shadow_shader->SetDepthComparison(Config::COMPARISON_LESS);

      const Config::Viewport viewports[] = {
        { 0.0f, 0.0f, float(extent_x), float(extent_y), 0.0f, 1.0f },
      };
      shadow_shader->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
    }

    const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    std::vector<std::shared_ptr<View>> arguments_views(instance_count);
    for (uint32_t i = 0; i < uint32_t(arguments_views.size()); ++i)
    {
      const auto stride = uint32_t(sizeof(Pass::Graphic));

      auto arguments_view = raster_arguments->CreateView("shadow_arguments_" + std::to_string(i));
      arguments_view->SetBind(View::BIND_COMMAND_INDIRECT);
      arguments_view->SetByteOffset(i * stride);
      arguments_view->SetByteCount(stride);

      arguments_views[i] = arguments_view;
    }


    shadow_layout = device->CreateLayout("shadow_layout");
    {
      const auto shadow_camera_data = shadow_data->CreateView("shadow_data");
      {
        shadow_camera_data->SetBind(View::BIND_CONSTANT_DATA);
        shadow_camera_data->SetByteOffset(0);
        shadow_camera_data->SetByteCount(sizeof(Frustum));
      }

      const std::shared_ptr<View> sb_views[] = {
        shadow_camera_data,
      };
      shadow_layout->UpdateSBViews({ sb_views, uint32_t(std::size(sb_views)) });
    }

    for (uint32_t i = 0; i < 6; ++i)
    {
      const auto shadow_depth_stencil = shadow_map->CreateView("shadow_depth_stencil_" + std::to_string(i));
      {
        shadow_depth_stencil->SetBind(View::BIND_DEPTH_STENCIL);
        shadow_depth_stencil->SetLayerOffset(i);
        shadow_depth_stencil->SetLayerCount(1);
      }

      auto& shadow_pass = shadow_passes[i];

      shadow_pass->SetType(Pass::TYPE_GRAPHIC);
      {
        shadow_pass->SetEnabled(true);

        const std::shared_ptr<View> ds_views[] = {
          shadow_depth_stencil,
        };
        shadow_pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

        const Pass::DSValue ds_values[] = {
          { 1.0f, std::nullopt },
        };
        shadow_pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

        shadow_pass->SetSubpassCount(arguments_views.size());
        for (uint32_t j = 0; j < arguments_views.size(); ++j)
        {
          const std::shared_ptr<View> va_views[] = {
            shadow_vertex_items,
          };
          shadow_pass->UpdateSubpassVAViews(j, { va_views, uint32_t(std::size(va_views)) });

          const std::shared_ptr<View> ia_views[] = {
            shadow_index_items,
          };
          shadow_pass->UpdateSubpassIAViews(j, { ia_views, uint32_t(std::size(ia_views)) });

          const uint32_t ue_offsets[] = {
            i * uint32_t(sizeof(Frustum)),
          };
          shadow_pass->UpdateSubpassSBOffsets(j, { ue_offsets, uint32_t(std::size(ue_offsets)) });

          const std::shared_ptr<View> aa_views[] = {
            arguments_views[j]
          };
          shadow_pass->UpdateSubpassAAViews(j, { aa_views, uint32_t(std::size(aa_views)) });

          shadow_pass->SetSubpassLayout(j, shadow_layout);
          shadow_pass->SetSubpassShader(j, shadow_shader);

        }
      }
    }
  }

  void Spark::InitializeSimple()
  {
    const auto core = &this->GetCore();
    const auto device = core->AccessDevice();

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    const auto raster_screen_data = screen_data->CreateView("raster_screen_data");
    {
      raster_screen_data->SetBind(View::BIND_CONSTANT_DATA);
      raster_screen_data->SetByteOffset(0);
      raster_screen_data->SetByteCount(uint32_t(-1));
    }

    const auto raster_camera_data = camera_data->CreateView("raster_camera_data");
    {
      raster_camera_data->SetBind(View::BIND_CONSTANT_DATA);
      raster_camera_data->SetByteOffset(0);
      raster_camera_data->SetByteCount(uint32_t(-1));
    }

    const auto raster_shadow_data = shadow_data->CreateView("raster_shadow_data");
    {
      raster_shadow_data->SetBind(View::BIND_CONSTANT_DATA);
      raster_shadow_data->SetByteOffset(0);
      raster_shadow_data->SetByteCount(sizeof(Frustum));
    }

    const auto raster_intstance_items = instance_items->CreateView("raster_instance_items");
    {
      raster_intstance_items->SetBind(View::BIND_CONSTANT_DATA);
      raster_intstance_items->SetByteOffset(0);
      raster_intstance_items->SetByteCount(sizeof(Instance));
    }

    const auto raster_texture0_items = texture0_items->CreateView("raster_texture0_view");
    {
      raster_texture0_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_texture1_items = texture1_items->CreateView("raster_texture1_view");
    {
      raster_texture1_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_texture2_items = texture2_items->CreateView("raster_texture2_view");
    {
      raster_texture2_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_texture3_items = texture3_items->CreateView("raster_texture3_view");
    {
      raster_texture3_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_texture4_items = texture4_items->CreateView("raster_texture4_view");
    {
      raster_texture4_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_environment_item = environment_item->CreateView("raster_environment_view");
    {
      raster_environment_item->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_render_target = render_target->CreateView("raster_render_target");
    {
      raster_render_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const auto raster_depth_stencil = depth_stencil->CreateView("raster_depth_stencil");
    {
      raster_depth_stencil->SetBind(View::BIND_DEPTH_STENCIL);
    }

    const auto raster_vertex_items = vertex_items->CreateView("raster_vertex_items");
    {
      raster_vertex_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto raster_index_items = triangle_items->CreateView("raster_index_items");
    {
      raster_index_items->SetBind(View::BIND_INDEX_ARRAY);
    }

    no_shadow_raster_shader = device->CreateShader("no_shadow_raster_shader");
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_simple.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      const std::string source = shader_ss.str();
      no_shadow_raster_shader->SetSource(source);
      no_shadow_raster_shader->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));
      no_shadow_raster_shader->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
      no_shadow_raster_shader->SetIndexer(Config::INDEXER_32_BIT);
      no_shadow_raster_shader->SetDefineItem("TEST", "1");

      const Config::Attribute attributes[] = {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 12, 64, FORMAT_R8G8B8A8_UINT, false },
        { 0, 16, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 28, 64, FORMAT_R32_UINT, false },
        { 0, 32, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 44, 64, FORMAT_R32_FLOAT, false },
        { 0, 48, 64, FORMAT_R32G32_FLOAT, false },
        { 0, 56, 64, FORMAT_R32G32_FLOAT, false },
      };
      no_shadow_raster_shader->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

      no_shadow_raster_shader->SetFillMode(Config::FILL_SOLID);
      no_shadow_raster_shader->SetCullMode(Config::CULL_BACK);
      no_shadow_raster_shader->SetClipEnabled(false);

      const Config::Blend blends[] = {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
      };
      no_shadow_raster_shader->UpdateBlends({ blends, uint32_t(std::size(blends)) });
      no_shadow_raster_shader->SetATCEnabled(false);

      no_shadow_raster_shader->SetDepthEnabled(true);
      no_shadow_raster_shader->SetDepthWrite(true);
      no_shadow_raster_shader->SetDepthComparison(Config::COMPARISON_LESS);

      const Config::Viewport viewports[] = {
        { 0.0f, 0.0f, float(extent_x), float(extent_y), 0.0f, 1.0f },
      };
      no_shadow_raster_shader->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
    }

    no_shadow_raster_layout = device->CreateLayout("no_shadow_raster_layout");
    {
      const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      };
      no_shadow_raster_layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });

      const std::shared_ptr<View> ub_views[] = {
        raster_screen_data,
        raster_camera_data,
        raster_shadow_data,
      };
      no_shadow_raster_layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

      const std::shared_ptr<View> ue_views[] = {
        raster_intstance_items
      };
      no_shadow_raster_layout->UpdateSBViews({ ue_views, uint32_t(std::size(ue_views)) });

      const std::shared_ptr<View> ri_views[] = {
        raster_texture0_items,
        raster_texture1_items,
        raster_texture2_items,
        raster_texture3_items,
        raster_texture4_items,
      };
      no_shadow_raster_layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });
    }

    const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    std::vector<std::shared_ptr<View>> arguments_views(instance_count);
    for (uint32_t i = 0; i < uint32_t(arguments_views.size()); ++i)
    {
      const auto stride = uint32_t(sizeof(Pass::Graphic));

      auto arguments_view = raster_arguments->CreateView("shadow_map_raster_arguments_" + std::to_string(i));
      arguments_view->SetBind(View::BIND_COMMAND_INDIRECT);
      arguments_view->SetByteOffset(i * stride);
      arguments_view->SetByteCount(stride);

      arguments_views[i] = arguments_view;
    }

    no_shadow_raster_pass->SetType(Pass::TYPE_GRAPHIC);
    {
      no_shadow_raster_pass->SetEnabled(true);

      const std::shared_ptr<View> rt_views[] = {
        raster_render_target,
      };
      no_shadow_raster_pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

      const std::shared_ptr<View> ds_views[] = {
        raster_depth_stencil,
      };
      no_shadow_raster_pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

      const Pass::RTValue rt_values[] = {
        std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
      };
      no_shadow_raster_pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

      const Pass::DSValue ds_values[] = {
        { 1.0f, std::nullopt },
      };
      no_shadow_raster_pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

      no_shadow_raster_pass->SetSubpassCount(arguments_views.size());
      for (uint32_t i = 0; i < arguments_views.size(); ++i)
      {
        const std::shared_ptr<View> va_views[] = {
          raster_vertex_items,
        };
        no_shadow_raster_pass->UpdateSubpassVAViews(i, { va_views, uint32_t(std::size(va_views)) });

        const std::shared_ptr<View> ia_views[] = {
          raster_index_items,
        };
        no_shadow_raster_pass->UpdateSubpassIAViews(i, { ia_views, uint32_t(std::size(ia_views)) });

        const uint32_t ue_offsets[] = {
          i * uint32_t(sizeof(Frustum)),
        };
        no_shadow_raster_pass->UpdateSubpassSBOffsets(i, { ue_offsets, uint32_t(std::size(ue_offsets)) });

        const std::shared_ptr<View> aa_views[] = {
          arguments_views[i]
        };
        no_shadow_raster_pass->UpdateSubpassAAViews(i, { aa_views, uint32_t(std::size(aa_views)) });

        no_shadow_raster_pass->SetSubpassLayout(i, no_shadow_raster_layout);
        no_shadow_raster_pass->SetSubpassShader(i, no_shadow_raster_shader);

      }
    }
  }

  void Spark::InitializeAdvanced()
  {
    const auto core = &this->GetCore();
    const auto device = core->AccessDevice();

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    const auto raster_screen_data = screen_data->CreateView("raster_screen_data");
    {
      raster_screen_data->SetBind(View::BIND_CONSTANT_DATA);
      raster_screen_data->SetByteOffset(0);
      raster_screen_data->SetByteCount(uint32_t(-1));
    }

    const auto raster_camera_data = camera_data->CreateView("raster_camera_data");
    {
      raster_camera_data->SetBind(View::BIND_CONSTANT_DATA);
      raster_camera_data->SetByteOffset(0);
      raster_camera_data->SetByteCount(uint32_t(-1));
    }

    const auto raster_shadow_data = shadow_data->CreateView("raster_shadow_data");
    {
      raster_shadow_data->SetBind(View::BIND_CONSTANT_DATA);
      raster_shadow_data->SetByteOffset(0);
      raster_shadow_data->SetByteCount(sizeof(Frustum));
    }

    const auto raster_intstance_items = instance_items->CreateView("raster_instance_items");
    {
      raster_intstance_items->SetBind(View::BIND_CONSTANT_DATA);
      raster_intstance_items->SetByteOffset(0);
      raster_intstance_items->SetByteCount(sizeof(Instance));
    }

    const auto raster_texture0_items = texture0_items->CreateView("raster_texture0_view");
    {
      raster_texture0_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_texture1_items = texture1_items->CreateView("raster_texture1_view");
    {
      raster_texture1_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_texture2_items = texture2_items->CreateView("raster_texture2_view");
    {
      raster_texture2_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_texture3_items = texture3_items->CreateView("raster_texture3_view");
    {
      raster_texture3_items->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto raster_shadow_map = shadow_map->CreateView("raster_shadow_map");
    {
      raster_shadow_map->SetBind(View::BIND_SHADER_RESOURCE);
      raster_shadow_map->SetUsage(View::USAGE_CUBEMAP_LAYER);
    }

    const auto raster_render_target = render_target->CreateView("raster_render_target");
    {
      raster_render_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const auto raster_depth_stencil = depth_stencil->CreateView("raster_depth_stencil");
    {
      raster_depth_stencil->SetBind(View::BIND_DEPTH_STENCIL);
    }

    const auto raster_vertex_items = vertex_items->CreateView("raster_vertex_items");
    {
      raster_vertex_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto raster_index_items = triangle_items->CreateView("raster_index_items");
    {
      raster_index_items->SetBind(View::BIND_INDEX_ARRAY);
    }


    shadow_map_raster_shader = device->CreateShader("shadow_map_raster_shader");
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_advanced.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      const std::string source = shader_ss.str();
      shadow_map_raster_shader->SetSource(source);
      shadow_map_raster_shader->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));
      shadow_map_raster_shader->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
      shadow_map_raster_shader->SetIndexer(Config::INDEXER_32_BIT);

      const Config::Attribute attributes[] = {
        { 0,  0, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 12, 64, FORMAT_R8G8B8A8_UINT, false },
        { 0, 16, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 28, 64, FORMAT_R32_UINT, false },
        { 0, 32, 64, FORMAT_R32G32B32_FLOAT, false },
        { 0, 44, 64, FORMAT_R32_FLOAT, false },
        { 0, 48, 64, FORMAT_R32G32_FLOAT, false },
        { 0, 56, 64, FORMAT_R32G32_FLOAT, false },
      };
      shadow_map_raster_shader->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

      shadow_map_raster_shader->SetFillMode(Config::FILL_SOLID);
      shadow_map_raster_shader->SetCullMode(Config::CULL_BACK);
      shadow_map_raster_shader->SetClipEnabled(false);

      const Config::Blend blends[] = {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
      };
      shadow_map_raster_shader->UpdateBlends({ blends, uint32_t(std::size(blends)) });
      shadow_map_raster_shader->SetATCEnabled(false);

      shadow_map_raster_shader->SetDepthEnabled(true);
      shadow_map_raster_shader->SetDepthWrite(true);
      shadow_map_raster_shader->SetDepthComparison(Config::COMPARISON_LESS);

      const Config::Viewport viewports[] = {
        { 0.0f, 0.0f, float(extent_x), float(extent_y), 0.0f, 1.0f },
      };
      shadow_map_raster_shader->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
    }

    shadow_map_raster_layout = device->CreateLayout("shadow_map_raster_layout");
    {
      const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      { Layout::Sampler::FILTERING_NEAREST, 1, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_ALWAYS, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f },
      };
      shadow_map_raster_layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });

      const std::shared_ptr<View> ub_views[] = {
        raster_screen_data,
        raster_camera_data,
        raster_shadow_data,
      };
      shadow_map_raster_layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

      const std::shared_ptr<View> ue_views[] = {
        raster_intstance_items
      };
      shadow_map_raster_layout->UpdateSBViews({ ue_views, uint32_t(std::size(ue_views)) });

      const std::shared_ptr<View> ri_views[] = {
        raster_texture0_items,
        raster_texture1_items,
        raster_texture2_items,
        raster_texture3_items,
        raster_shadow_map,
      };
      shadow_map_raster_layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });
    }

    const auto [instance_data, instance_count] = prop_instances->GetTypedBytes<Instance>(0);
    std::vector<std::shared_ptr<View>> arguments_views(instance_count);
    for (uint32_t i = 0; i < uint32_t(arguments_views.size()); ++i)
    {
      const auto stride = uint32_t(sizeof(Pass::Graphic));

      auto arguments_view = raster_arguments->CreateView("shadow_map_raster_arguments_" + std::to_string(i));
      arguments_view->SetBind(View::BIND_COMMAND_INDIRECT);
      arguments_view->SetByteOffset(i * stride);
      arguments_view->SetByteCount(stride);

      arguments_views[i] = arguments_view;
    }

    shadow_map_raster_pass->SetType(Pass::TYPE_GRAPHIC);
    {
      shadow_map_raster_pass->SetEnabled(true);

      const std::shared_ptr<View> rt_views[] = {
        raster_render_target,
      };
      shadow_map_raster_pass->UpdateRTViews({ rt_views, uint32_t(std::size(rt_views)) });

      const std::shared_ptr<View> ds_views[] = {
        raster_depth_stencil,
      };
      shadow_map_raster_pass->UpdateDSViews({ ds_views, uint32_t(std::size(ds_views)) });

      const Pass::RTValue rt_values[] = {
        std::array<float, 4>{ NAN, NAN, 0.0f, 0.0f },
      };
      shadow_map_raster_pass->UpdateRTValues({ rt_values, uint32_t(std::size(rt_values)) });

      const Pass::DSValue ds_values[] = {
        { 1.0f, std::nullopt },
      };
      shadow_map_raster_pass->UpdateDSValues({ ds_values, uint32_t(std::size(ds_values)) });

      shadow_map_raster_pass->SetSubpassCount(arguments_views.size());
      for (uint32_t i = 0; i < arguments_views.size(); ++i)
      {
        const std::shared_ptr<View> va_views[] = {
          raster_vertex_items,
        };
        shadow_map_raster_pass->UpdateSubpassVAViews(i, { va_views, uint32_t(std::size(va_views)) });

        const std::shared_ptr<View> ia_views[] = {
          raster_index_items,
        };
        shadow_map_raster_pass->UpdateSubpassIAViews(i, { ia_views, uint32_t(std::size(ia_views)) });

        const uint32_t ue_offsets[] = {
          i * uint32_t(sizeof(Frustum)),
        };
        shadow_map_raster_pass->UpdateSubpassSBOffsets(i, { ue_offsets, uint32_t(std::size(ue_offsets)) });

        const std::shared_ptr<View> aa_views[] = {
          arguments_views[i]
        };
        shadow_map_raster_pass->UpdateSubpassAAViews(i, { aa_views, uint32_t(std::size(aa_views)) });

        shadow_map_raster_pass->SetSubpassLayout(i, shadow_map_raster_layout);
        shadow_map_raster_pass->SetSubpassShader(i, shadow_map_raster_shader);

      }
    }
  }

  void Spark::InitializeEnvironment()
  {
    const auto core = &this->GetCore();
    const auto device = core->AccessDevice();

    const auto extent_x = prop_extent_x->GetUint();
    const auto extent_y = prop_extent_y->GetUint();

    environment_vtx_data = device->CreateResource("environment_vtx_data");
    {
      environment_vtx_data->SetType(Resource::TYPE_BUFFER);
      environment_vtx_data->SetStride(uint32_t(sizeof(glm::f32vec4)));
      environment_vtx_data->SetCount(uint32_t(environment_vtx.size()));
      environment_vtx_data->SetInteropCount(1);
      environment_vtx_data->SetInteropItem(0, std::pair(environment_vtx.data(), uint32_t(environment_vtx.size() * sizeof(glm::f32vec4))));
    }

    environment_idx_data = device->CreateResource("environment_idx_data");
    {
      environment_idx_data->SetType(Resource::TYPE_BUFFER);
      environment_idx_data->SetStride(uint32_t(sizeof(glm::u32vec3)));
      environment_idx_data->SetCount(uint32_t(environment_idx.size()));
      environment_idx_data->SetInteropCount(1);
      environment_idx_data->SetInteropItem(0, std::pair(environment_idx.data(), uint32_t(environment_idx.size() * sizeof(glm::u32vec3))));
    }

    const auto environment_render_target = render_target->CreateView("environment_render_target");
    {
      environment_render_target->SetBind(View::BIND_RENDER_TARGET);
    }

    const auto environment_depth_stencil = depth_stencil->CreateView("environment_depth_stencil");
    {
      environment_depth_stencil->SetBind(View::BIND_DEPTH_STENCIL);
    }

    const auto environment_screen_data = screen_data->CreateView("environment_screen_data");
    {
      environment_screen_data->SetBind(View::BIND_CONSTANT_DATA);
    }

    const auto environment_camera_data = camera_data->CreateView("environment_camera_data");
    {
      environment_camera_data->SetBind(View::BIND_CONSTANT_DATA);
    }

    const auto environment_item_view = environment_item->CreateView("environment_item_view");
    {
      environment_item_view->SetBind(View::BIND_SHADER_RESOURCE);
    }

    const auto environment_vtx_items = environment_vtx_data->CreateView("environment_vtx_items");
    {
      environment_vtx_items->SetBind(View::BIND_VERTEX_ARRAY);
    }

    const auto environment_idx_items = environment_idx_data->CreateView("environment_idx_items");
    {
      environment_idx_items->SetBind(View::BIND_INDEX_ARRAY);
    }

    environment_shader = device->CreateShader("environment_shader");
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_environment.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      const auto source = shader_ss.str();

      environment_shader->SetSource(source);
      environment_shader->SetCompilation(Config::Compilation(Config::COMPILATION_VS | Config::COMPILATION_PS));
      environment_shader->SetTopology(Config::TOPOLOGY_TRIANGLELIST);
      environment_shader->SetIndexer(Config::INDEXER_32_BIT);

      const Config::Attribute attributes[] = {
        { 0, 0, 16, FORMAT_R32G32_FLOAT, false },
        { 0, 8, 16, FORMAT_R32G32_FLOAT, false },
      };
      environment_shader->UpdateAttributes({ attributes, uint32_t(std::size(attributes)) });

      environment_shader->SetFillMode(Config::FILL_SOLID);
      environment_shader->SetCullMode(Config::CULL_NONE);
      environment_shader->SetClipEnabled(false);

      const Config::Blend blends[] = {
        { false, Config::ARGUMENT_SRC_ALPHA, Config::ARGUMENT_INV_SRC_ALPHA, Config::OPERATION_ADD, Config::ARGUMENT_INV_SRC_ALPHA, Config::ARGUMENT_ZERO, Config::OPERATION_ADD, 0xF },
      };
      environment_shader->UpdateBlends({ blends, uint32_t(std::size(blends)) });
      environment_shader->SetATCEnabled(false);

      environment_shader->SetDepthEnabled(true);
      environment_shader->SetDepthWrite(false);
      environment_shader->SetDepthComparison(Config::COMPARISON_EQUAL);

      const Config::Viewport viewports[] = {
        { 0.0f, 0.0f, float(extent_x), float(extent_y), 0.0f, 1.0f },
      };
      environment_shader->UpdateViewports({ viewports, uint32_t(std::size(viewports)) });
    }

    environment_layout = device->CreateLayout("environment_layout");
    {
      const Layout::Sampler samplers[] = {
      { Layout::Sampler::FILTERING_ANISOTROPIC, 16, Layout::Sampler::ADDRESSING_REPEAT, Layout::Sampler::COMPARISON_NEVER, {0.0f, 0.0f, 0.0f, 0.0f},-FLT_MAX, FLT_MAX, 0.0f },
      };
      environment_layout->UpdateSamplers({ samplers, uint32_t(std::size(samplers)) });

      const std::shared_ptr<View> ub_views[] = {
        environment_screen_data,
        environment_camera_data,
      };
      environment_layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

      const std::shared_ptr<View> ri_views[] = {
        environment_item_view,
      };
      environment_layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });
    }


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

      const std::shared_ptr<View> va_views[] = {
        environment_vtx_items,
      };
      environment_pass->UpdateSubpassVAViews(0, { va_views, uint32_t(std::size(va_views)) });

      const std::shared_ptr<View> ia_views[] = {
        environment_idx_items,
      };
      environment_pass->UpdateSubpassIAViews(0, { ia_views, uint32_t(std::size(ia_views)) });

      //const std::shared_ptr<View> aa_views[] = {
      //  arguments_view,
      //};
      //environment_pass->UpdateSubpassAAViews(0, { aa_views, uint32_t(std::size(aa_views)) });

      Pass::Graphic environment_tasks[] = {
        {6, 1, 0, 0, 0},
      };

      environment_pass->UpdateSubpassGraphicTasks(0, { environment_tasks, uint32_t(std::size(environment_tasks)) });

      environment_pass->SetSubpassLayout(0, environment_layout);
      environment_pass->SetSubpassShader(0, environment_shader);
    }
  }

  void Spark::InitializePresent()
  {
    const auto core = &this->GetCore();
    const auto extent_x = property->GetObjectItem("camera")->GetObjectItem("extent_x")->GetUint();
    const auto extent_y = property->GetObjectItem("camera")->GetObjectItem("extent_y")->GetUint();

    const auto device = core->AccessDevice();

    auto present_frame_output = frame_output->CreateView("present_frame_output");
    {
      present_frame_output->SetBind(View::BIND_UNORDERED_ACCESS);
      present_frame_output->SetLayerOffset(0);
      present_frame_output->SetLayerCount(uint32_t(-1));
    }

    auto present_camera_data = camera_data->CreateView("present_camera_data");
    {
      present_camera_data->SetBind(View::BIND_CONSTANT_DATA);
      present_camera_data->SetByteOffset(0);
      present_camera_data->SetByteCount(uint32_t(-1));
    }

    auto present_render_target = render_target->CreateView("present_render_target");
    {
      present_render_target->SetBind(View::BIND_SHADER_RESOURCE);
      present_render_target->SetLayerOffset(0);
      present_render_target->SetLayerCount(uint32_t(-1));
    }

    present_arguments = device->CreateResource("present_arguments");
    {
      present_arguments->SetType(Resource::TYPE_BUFFER);
      present_arguments->SetStride(uint32_t(sizeof(Pass::Compute)));
      present_arguments->SetCount(1);
      present_arguments->SetHint(Resource::HINT_DYNAMIC_BUFFER);
    }

    auto arguments_view = present_arguments->CreateView("arguments_view");
    {
      arguments_view->SetBind(View::BIND_COMMAND_INDIRECT);
      arguments_view->SetByteOffset(0);
      arguments_view->SetByteCount(uint32_t(-1));
    }

    present_shader = device->CreateShader("present_shader");
    {
      std::fstream shader_fs;
      shader_fs.open("./asset/shaders/spark_present.hlsl", std::fstream::in);
      std::stringstream shader_ss;
      shader_ss << shader_fs.rdbuf();
      const std::string cs_source = shader_ss.str();

      present_shader->SetSource(cs_source);
      present_shader->SetCompilation(Config::COMPILATION_CS);
    }



    present_layout = device->CreateLayout("present_layout");
    {
      const std::shared_ptr<View> ub_views[] = {
        present_camera_data,
      };
      present_layout->UpdateUBViews({ ub_views, uint32_t(std::size(ub_views)) });

      present_layout->UpdateRBViews({});

      present_layout->UpdateWBViews({});

      const std::shared_ptr<View> ri_views[] = {
        present_render_target,
      };
      present_layout->UpdateRIViews({ ri_views, uint32_t(std::size(ri_views)) });

      const std::shared_ptr<View> wi_views[] = {
        present_frame_output,
      };
      present_layout->UpdateWIViews({ wi_views, uint32_t(std::size(wi_views)) });
    }


    {
      present_pass->SetType(Pass::TYPE_COMPUTE);
      present_pass->SetSubpassCount(1);
      present_pass->SetEnabled(true);

      const std::shared_ptr<View> aa_views[] = {
        arguments_view,
      };
      present_pass->UpdateSubpassAAViews(0, { aa_views, uint32_t(std::size(aa_views)) });

      present_pass->SetSubpassLayout(0, present_layout);
      present_pass->SetSubpassShader(0, present_shader);
    }
  }

  void Spark::DiscardResources()
  {}



  void Spark::Initialize()
  {
    const auto core = &this->GetCore();

    const auto prop_camera = property->GetObjectItem("camera");
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

    const auto prop_scene = property->GetObjectItem("scene");
    prop_instances = prop_scene->GetObjectItem("instances");
    prop_triangles = prop_scene->GetObjectItem("triangles");
    prop_vertices = prop_scene->GetObjectItem("vertices");
    prop_textures0 = prop_scene->GetObjectItem("textures0");
    prop_textures1 = prop_scene->GetObjectItem("textures1");
    prop_textures2 = prop_scene->GetObjectItem("textures2");
    prop_textures3 = prop_scene->GetObjectItem("textures3");
    prop_textures4 = prop_scene->GetObjectItem("textures4");

    InitializeResources();

    const auto device = core->AccessDevice();

    
    for (auto i = 0; i < 6; ++i)
    {
      shadow_passes[i] = device->CreatePass("spark_shadow_" + std::to_string(i));
    }
    no_shadow_raster_pass = device->CreatePass("spark_simple");
    shadow_map_raster_pass = device->CreatePass("spark_advanced");
    environment_pass = device->CreatePass("spark_environment");
    present_pass = device->CreatePass("spark_present");

    {
      InitializeShadow();
      InitializeSimple();
      InitializeAdvanced();
      InitializeEnvironment();
      InitializePresent();
    }
  }


  void Spark::Use()
  {
    switch (shadows)
    {
    case NO_SHADOWS:
    {
      no_shadow_raster_pass->SetEnabled(true);
      shadow_map_raster_pass->SetEnabled(false);
      present_pass->SetEnabled(true);
      for (auto& pass : shadow_passes)
      {
        pass->SetEnabled(false);
      }
      break;
    }
    case POINT_SHADOWS:
    {
      no_shadow_raster_pass->SetEnabled(false);
      shadow_map_raster_pass->SetEnabled(true);
      present_pass->SetEnabled(true);
      for (auto& pass : shadow_passes)
      {
        pass->SetEnabled(true);
      }
      break;
    }
    default:
    {
      no_shadow_raster_pass->SetEnabled(false);
      shadow_map_raster_pass->SetEnabled(false);
      present_pass->SetEnabled(false);
      for (auto& pass : shadow_passes)
      {
        pass->SetEnabled(false);
      }
      break;
    }
    }


    {
      auto graphic_arg = reinterpret_cast<Pass::Graphic*>(raster_arguments->Map());

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

      auto compute_arg = reinterpret_cast<Pass::Compute*>(present_arguments->Map());
      {
        compute_arg->grid_x = extent_x / 8;
        compute_arg->grid_y = extent_y / 8;
        compute_arg->grid_z = 1;
      }
      present_arguments->Unmap();
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