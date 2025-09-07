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
          sizeof(Screen),
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
          sizeof(Frustum),
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
          sizeof(Frustum),
          1,
        },
        Resource::Hint(Resource::HINT_DYNAMIC_BUFFER)
        );
    }

    void Scope::CreateSceneBufferInst()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_buffer_inst", scene_buffer_inst));
    }

    void Scope::CreateSceneBufferTrng()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_buffer_trng", scene_buffer_trng));
    }

    void Scope::CreateSceneBufferVert()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_buffer_vert", scene_buffer_vert));
    }

    void Scope::CreateTraceBufferTBox()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_tbox", trace_buffer_tbox));
    }

    void Scope::CreateTraceBufferBBox()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_bbox", trace_buffer_bbox));
    }

    void Scope::CreateTraceBufferInst()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_inst", trace_buffer_inst));
    }

    void Scope::CreateTraceBufferTrng()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_trng", trace_buffer_trng));
    }

    void Scope::CreateTraceBufferVert()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("trace_buffer_vert", trace_buffer_vert));
    }

    void Scope::CreateSceneArrayAAAM()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_array_aaam", scene_array_aaam));
    }

    void Scope::CreateSceneArraySNNO()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_array_snno", scene_array_snno));
    }

    void Scope::CreateSceneArrayEEET()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_array_eeet", scene_array_eeet));
    }

    //void Scope::CreateSceneTextures3()
    //{
    //  BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures3", scene_textures3));
    //}

    //void Scope::CreateSceneTextures4()
    //{
    //  BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures4", scene_textures4));
    //}

    //void Scope::CreateSceneTextures5()
    //{
    //  BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures5", scene_textures5));
    //}

    //void Scope::CreateSceneTextures6()
    //{
    //  BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures6", scene_textures6));
    //}

    //void Scope::CreateSceneTextures7()
    //{
    //  BLAST_ASSERT(core->GetDevice()->ObtainResource("scene_textures7", scene_textures7));
    //}

    //void Scope::CreateLightmapsInput()
    //{
    //  BLAST_ASSERT(core->GetDevice()->ObtainResource("lightmaps_input", lightmaps_input));
    //}

    //void Scope::CreateLightmapsAccum()
    //{
    //  BLAST_ASSERT(core->GetDevice()->ObtainResource("lightmaps_accum", lightmaps_accum));
    //}

    //void Scope::CreateLightmapsFinal()
    //{
    //  BLAST_ASSERT(core->GetDevice()->ObtainResource("lightmaps_final", lightmaps_final));
    //}

    void Scope::CreateScreenQuadVertices()
    {
      static const std::array<glm::f32vec2, 4> quad_vtx = {
        glm::f32vec2(-1.0f, 1.0f),
        glm::f32vec2( 1.0f, 1.0f),
        glm::f32vec2(-1.0f,-1.0f),
        glm::f32vec2( 1.0f,-1.0f),
      };

      screen_quad_vertices = core->GetDevice()->CreateResource("spark_screen_quad_vertices",
        Resource::BufferDesc
        {
          Usage(USAGE_VERTEX_ARRAY),
          sizeof(glm::f32vec2),
          quad_vtx.size(),
        },
        Resource::Hint(Resource::Hint::HINT_UNKNOWN),
        { reinterpret_cast<const uint8_t*>(quad_vtx.data()), quad_vtx.size() * sizeof(glm::f32vec2) }
      );
    }

    void Scope::CreateScreenQuadTriangles()
    {
      static const std::array<glm::u32vec3, 2> quad_idx = {
        glm::u32vec3(0u, 1u, 2u),
        glm::u32vec3(3u, 2u, 1u),
      };

      screen_quad_triangles = core->GetDevice()->CreateResource("spark_screen_quad_triangles",
        Resource::BufferDesc
        {
          Usage(USAGE_INDEX_ARRAY),
          sizeof(glm::u32vec3),
          quad_idx.size(),
        },
        Resource::Hint(Resource::Hint::HINT_UNKNOWN),
        { reinterpret_cast<const uint8_t*>(quad_idx.data()), quad_idx.size() * sizeof(glm::u32vec3) }
      );
    }

    void Scope::CreateSkyboxCubemap()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("environment_skybox_cubemap", skybox_cubemap));
    }

    void Scope::CreateReflectionMap()
    {
      BLAST_ASSERT(core->GetDevice()->ObtainResource("environment_reflection_map", reflection_map));
    }

    void Scope::CreateGraphicArguments()
    {
      const auto [data, count] = prop_inst->GetObjectItem("binary")->GetRawItems<Instance>(0);

      graphic_arguments = core->GetDevice()->CreateResource("spark_graphic_arguments",
        Resource::BufferDesc
        {
          Usage(USAGE_ARGUMENT_LIST),
          sizeof(Batch::Graphic),
          count,
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
          sizeof(Batch::Compute),
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

    void Scope::DestroySceneBufferInst()
    {
      core->GetDevice()->DestroyResource(scene_buffer_inst);
      scene_buffer_inst.reset();
    }

    void Scope::DestroySceneBufferTrng()
    {
      core->GetDevice()->DestroyResource(scene_buffer_trng);
      scene_buffer_trng.reset();
    }

    void Scope::DestroySceneBufferVert()
    {
      core->GetDevice()->DestroyResource(scene_buffer_vert);
      scene_buffer_vert.reset();
    }

    void Scope::DestroyTraceBufferTBox()
    {
      core->GetDevice()->DestroyResource(trace_buffer_tbox);
      trace_buffer_tbox.reset();
    }

    void Scope::DestroyTraceBufferBBox()
    {
      core->GetDevice()->DestroyResource(trace_buffer_bbox);
      trace_buffer_bbox.reset();
    }

    void Scope::DestroyTraceBufferInst()
    {
      core->GetDevice()->DestroyResource(trace_buffer_inst);
      trace_buffer_inst.reset();
    }

    void Scope::DestroyTraceBufferTrng()
    {
      core->GetDevice()->DestroyResource(trace_buffer_trng);
      trace_buffer_trng.reset();
    }

    void Scope::DestroyTraceBufferVert()
    {
      core->GetDevice()->DestroyResource(trace_buffer_vert);
      trace_buffer_vert.reset();
    }

    void Scope::DestroySceneArrayAAAM()
    {
      core->GetDevice()->DestroyResource(scene_array_aaam);
      scene_array_aaam.reset();
    }

    void Scope::DestroySceneArraySNNO()
    {
      core->GetDevice()->DestroyResource(scene_array_snno);
      scene_array_snno.reset();
    }

    void Scope::DestroySceneArrayEEET()
    {
      core->GetDevice()->DestroyResource(scene_array_eeet);
      scene_array_eeet.reset();
    }

    //void Scope::DestroySceneTextures3()
    //{
    //  core->GetDevice()->DestroyResource(scene_textures3);
    //  scene_textures3.reset();
    //}

    //void Scope::DestroySceneTextures4()
    //{
    //  core->GetDevice()->DestroyResource(scene_textures4);
    //  scene_textures4.reset();
    //}

    //void Scope::DestroySceneTextures5()
    //{
    //  core->GetDevice()->DestroyResource(scene_textures5);
    //  scene_textures5.reset();
    //}

    //void Scope::DestroySceneTextures6()
    //{
    //  core->GetDevice()->DestroyResource(scene_textures6);
    //  scene_textures6.reset();
    //}

    //void Scope::DestroySceneTextures7()
    //{
    //  core->GetDevice()->DestroyResource(scene_textures7);
    //  scene_textures7.reset();
    //}

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

    //void Scope::DestroyLightmapsInput()
    //{
    //  core->GetDevice()->DestroyResource(lightmaps_input);
    //  lightmaps_input.reset();
    //}

    //void Scope::DestroyLightmapsAccum()
    //{
    //  core->GetDevice()->DestroyResource(lightmaps_accum);
    //  lightmaps_accum.reset();
    //}

    //void Scope::DestroyLightmapsFinal()
    //{
    //  core->GetDevice()->DestroyResource(lightmaps_final);
    //  lightmaps_final.reset();
    //}

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
        prop_inst = prop_scene->GetObjectItem("buffer_inst");
        prop_trng = prop_scene->GetObjectItem("buffer_trng");
        prop_vert = prop_scene->GetObjectItem("buffer_vert");
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

      CreateColorTarget();
      CreateDepthTarget();
      CreateShadowMap();

      CreateGBufferTarget0();
      CreateGBufferTarget1();

      CreateScreenData();
      CreateCameraData();
      CreateShadowData();

      CreateSceneBufferInst();
      CreateSceneBufferTrng();
      CreateSceneBufferVert();

      CreateTraceBufferTBox();
      CreateTraceBufferBBox();

      CreateTraceBufferInst();
      CreateTraceBufferTrng();
      CreateTraceBufferVert();

      CreateSceneArrayAAAM();
      CreateSceneArraySNNO();
      CreateSceneArrayEEET();
      //CreateSceneTextures3();
      //CreateSceneTextures4();
      //CreateSceneTextures5();
      //CreateSceneTextures6();
      //CreateSceneTextures7();

      CreateScreenQuadVertices();
      CreateScreenQuadTriangles();

      CreateSkyboxCubemap();
      CreateReflectionMap();

      //CreateLightmapsInput();
      //CreateLightmapsAccum();
      //CreateLightmapsFinal();

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

      //DestroyLightmapsInput();
      //DestroyLightmapsAccum();
      //DestroyLightmapsFinal();

      DestroySceneArrayAAAM();
      DestroySceneArraySNNO();
      DestroySceneArrayEEET();
      //DestroySceneTextures3();
      //DestroySceneTextures4();
      //DestroySceneTextures5();
      //DestroySceneTextures6();
      //DestroySceneTextures7();

      DestroyTraceBufferInst();
      DestroyTraceBufferTrng();
      DestroyTraceBufferVert();

      DestroyTraceBufferTBox();
      DestroyTraceBufferBBox();

      DestroySceneBufferInst();
      DestroySceneBufferTrng();
      DestroySceneBufferVert();    

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
