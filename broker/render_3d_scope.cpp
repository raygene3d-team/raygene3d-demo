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


#include "render_3d_scope.h"

namespace RayGene3D
{
  void Render3DScope::CreateShadowMap()
  {
    shadow_map = core->GetDevice()->CreateResource("spark_shadow_map",
      Resource::Tex2DDesc
      {
        Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_D16_UNORM,
        shadow_resolution,
        shadow_resolution,
      },
      Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE)
      );
  }

  void Render3DScope::CreateColorTarget()
  {
    color_target = core->GetDevice()->CreateResource("spark_color_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE | USAGE_UNORDERED_ACCESS),
        1,
        1,
        FORMAT_R11G11B10_FLOAT,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
      }
    );
  }

  void Render3DScope::CreateDepthTarget()
  {
    depth_target = core->GetDevice()->CreateResource("spark_depth_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_DEPTH_STENCIL | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_D32_FLOAT,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
      }
    );
  }

  void Render3DScope::CreateGBufferTarget0()
  {
    gbuffer_0_target = core->GetDevice()->CreateResource("spark_gbuffer_0_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_R8G8B8A8_UNORM,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
      }
    );
  }

  void Render3DScope::CreateGBufferTarget1()
  {
    gbuffer_1_target = core->GetDevice()->CreateResource("spark_gbuffer_1_target",
      Resource::Tex2DDesc
      {
        Usage(USAGE_RENDER_TARGET | USAGE_SHADER_RESOURCE),
        1,
        1,
        FORMAT_R8G8B8A8_UNORM,
        prop_extent_x->GetUint(),
        prop_extent_y->GetUint(),
      }
    );
  }

  void Render3DScope::CreateScreenData()
  {
    screen_data = core->GetDevice()->CreateResource("spark_screen_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Screen)),
        1,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );
  }

  void Render3DScope::CreateCameraData()
  {
    camera_data = core->GetDevice()->CreateResource("spark_camera_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Frustum)),
        1,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );
  }

  void Render3DScope::CreateShadowData()
  {
    shadow_data = core->GetDevice()->CreateResource("spark_shadow_data",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Frustum)),
        1,
      },
      Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
      );
  }

  void Render3DScope::CreateSceneInstances()
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_instances->GetRawBytes(0),
    };

    scene_instances = core->GetDevice()->CreateResource("spark_scene_instances",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Instance)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateSceneTriangles()
  {
    const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_triangles->GetRawBytes(0),
    };

    scene_triangles = core->GetDevice()->CreateResource("spark_scene_triangles",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        uint32_t(sizeof(Triangle)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateSceneVertices()
  {
    const auto [data, count] = prop_vertices->GetTypedBytes<Vertex>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_vertices->GetRawBytes(0),
    };

    scene_vertices = core->GetDevice()->CreateResource("spark_scene_vertices",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        uint32_t(sizeof(Vertex)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateSceneTBoxes()
  {
    const auto [data, count] = prop_t_boxes->GetTypedBytes<Box>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_t_boxes->GetRawBytes(0),
    };

    scene_t_boxes = core->GetDevice()->CreateResource("spark_scene_t_boxes",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        uint32_t(sizeof(Box)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateSceneBBoxes()
  {
    const auto [data, count] = prop_b_boxes->GetTypedBytes<Box>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_b_boxes->GetRawBytes(0),
    };

    scene_b_boxes = core->GetDevice()->CreateResource("spark_scene_b_boxes",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        uint32_t(sizeof(Box)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateTraceInstances()
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_instances->GetRawBytes(0),
    };

    trace_instances = core->GetDevice()->CreateResource("spark_trace_instances",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        uint32_t(sizeof(Instance)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateTraceTriangles()
  {
    const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_triangles->GetRawBytes(0),
    };

    trace_triangles = core->GetDevice()->CreateResource("spark_trace_triangles",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE | USAGE_RAYTRACING_INPUT),
        uint32_t(sizeof(Triangle)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_ADDRESS_BUFFER),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateTraceVertices()
  {
    const auto [data, count] = prop_vertices->GetTypedBytes<Vertex>(0);
    std::pair<const void*, uint32_t> interops[] = {
      prop_vertices->GetRawBytes(0),
    };

    trace_vertices = core->GetDevice()->CreateResource("spark_trace_vertices",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE | USAGE_RAYTRACING_INPUT),
        uint32_t(sizeof(Vertex)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_ADDRESS_BUFFER),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateSceneTextures0()
  {
    const auto layers = prop_textures0->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != prop_textures0->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures0->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures0->GetArrayItem(i)->GetRawBytes(0));
    }

    scene_textures0 = core->GetDevice()->CreateResource("spark_scene_textures0",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DScope::CreateSceneTextures1()
  {
    const auto layers = prop_textures1->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != prop_textures1->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures1->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures1->GetArrayItem(i)->GetRawBytes(0));
    }

    scene_textures1 = core->GetDevice()->CreateResource("spark_scene_textures1",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DScope::CreateSceneTextures2()
  {
    const auto layers = prop_textures2->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != prop_textures2->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures2->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures2->GetArrayItem(i)->GetRawBytes(0));
    }

    scene_textures2 = core->GetDevice()->CreateResource("spark_scene_textures2",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DScope::CreateSceneTextures3()
  {
    const auto layers = prop_textures3->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_UNORM;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != prop_textures3->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_textures3->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_textures3->GetArrayItem(i)->GetRawBytes(0));
    }

    scene_textures3 = core->GetDevice()->CreateResource("spark_scene_textures3",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DScope::CreateLightMaps()
  {
    const auto layers = prop_lightmaps->GetArraySize();
    const auto format = FORMAT_R8G8B8A8_SRGB;
    const auto bpp = 4u;

    auto mipmaps = 1u;
    auto extent_x = 1u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != prop_lightmaps->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_lightmaps->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_lightmaps->GetArrayItem(i)->GetRawBytes(0));
    }

    light_maps = core->GetDevice()->CreateResource("spark_light_maps",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DScope::CreateScreenQuadVertices()
  {
    static const std::array<glm::f32vec2, 4> quad_vtx = {
      glm::f32vec2(-1.0f, 1.0f),
      glm::f32vec2(1.0f, 1.0f),
      glm::f32vec2(-1.0f,-1.0f),
      glm::f32vec2(1.0f,-1.0f),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_vtx.data(), uint32_t(quad_vtx.size() * sizeof(glm::f32vec2)) },
    };

    screen_quad_vertices = core->GetDevice()->CreateResource("spark_screen_quad_vertices",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        uint32_t(sizeof(glm::f32vec2)),
        uint32_t(quad_vtx.size()),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateScreenQuadTriangles()
  {
    static const std::array<glm::u32vec3, 2> quad_idx = {
      glm::u32vec3(0u, 1u, 2u),
      glm::u32vec3(3u, 2u, 1u),
    };

    std::pair<const void*, uint32_t> interops[] = {
      { quad_idx.data(), uint32_t(quad_idx.size() * sizeof(glm::u32vec3)) },
    };

    screen_quad_triangles = core->GetDevice()->CreateResource("spark_screen_quad_triangles",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        uint32_t(sizeof(glm::u32vec3)),
        uint32_t(quad_idx.size()),
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
    );
  }

  void Render3DScope::CreateSkyboxTexture()
  {
    const auto layers = prop_skybox->GetArraySize();
    const auto format = FORMAT_R32G32B32A32_FLOAT;
    const auto bpp = 16u;

    auto mipmaps = 1u;
    auto extent_x = 2u;
    auto extent_y = 1u;
    auto size = 0u;
    while ((size += extent_x * extent_y * bpp) != prop_skybox->GetArrayItem(0)->GetRawBytes(0).second && mipmaps < 16u)
    {
      mipmaps += 1; extent_x <<= 1; extent_y <<= 1;
    }

    std::vector<std::pair<const void*, uint32_t>> interops;
    for (uint32_t i = 0; i < prop_skybox->GetArraySize(); ++i)
    {
      interops.emplace_back(prop_skybox->GetArrayItem(i)->GetRawBytes(0));
    }

    skybox_texture = core->GetDevice()->CreateResource("spark_skybox_texture",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmaps,
        layers,
        format,
        extent_x,
        extent_y,
      },
      Resource::Hint(Resource::HINT_UNKNOWN),
      { interops.data(), uint32_t(interops.size()) }
    );
  }

  void Render3DScope::CreateGraphicArguments()
  {
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

    graphic_arguments = core->GetDevice()->CreateResource("spark_graphic_arguments",
      Resource::BufferDesc
      {
        Usage(USAGE_ARGUMENT_INDIRECT),
        uint32_t(sizeof(Batch::Graphic)),
        uint32_t(count),
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
      );
  }

  void Render3DScope::CreateComputeArguments()
  {
    compute_arguments = core->GetDevice()->CreateResource("spark_compute_arguments",
      Resource::BufferDesc
      {
        Usage(USAGE_ARGUMENT_INDIRECT),
        uint32_t(sizeof(Batch::Compute)),
        1u,
      },
      Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
      );
  }

  void Render3DScope::DestroyColorTarget()
  {
    core->GetDevice()->DestroyResource(color_target);
    color_target.reset();
  }

  void Render3DScope::DestroyDepthTarget()
  {
    core->GetDevice()->DestroyResource(depth_target);
    depth_target.reset();
  }

  void Render3DScope::DestroyShadowMap()
  {
    core->GetDevice()->DestroyResource(shadow_map);
    shadow_map.reset();
  }

  void Render3DScope::DestroyGBufferTarget0()
  {
    core->GetDevice()->DestroyResource(gbuffer_0_target);
    gbuffer_0_target.reset();
  }

  void Render3DScope::DestroyGBufferTarget1()
  {
    core->GetDevice()->DestroyResource(gbuffer_1_target);
    gbuffer_1_target.reset();
  }

  void Render3DScope::DestroyScreenData()
  {
    core->GetDevice()->DestroyResource(screen_data);
    screen_data.reset();
  }

  void Render3DScope::DestroyCameraData()
  {
    core->GetDevice()->DestroyResource(camera_data);
    camera_data.reset();
  }

  void Render3DScope::DestroyShadowData()
  {
    core->GetDevice()->DestroyResource(shadow_data);
    shadow_data.reset();
  }

  void Render3DScope::DestroySceneInstances()
  {
    core->GetDevice()->DestroyResource(scene_instances);
    scene_instances.reset();
  }

  void Render3DScope::DestroySceneTriangles()
  {
    core->GetDevice()->DestroyResource(scene_triangles);
    scene_triangles.reset();
  }

  void Render3DScope::DestroySceneVertices()
  {
    core->GetDevice()->DestroyResource(scene_vertices);
    scene_vertices.reset();
  }

  void Render3DScope::DestroySceneTBoxes()
  {
    core->GetDevice()->DestroyResource(scene_t_boxes);
    scene_t_boxes.reset();
  }

  void Render3DScope::DestroySceneBBoxes()
  {
    core->GetDevice()->DestroyResource(scene_b_boxes);
    scene_b_boxes.reset();
  }

  void Render3DScope::DestroyTraceInstances()
  {
    core->GetDevice()->DestroyResource(trace_instances);
    trace_instances.reset();
  }

  void Render3DScope::DestroyTraceTriangles()
  {
    core->GetDevice()->DestroyResource(trace_triangles);
    trace_triangles.reset();
  }

  void Render3DScope::DestroyTraceVertices()
  {
    core->GetDevice()->DestroyResource(trace_vertices);
    trace_vertices.reset();
  }

  void Render3DScope::DestroySceneTextures0()
  {
    core->GetDevice()->DestroyResource(scene_textures0);
    scene_textures0.reset();
  }

  void Render3DScope::DestroySceneTextures1()
  {
    core->GetDevice()->DestroyResource(scene_textures1);
    scene_textures1.reset();
  }

  void Render3DScope::DestroySceneTextures2()
  {
    core->GetDevice()->DestroyResource(scene_textures2);
    scene_textures2.reset();
  }

  void Render3DScope::DestroySceneTextures3()
  {
    core->GetDevice()->DestroyResource(scene_textures3);
    scene_textures3.reset();
  }

  void Render3DScope::DestroyLightMaps()
  {
    core->GetDevice()->DestroyResource(light_maps);
    light_maps.reset();
  }

  void Render3DScope::DestroyScreenQuadVertices()
  {
    core->GetDevice()->DestroyResource(screen_quad_vertices);
    screen_quad_vertices.reset();
  }

  void Render3DScope::DestroyScreenQuadTriangles()
  {
    core->GetDevice()->DestroyResource(screen_quad_triangles);
    screen_quad_triangles.reset();
  }

  void Render3DScope::DestroySkyboxTexture()
  {
    core->GetDevice()->DestroyResource(skybox_texture);
    skybox_texture.reset();
  }

  void Render3DScope::DestroyGraphicArguments()
  {
    core->GetDevice()->DestroyResource(graphic_arguments);
    graphic_arguments.reset();
  }

  void Render3DScope::DestroyComputeArguments()
  {
    core->GetDevice()->DestroyResource(compute_arguments);
    compute_arguments.reset();
  }

  Render3DScope::Render3DScope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util)
    : core(core)
    , util(util)
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
    core->VisitView(find_view_fn);


    prop_scene = util->GetStorage()->GetTree()->GetObjectItem("scene_property");
    {
      prop_instances = prop_scene->GetObjectItem("instances");
      prop_triangles = prop_scene->GetObjectItem("triangles");
      prop_vertices = prop_scene->GetObjectItem("vertices");

      //prop_vertices0 = prop_scene->GetObjectItem("vertices0");
      //prop_vertices1 = prop_scene->GetObjectItem("vertices1");
      //prop_vertices2 = prop_scene->GetObjectItem("vertices2");
      //prop_vertices3 = prop_scene->GetObjectItem("vertices3");

      prop_t_boxes = prop_scene->GetObjectItem("t_boxes");
      prop_b_boxes = prop_scene->GetObjectItem("b_boxes");

      prop_textures0 = prop_scene->GetObjectItem("textures0");
      prop_textures1 = prop_scene->GetObjectItem("textures1");
      prop_textures2 = prop_scene->GetObjectItem("textures2");
      prop_textures3 = prop_scene->GetObjectItem("textures3");

      prop_lightmaps = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
      {
        prop_lightmaps->SetArraySize(uint32_t(1));

        const auto texel_value = glm::u8vec4(255, 255, 255, 255);
        const auto texel_size = uint32_t(sizeof(texel_value));

        const auto texels_property = std::shared_ptr<Property>(new Property(Property::TYPE_RAW));
        texels_property->RawAllocate(texel_size);
        texels_property->SetRawBytes({ &texel_value, texel_size }, 0);
        prop_lightmaps->SetArrayItem(0, texels_property);
      }

      //prop_lightmaps = prop_scene->GetObjectItem("textures4");
    }

    prop_camera = util->GetStorage()->GetTree()->GetObjectItem("camera_property");
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

    prop_skybox = util->GetStorage()->GetTree()->GetObjectItem("environment_property");

    CreateColorTarget();
    CreateDepthTarget();
    CreateShadowMap();

    CreateGBufferTarget0();
    CreateGBufferTarget1();

    CreateScreenData();
    CreateCameraData();
    CreateShadowData();

    CreateSceneInstances();
    CreateSceneTriangles();
    CreateSceneVertices();

    CreateSceneTBoxes();
    CreateSceneBBoxes();

    CreateTraceInstances();
    CreateTraceTriangles();
    CreateTraceVertices();

    CreateSceneTextures0();
    CreateSceneTextures1();
    CreateSceneTextures2();
    CreateSceneTextures3();

    CreateScreenQuadVertices();
    CreateScreenQuadTriangles();
    CreateSkyboxTexture();

    CreateLightMaps();

    CreateGraphicArguments();
    CreateComputeArguments();
  }

  Render3DScope::~Render3DScope()
  {
    DestroyGraphicArguments();
    DestroyComputeArguments();

    DestroyScreenQuadVertices();
    DestroyScreenQuadTriangles();
    DestroySkyboxTexture();

    DestroyLightMaps();

    DestroySceneTextures0();
    DestroySceneTextures1();
    DestroySceneTextures2();
    DestroySceneTextures3();

    DestroyTraceInstances();
    DestroyTraceTriangles();
    DestroyTraceVertices();

    DestroySceneTBoxes();
    DestroySceneBBoxes();

    DestroySceneInstances();
    DestroySceneTriangles();
    DestroySceneVertices();

    DestroyScreenData();
    DestroyCameraData();
    DestroyShadowData();

    DestroyColorTarget();
    DestroyDepthTarget();
    DestroyShadowMap();

    DestroyGBufferTarget0();
    DestroyGBufferTarget1();
  }
}
