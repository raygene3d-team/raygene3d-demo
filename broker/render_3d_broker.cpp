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


#include "render_3d_broker.h"

namespace RayGene3D
{
  void Render3DBroker::Initialize()
  {
  }

  void Render3DBroker::Use()
  {
    no_shadow->Disable();
    cubemap_shadow->Disable();
    sw_traced_shadow->Disable();
    hw_traced_shadow->Disable();

    switch (mode)
    {
    case NO_SHADOW: 
      no_shadow->Enable(); break;
    case CUBEMAP_SHADOW:
      cubemap_shadow->Enable(); break;
    case SW_TRACED_SHADOW:
      sw_traced_shadow->Enable(); break;
    case HW_TRACED_SHADOW:
      hw_traced_shadow->Enable(); break;
    default:
      break;
    }

    {
      auto graphic_arg = reinterpret_cast<Batch::Graphic*>(scope.graphic_arguments->Map());

      const auto [instance_data, instance_count] = scope.prop_instances->GetTypedBytes<Instance>(0);
      for (uint32_t i = 0; i < instance_count; ++i)
      {
        graphic_arg[i].idx_count = instance_data[i].prim_count * 3u;
        graphic_arg[i].ins_count = 1u;
        graphic_arg[i].idx_offset = instance_data[i].prim_offset * 3u;
        graphic_arg[i].vtx_offset = instance_data[i].vert_offset * 1u;
        graphic_arg[i].ins_offset = 0u;
      }

      scope.graphic_arguments->Unmap();
    }

    {
      const auto extent_x = scope.prop_extent_x->GetUint();
      const auto extent_y = scope.prop_extent_y->GetUint();

      auto compute_arg = reinterpret_cast<Batch::Compute*>(scope.compute_arguments->Map());
      {
        compute_arg[0].grid_x = extent_x / 8u;
        compute_arg[0].grid_y = extent_y / 8u;
        compute_arg[0].grid_z = 1u;
      }
      scope.compute_arguments->Unmap();
    }

    {
      const auto extent_x = scope.prop_extent_x->GetUint();
      const auto extent_y = scope.prop_extent_y->GetUint();

      const auto counter = scope.prop_counter->GetUint();

      Screen screen;
      screen.extent_x = extent_x;
      screen.extent_y = extent_y;
      screen.rnd_base = counter;
      screen.rnd_seed = rand();

      auto screen_mapped = scope.screen_data->Map();
      memcpy(screen_mapped, &screen, sizeof(Screen));
      scope.screen_data->Unmap();
    }

    {
      const auto eye = glm::f32vec3{
        scope.prop_eye->GetArrayItem(0)->GetReal(),
        scope.prop_eye->GetArrayItem(1)->GetReal(),
        scope.prop_eye->GetArrayItem(2)->GetReal()
      };

      const auto lookat = glm::f32vec3{
        scope.prop_lookat->GetArrayItem(0)->GetReal(),
        scope.prop_lookat->GetArrayItem(1)->GetReal(),
        scope.prop_lookat->GetArrayItem(2)->GetReal()
      };

      const auto up = glm::f32vec3{
        scope.prop_up->GetArrayItem(0)->GetReal(),
        scope.prop_up->GetArrayItem(1)->GetReal(),
        scope.prop_up->GetArrayItem(2)->GetReal()
      };

      const auto fov_x = scope.prop_fov_x->GetReal();
      const auto fov_y = scope.prop_fov_y->GetReal();

      const auto n_plane = scope.prop_n_plane->GetReal();
      const auto f_plane = scope.prop_f_plane->GetReal();

      const auto extent_x = scope.prop_extent_x->GetUint();
      const auto extent_y = scope.prop_extent_y->GetUint();

      Frustum camera_frustum;
      camera_frustum.proj = glm::perspective(glm::radians(fov_y), float(extent_x) / float(extent_y), n_plane, f_plane);
      camera_frustum.proj_inv = glm::inverse(camera_frustum.proj);
      camera_frustum.view = glm::lookAt(eye, lookat, up);
      camera_frustum.view_inv = glm::inverse(camera_frustum.view);

      auto camera_mapped = scope.camera_data->Map();
      memcpy(camera_mapped, &camera_frustum, sizeof(Frustum));
      scope.camera_data->Unmap();
    }

    {
      auto bb_min = glm::f32vec3( FLT_MAX, FLT_MAX, FLT_MAX);
      auto bb_max = glm::f32vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
      const auto [instance_data, instance_count] = scope.prop_instances->GetTypedBytes<Instance>(0);
      for (uint32_t i = 0; i < instance_count; ++i)
      {
        bb_min = glm::min(bb_min, instance_data[i].bb_min); 
        bb_max = glm::max(bb_max, instance_data[i].bb_max);
      }

      const auto bb_center = 0.5f * (bb_max + bb_min);
      const auto bb_extent = 0.5f * (bb_max - bb_min);

      const auto radius = glm::length(bb_extent);
      const auto proj = glm::ortho(-radius, radius,-radius, radius, 0.01f, 2.0f * radius);
      const auto proj_inv = glm::inverse(proj);

      const auto phi = glm::radians(scope.prop_phi->GetReal());
      const auto theta = glm::radians(scope.prop_theta->GetReal());
      
      const auto direction = glm::euclidean(glm::f32vec2(phi, theta));
      const auto view = glm::lookAt(bb_center + direction * (radius + 0.01f), bb_center, glm::f32vec3{ 0.0f, 1.0f, 0.0f });
      const auto view_inv = glm::inverse(view);

      Frustum shadow_frustum;
      shadow_frustum.view = view;
      shadow_frustum.view_inv = view_inv;
      shadow_frustum.proj = proj;
      shadow_frustum.proj_inv = proj_inv;

      auto shadow_mapped = scope.shadow_data->Map();
      memcpy(shadow_mapped, &shadow_frustum, sizeof(Frustum));
      scope.shadow_data->Unmap();
    }
  }

  void Render3DBroker::Discard()
  {
  }

  Render3DBroker::Render3DBroker(Wrap& wrap)
    : Broker("spark_broker", wrap)
    , scope(wrap.GetCore(), wrap.GetUtil())
  {
    no_shadow = std::unique_ptr<Render3DMode>(new NoShadow(scope));
    cubemap_shadow = std::unique_ptr<Render3DMode>(new CubemapShadow(scope));
    sw_traced_shadow = std::unique_ptr<Render3DMode>(new SWTracedShadow(scope));
    hw_traced_shadow = std::unique_ptr<Render3DMode>(new HWTracedShadow(scope));
  }

  Render3DBroker::~Render3DBroker()
  {
    no_shadow.reset();
    cubemap_shadow.reset();
    sw_traced_shadow.reset();
    hw_traced_shadow.reset();
  }
}