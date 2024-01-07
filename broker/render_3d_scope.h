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
#include "../raygene3d-wrap/wrap.h"

namespace RayGene3D
{
  struct Render3DScope
  {
    SPtrView backbuffer_uav;
    SPtrView backbuffer_rtv;

    std::shared_ptr<Property> prop_scene;
    std::shared_ptr<Property> prop_camera;
    //std::shared_ptr<Property> prop_environment;

    std::shared_ptr<Property> prop_eye;
    std::shared_ptr<Property> prop_lookat;
    std::shared_ptr<Property> prop_up;

    std::shared_ptr<Property> prop_fov_x;
    std::shared_ptr<Property> prop_fov_y;

    std::shared_ptr<Property> prop_extent_x;
    std::shared_ptr<Property> prop_extent_y;

    std::shared_ptr<Property> prop_n_plane;
    std::shared_ptr<Property> prop_f_plane;

    std::shared_ptr<Property> prop_counter;

    std::shared_ptr<Property> prop_instances;
    std::shared_ptr<Property> prop_triangles;
    std::shared_ptr<Property> prop_vertices;

    //std::shared_ptr<Property> prop_vertices0;
    //std::shared_ptr<Property> prop_vertices1;
    //std::shared_ptr<Property> prop_vertices2;
    //std::shared_ptr<Property> prop_vertices3;

    std::shared_ptr<Property> prop_t_boxes;
    std::shared_ptr<Property> prop_b_boxes;

    std::shared_ptr<Property> prop_textures0;
    std::shared_ptr<Property> prop_textures1;
    std::shared_ptr<Property> prop_textures2;
    std::shared_ptr<Property> prop_textures3;

    std::shared_ptr<Property> prop_lightmaps;

    std::shared_ptr<Property> prop_skybox;
    std::shared_ptr<Property> prop_reflection_probe;


    SPtrResource color_target; // 10-10-10-2 - color (GI+occlusion / material)
    SPtrResource depth_target; // 32 - depth
    SPtrResource shadow_map;

    SPtrResource gbuffer_0_target; // 8-8-8-8 - albedo / metallic
    SPtrResource gbuffer_1_target; // 8-8-8-8 - normal / smoothness

    SPtrResource screen_data;
    SPtrResource camera_data;
    SPtrResource shadow_data;
    SPtrResource reflection_probe_data;

    SPtrResource scene_instances;
    SPtrResource scene_triangles;
    SPtrResource scene_vertices;
    //SPtrResource scene_vertices0;
    //SPtrResource scene_vertices1;
    //SPtrResource scene_vertices2;
    //SPtrResource scene_vertices3;

    SPtrResource scene_t_boxes;
    SPtrResource scene_b_boxes;

    //Copies of original resources bacause of DX11 limitations
    SPtrResource trace_instances;
    SPtrResource trace_triangles;
    SPtrResource trace_vertices;

    SPtrResource scene_textures0;
    SPtrResource scene_textures1;
    SPtrResource scene_textures2;
    SPtrResource scene_textures3;

    SPtrResource light_maps;

    SPtrResource reflection_probe;

    SPtrResource screen_quad_vertices;
    SPtrResource screen_quad_triangles;

    SPtrResource skybox_texture;

    SPtrResource graphic_arguments;
    SPtrResource compute_arguments;
  };
}