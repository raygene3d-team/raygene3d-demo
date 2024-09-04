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


#include "scope.h"

namespace RayGene3D
{
  namespace Render3D
  {
    void Scope::CreateShadowMap()
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
        }
      );
    }

    void Scope::CreateColorTarget()
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

    void Scope::CreateDepthTarget()
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

    void Scope::CreateGBufferTarget0()
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

    void Scope::CreateGBufferTarget1()
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

    void Scope::CreateScreenData()
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

    void Scope::CreateCameraData()
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

    void Scope::CreateShadowData()
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

    void Scope::CreateSceneInstances()
    {
      //const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
      //std::pair<const void*, uint32_t> interops[] = {
      //  { data, uint32_t(sizeof(Instance)) * count },
      //};

      //scene_instances = core->GetDevice()->CreateResource("spark_scene_instances",
      //  Resource::BufferDesc
      //  {
      //    Usage(USAGE_CONSTANT_DATA),
      //    uint32_t(sizeof(Instance)),
      //    count,
      //  },
      //  Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      //  { interops, uint32_t(std::size(interops)) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_instances", scene_instances));
    }

    void Scope::CreateSceneTriangles()
    {
      //const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);
      //std::pair<const void*, uint32_t> interops[] = {
      //  { data, uint32_t(sizeof(Triangle)) * count },
      //};

      //scene_triangles = core->GetDevice()->CreateResource("spark_scene_triangles",
      //  Resource::BufferDesc
      //  {
      //    Usage(USAGE_INDEX_ARRAY),
      //    uint32_t(sizeof(Triangle)),
      //    count,
      //  },
      //  Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      //  { interops, uint32_t(std::size(interops)) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_triangles", scene_triangles));
    }

    void Scope::CreateSceneVertices()
    {
      //const auto [data, count] = prop_vertices->GetTypedBytes<Vertex>(0);
      //std::pair<const void*, uint32_t> interops[] = {
      //  { data, uint32_t(sizeof(Vertex)) * count },
      //};

      //scene_vertices = core->GetDevice()->CreateResource("spark_scene_vertices",
      //  Resource::BufferDesc
      //  {
      //    Usage(USAGE_VERTEX_ARRAY),
      //    uint32_t(sizeof(Vertex)),
      //    count,
      //  },
      //  Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      //  { interops, uint32_t(std::size(interops)) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_vertices", scene_vertices));
    }

    void Scope::CreateSceneTBoxes()
    {
      //const auto [data, count] = prop_t_boxes->GetTypedBytes<Box>(0);
      //std::pair<const void*, uint32_t> interops[] = {
      //  { data, uint32_t(sizeof(Box)) * count },
      //};

      //scene_t_boxes = core->GetDevice()->CreateResource("spark_scene_t_boxes",
      //  Resource::BufferDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    uint32_t(sizeof(Box)),
      //    count,
      //  },
      //  Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      //  { interops, uint32_t(std::size(interops)) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_t_boxes", scene_t_boxes));
    }

    void Scope::CreateSceneBBoxes()
    {
      //const auto [data, count] = prop_b_boxes->GetTypedBytes<Box>(0);
      //std::pair<const void*, uint32_t> interops[] = {
      //  { data, uint32_t(sizeof(Box)) * count },
      //};

      //scene_b_boxes = core->GetDevice()->CreateResource("spark_scene_b_boxes",
      //  Resource::BufferDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    uint32_t(sizeof(Box)),
      //    count,
      //  },
      //  Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      //  { interops, uint32_t(std::size(interops)) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_b_boxes", scene_b_boxes));
    }

    void Scope::CreateTraceInstances()
    {
      //const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
      //std::pair<const void*, uint32_t> interops[] = {
      //  { data, uint32_t(sizeof(Instance)) * count },
      //};

      //trace_instances = core->GetDevice()->CreateResource("spark_trace_instances",
      //  Resource::BufferDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    uint32_t(sizeof(Instance)),
      //    count,
      //  },
      //  Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      //  { interops, uint32_t(std::size(interops)) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_instances", trace_instances));
    }

    void Scope::CreateTraceTriangles()
    {
      //const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);
      //std::pair<const void*, uint32_t> interops[] = {
      //  { data, uint32_t(sizeof(Triangle)) * count },
      //};

      //trace_triangles = core->GetDevice()->CreateResource("spark_trace_triangles",
      //  Resource::BufferDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE | USAGE_RAYTRACING_INPUT),
      //    uint32_t(sizeof(Triangle)),
      //    count,
      //  },
      //  Resource::Hint(Resource::Hint::HINT_ADDRESS_BUFFER),
      //  { interops, uint32_t(std::size(interops)) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_triangles", trace_triangles));
    }

    void Scope::CreateTraceVertices()
    {
      //const auto [data, count] = prop_vertices->GetTypedBytes<Vertex>(0);
      //std::pair<const void*, uint32_t> interops[] = {
      //  { data, uint32_t(sizeof(Vertex)) * count },
      //};

      //trace_vertices = core->GetDevice()->CreateResource("spark_trace_vertices",
      //  Resource::BufferDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE | USAGE_RAYTRACING_INPUT),
      //    uint32_t(sizeof(Vertex)),
      //    count,
      //  },
      //  Resource::Hint(Resource::Hint::HINT_ADDRESS_BUFFER),
      //  { interops, uint32_t(std::size(interops)) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_vertices", trace_vertices));
    }

    void Scope::CreateSceneTextures0()
    {
      //const auto& layers = prop_textures0->GetObjectItem("layers");
      //const auto& mipmap = prop_textures0->GetObjectItem("mipmap");
      //const auto& extent_x = prop_textures0->GetObjectItem("extent_x");
      //const auto& extent_y = prop_textures0->GetObjectItem("extent_y");
      //const auto& raws = prop_textures0->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      //scene_textures0 = core->GetDevice()->CreateResource("spark_scene_textures0",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R8G8B8A8_SRGB,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures0", scene_textures0));
    }

    void Scope::CreateSceneTextures1()
    {
      //const auto& layers = prop_textures1->GetObjectItem("layers");
      //const auto& mipmap = prop_textures1->GetObjectItem("mipmap");
      //const auto& extent_x = prop_textures1->GetObjectItem("extent_x");
      //const auto& extent_y = prop_textures1->GetObjectItem("extent_y");
      //const auto& raws = prop_textures1->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      //scene_textures1 = core->GetDevice()->CreateResource("spark_scene_textures1",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R8G8B8A8_SRGB,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures1", scene_textures1));
    }

    void Scope::CreateSceneTextures2()
    {
      //const auto& layers = prop_textures2->GetObjectItem("layers");
      //const auto& mipmap = prop_textures2->GetObjectItem("mipmap");
      //const auto& extent_x = prop_textures2->GetObjectItem("extent_x");
      //const auto& extent_y = prop_textures2->GetObjectItem("extent_y");
      //const auto& raws = prop_textures2->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      //scene_textures2 = core->GetDevice()->CreateResource("spark_scene_textures2",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R8G8B8A8_SRGB,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures2", scene_textures2));
    }

    void Scope::CreateSceneTextures3()
    {
      //const auto& layers = prop_textures3->GetObjectItem("layers");
      //const auto& mipmap = prop_textures3->GetObjectItem("mipmap");
      //const auto& extent_x = prop_textures3->GetObjectItem("extent_x");
      //const auto& extent_y = prop_textures3->GetObjectItem("extent_y");
      //const auto& raws = prop_textures3->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      //scene_textures3 = core->GetDevice()->CreateResource("spark_scene_textures3",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R8G8B8A8_UNORM,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //);

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures3", scene_textures3));
    }

    void Scope::CreateSceneTextures4()
    {
      //const auto& layers = prop_textures4->GetObjectItem("layers");
      //const auto& mipmap = prop_textures4->GetObjectItem("mipmap");
      //const auto& extent_x = prop_textures4->GetObjectItem("extent_x");
      //const auto& extent_y = prop_textures4->GetObjectItem("extent_y");
      //const auto& raws = prop_textures4->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      //scene_textures4 = core->GetDevice()->CreateResource("spark_scene_textures4",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R8G8B8A8_SRGB,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //  );

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures4", scene_textures4));
    }

    void Scope::CreateSceneTextures5()
    {
      //const auto& layers = prop_textures5->GetObjectItem("layers");
      //const auto& mipmap = prop_textures5->GetObjectItem("mipmap");
      //const auto& extent_x = prop_textures5->GetObjectItem("extent_x");
      //const auto& extent_y = prop_textures5->GetObjectItem("extent_y");
      //const auto& raws = prop_textures5->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      //scene_textures5 = core->GetDevice()->CreateResource("spark_scene_textures5",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R8G8B8A8_UNORM,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //  );

       BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures5", scene_textures5));
    }

    void Scope::CreateSceneTextures6()
    {
      //const auto& layers = prop_textures6->GetObjectItem("layers");
      //const auto& mipmap = prop_textures6->GetObjectItem("mipmap");
      //const auto& extent_x = prop_textures6->GetObjectItem("extent_x");
      //const auto& extent_y = prop_textures6->GetObjectItem("extent_y");
      //const auto& raws = prop_textures6->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      //scene_textures6 = core->GetDevice()->CreateResource("spark_scene_textures6",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R8G8B8A8_UNORM,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //  );

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures6", scene_textures6));
    }

    void Scope::CreateSceneTextures7()
    {
      //const auto& layers = prop_textures7->GetObjectItem("layers");
      //const auto& mipmap = prop_textures7->GetObjectItem("mipmap");
      //const auto& extent_x = prop_textures7->GetObjectItem("extent_x");
      //const auto& extent_y = prop_textures7->GetObjectItem("extent_y");
      //const auto& raws = prop_textures7->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      //scene_textures7 = core->GetDevice()->CreateResource("spark_scene_textures7",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R8G8B8A8_UNORM,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //  );

      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures7", scene_textures7));
    }

    void Scope::CreateLightMaps()
    {
      const auto& layers = prop_lightmaps->GetObjectItem("layers");
      const auto& mipmap = prop_lightmaps->GetObjectItem("mipmap");
      const auto& extent_x = prop_lightmaps->GetObjectItem("extent_x");
      const auto& extent_y = prop_lightmaps->GetObjectItem("extent_y");
      const auto& raws = prop_lightmaps->GetObjectItem("raws");

      auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      {
        const auto& raw = raws->GetArrayItem(i);
        interops[i] = raw->GetRawBytes(0);
      }

      scene_lightmaps = core->GetDevice()->CreateResource("spark_scene_lightmaps",
        Resource::Tex2DDesc
        {
          Usage(USAGE_SHADER_RESOURCE),
          mipmap->GetUint(),
          layers->GetUint(),
          FORMAT_R8G8B8A8_UNORM,
          extent_x->GetUint(),
          extent_y->GetUint(),
        },
        Resource::Hint(Resource::HINT_LAYERED_IMAGE),
        { interops.data(), uint32_t(interops.size()) }
      );
    }

    void Scope::CreateScreenQuadVertices()
    {
      static const std::array<glm::f32vec2, 4> quad_vtx = {
        glm::f32vec2(-1.0f, 1.0f),
        glm::f32vec2( 1.0f, 1.0f),
        glm::f32vec2(-1.0f,-1.0f),
        glm::f32vec2( 1.0f,-1.0f),
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

    void Scope::CreateScreenQuadTriangles()
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

    void Scope::CreateSkyboxCubemap()
    {
      //const auto& layers = prop_skybox->GetObjectItem("layers");
      //const auto& mipmap = prop_skybox->GetObjectItem("mipmap");
      //const auto& extent_x = prop_skybox->GetObjectItem("extent_x");
      //const auto& extent_y = prop_skybox->GetObjectItem("extent_y");
      //const auto& raws = prop_skybox->GetObjectItem("raws");

      //auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
      //for (auto i = 0u; i < uint32_t(interops.size()); ++i)
      //{
      //  const auto& raw = raws->GetArrayItem(i);
      //  interops[i] = raw->GetRawBytes(0);
      //}

      BLAST_ASSERT(core->GetDevice()->ObtainResource("environment_skybox_cubemap", skybox_cubemap));


      //skybox_texture = core->GetDevice()->CreateResource("spark_skybox_texture",
      //  Resource::Tex2DDesc
      //  {
      //    Usage(USAGE_SHADER_RESOURCE),
      //    mipmap->GetUint(),
      //    layers->GetUint(),
      //    FORMAT_R32G32B32A32_FLOAT,
      //    extent_x->GetUint(),
      //    extent_y->GetUint(),
      //  },
      //  Resource::Hint(Resource::HINT_CUBEMAP_IMAGE | Resource::HINT_LAYERED_IMAGE),
      //  { interops.data(), uint32_t(interops.size()) }
      //);
    }

    void Scope::CreateReflectionMap()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("environment_reflection_map", reflection_map));
    }

    void Scope::CreateGraphicArguments()
    {
      const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);

      graphic_arguments = core->GetDevice()->CreateResource("spark_graphic_arguments",
        Resource::BufferDesc
        {
          Usage(USAGE_ARGUMENT_LIST),
          uint32_t(sizeof(Batch::Graphic)),
          uint32_t(count),
        },
        Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
        );
    }

    void Scope::CreateComputeArguments()
    {
      compute_arguments = core->GetDevice()->CreateResource("spark_compute_arguments",
        Resource::BufferDesc
        {
          Usage(USAGE_ARGUMENT_LIST),
          uint32_t(sizeof(Batch::Compute)),
          1u,
        },
        Resource::Hint(Resource::Hint::HINT_DYNAMIC_BUFFER)
        );
    }

    void Scope::DestroyColorTarget()
    {
      core->GetDevice()->DestroyResource(color_target);
      color_target.reset();
    }

    void Scope::DestroyDepthTarget()
    {
      core->GetDevice()->DestroyResource(depth_target);
      depth_target.reset();
    }

    void Scope::DestroyShadowMap()
    {
      core->GetDevice()->DestroyResource(shadow_map);
      shadow_map.reset();
    }

    void Scope::DestroyGBufferTarget0()
    {
      core->GetDevice()->DestroyResource(gbuffer_0_target);
      gbuffer_0_target.reset();
    }

    void Scope::DestroyGBufferTarget1()
    {
      core->GetDevice()->DestroyResource(gbuffer_1_target);
      gbuffer_1_target.reset();
    }

    void Scope::DestroyScreenData()
    {
      core->GetDevice()->DestroyResource(screen_data);
      screen_data.reset();
    }

    void Scope::DestroyCameraData()
    {
      core->GetDevice()->DestroyResource(camera_data);
      camera_data.reset();
    }

    void Scope::DestroyShadowData()
    {
      core->GetDevice()->DestroyResource(shadow_data);
      shadow_data.reset();
    }

    void Scope::DestroySceneInstances()
    {
      core->GetDevice()->DestroyResource(scene_instances);
      scene_instances.reset();
    }

    void Scope::DestroySceneTriangles()
    {
      core->GetDevice()->DestroyResource(scene_triangles);
      scene_triangles.reset();
    }

    void Scope::DestroySceneVertices()
    {
      core->GetDevice()->DestroyResource(scene_vertices);
      scene_vertices.reset();
    }

    void Scope::DestroySceneTBoxes()
    {
      core->GetDevice()->DestroyResource(scene_t_boxes);
      scene_t_boxes.reset();
    }

    void Scope::DestroySceneBBoxes()
    {
      core->GetDevice()->DestroyResource(scene_b_boxes);
      scene_b_boxes.reset();
    }

    void Scope::DestroyTraceInstances()
    {
      core->GetDevice()->DestroyResource(trace_instances);
      trace_instances.reset();
    }

    void Scope::DestroyTraceTriangles()
    {
      core->GetDevice()->DestroyResource(trace_triangles);
      trace_triangles.reset();
    }

    void Scope::DestroyTraceVertices()
    {
      core->GetDevice()->DestroyResource(trace_vertices);
      trace_vertices.reset();
    }

    void Scope::DestroySceneTextures0()
    {
      core->GetDevice()->DestroyResource(scene_textures0);
      scene_textures0.reset();
    }

    void Scope::DestroySceneTextures1()
    {
      core->GetDevice()->DestroyResource(scene_textures1);
      scene_textures1.reset();
    }

    void Scope::DestroySceneTextures2()
    {
      core->GetDevice()->DestroyResource(scene_textures2);
      scene_textures2.reset();
    }

    void Scope::DestroySceneTextures3()
    {
      core->GetDevice()->DestroyResource(scene_textures3);
      scene_textures3.reset();
    }

    void Scope::DestroySceneTextures4()
    {
      core->GetDevice()->DestroyResource(scene_textures4);
      scene_textures4.reset();
    }

    void Scope::DestroySceneTextures5()
    {
      core->GetDevice()->DestroyResource(scene_textures5);
      scene_textures5.reset();
    }

    void Scope::DestroySceneTextures6()
    {
      core->GetDevice()->DestroyResource(scene_textures6);
      scene_textures6.reset();
    }

    void Scope::DestroySceneTextures7()
    {
      core->GetDevice()->DestroyResource(scene_textures7);
      scene_textures7.reset();
    }

    void Scope::DestroyLightMaps()
    {
      core->GetDevice()->DestroyResource(scene_lightmaps);
      scene_lightmaps.reset();
    }

    void Scope::DestroyScreenQuadVertices()
    {
      core->GetDevice()->DestroyResource(screen_quad_vertices);
      screen_quad_vertices.reset();
    }

    void Scope::DestroyScreenQuadTriangles()
    {
      core->GetDevice()->DestroyResource(screen_quad_triangles);
      screen_quad_triangles.reset();
    }

    void Scope::DestroySkyboxCubemap()
    {
      core->GetDevice()->DestroyResource(skybox_cubemap);
      skybox_cubemap.reset();
    }

    void Scope::DestroyReflectionMap()
    {
      core->GetDevice()->DestroyResource(reflection_map);
      reflection_map.reset();
    }

    void Scope::DestroyGraphicArguments()
    {
      core->GetDevice()->DestroyResource(graphic_arguments);
      graphic_arguments.reset();
    }

    void Scope::DestroyComputeArguments()
    {
      core->GetDevice()->DestroyResource(compute_arguments);
      compute_arguments.reset();
    }

    Scope::Scope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util)
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


      prop_scene = util->GetStorage()->GetTree()->GetObjectItem("scene");
      {
        prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);
        prop_triangles = prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->GetArrayItem(0);
        prop_vertices = prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->GetArrayItem(0);

        ////prop_vertices0 = prop_scene->GetObjectItem("vertices0");
        ////prop_vertices1 = prop_scene->GetObjectItem("vertices1");
        ////prop_vertices2 = prop_scene->GetObjectItem("vertices2");
        ////prop_vertices3 = prop_scene->GetObjectItem("vertices3");

        //prop_t_boxes = prop_scene->GetObjectItem("t_boxes")->GetObjectItem("raws")->GetArrayItem(0);
        //prop_b_boxes = prop_scene->GetObjectItem("b_boxes")->GetObjectItem("raws")->GetArrayItem(0);

        //prop_textures0 = prop_scene->GetObjectItem("textures_0");
        //prop_textures1 = prop_scene->GetObjectItem("textures_1");
        //prop_textures2 = prop_scene->GetObjectItem("textures_2");
        //prop_textures3 = prop_scene->GetObjectItem("textures_3");
        //prop_textures4 = prop_scene->GetObjectItem("textures_4");
        //prop_textures5 = prop_scene->GetObjectItem("textures_5");
        //prop_textures6 = prop_scene->GetObjectItem("textures_6");
        //prop_textures7 = prop_scene->GetObjectItem("textures_7");

        prop_lightmaps = prop_scene->GetObjectItem("lightmaps");
      }




      prop_camera = util->GetStorage()->GetTree()->GetObjectItem("camera");
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

      prop_lighting = util->GetStorage()->GetTree()->GetObjectItem("lighting");
      {
        prop_theta = prop_lighting->GetObjectItem("theta");
        prop_phi = prop_lighting->GetObjectItem("phi");
        prop_intensity = prop_lighting->GetObjectItem("intensity");
      }

      prop_environment = util->GetStorage()->GetTree()->GetObjectItem("environment");
      {
        prop_skybox = prop_environment->GetObjectItem("skybox_cubemap");
      }

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
      CreateSceneTextures4();
      CreateSceneTextures5();
      CreateSceneTextures6();
      CreateSceneTextures7();

      //scene_instances = core->GetDevice()->FindResource("scene_instances");
      //scene_triangles = core->GetDevice()->FindResource("scene_triangles");
      //scene_vertices = core->GetDevice()->FindResource("scene_vertices");

      //scene_t_boxes = core->GetDevice()->FindResource("scene_t_boxes");
      //scene_b_boxes = core->GetDevice()->FindResource("scene_b_boxes");

      //trace_instances = core->GetDevice()->FindResource("trace_instances");
      //trace_triangles = core->GetDevice()->FindResource("trace_triangles");
      //trace_vertices = core->GetDevice()->FindResource("trace_vertices");

      //scene_textures0 = core->GetDevice()->FindResource("scene_textures0");
      //scene_textures1 = core->GetDevice()->FindResource("scene_textures1");
      //scene_textures2 = core->GetDevice()->FindResource("scene_textures2");
      //scene_textures3 = core->GetDevice()->FindResource("scene_textures3");
      //scene_textures4 = core->GetDevice()->FindResource("scene_textures4");
      //scene_textures5 = core->GetDevice()->FindResource("scene_textures5");
      //scene_textures6 = core->GetDevice()->FindResource("scene_textures6");
      //scene_textures7 = core->GetDevice()->FindResource("scene_textures7");

      CreateScreenQuadVertices();
      CreateScreenQuadTriangles();

      CreateSkyboxCubemap();
      CreateReflectionMap();

      CreateLightMaps();

      CreateGraphicArguments();
      CreateComputeArguments();
    }

    Scope::~Scope()
    {
      DestroyGraphicArguments();
      DestroyComputeArguments();

      DestroyScreenQuadVertices();
      DestroyScreenQuadTriangles();

      DestroyReflectionMap();
      DestroySkyboxCubemap();

      DestroyLightMaps();

      DestroySceneTextures0();
      DestroySceneTextures1();
      DestroySceneTextures2();
      DestroySceneTextures3();
      DestroySceneTextures4();
      DestroySceneTextures5();
      DestroySceneTextures6();
      DestroySceneTextures7();

      DestroyTraceInstances();
      DestroyTraceTriangles();
      DestroyTraceVertices();

      DestroySceneTBoxes();
      DestroySceneBBoxes();

      DestroySceneInstances();
      DestroySceneTriangles();
      DestroySceneVertices();

      //scene_instances.reset();
      //scene_triangles.reset();
      //scene_vertices.reset();

      //scene_t_boxes.reset();
      //scene_b_boxes.reset();

      //trace_instances.reset();
      //trace_triangles.reset();
      //trace_vertices.reset();

      //scene_textures0.reset();
      //scene_textures1.reset();
      //scene_textures2.reset();
      //scene_textures3.reset();
      //scene_textures4.reset();
      //scene_textures5.reset();
      //scene_textures6.reset();
      //scene_textures7.reset();
    

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
}
