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

#include "scope.h"

#include <mikktspace/mikktspace.h>
#include <meshoptimizer/src/meshoptimizer.h>
#include <xatlas/xatlas.h>

namespace RayGene3D
{
  namespace IO
  {
    class Mode;

    struct Mesh
    {
    public:
      std::vector<Vertex> vertices;
      std::vector<Triangle> triangles;

      std::vector<Meshlet> meshlets;
      std::vector<uint32_t> v_indices;
      std::vector<uint8_t> t_indices;

      std::vector<Box> boxes;

      glm::f32vec3 aabb_min = glm::f32vec3{ FLT_MAX, FLT_MAX, FLT_MAX };
      glm::f32vec3 aabb_max = glm::f32vec3{-FLT_MAX,-FLT_MAX,-FLT_MAX };

      size_t degenerated_geom_tris_count{ 0u };
      size_t degenerated_wrap_tris_count{ 0u };

      double meshopt_avg_vertices{ 0.0 };
      double meshopt_avg_triangles{ 0.0 };
      double meshopt_avg_boundary{ 0.0 };
      double meshopt_avg_connected{ 0.0 };
      size_t meshopt_not_full{ 0u };

      glm::f32vec4 material_param_0{ 0.0f, 0.0f, 0.0f, 0.0f };
      glm::f32vec4 material_param_1{ 0.0f, 0.0f, 0.0f, 0.0f };
      glm::f32vec4 material_param_2{ 0.0f, 0.0f, 0.0f, 0.0f };
      glm::f32vec4 material_param_3{ 0.0f, 0.0f, 0.0f, 0.0f };

      uint32_t aaam_texture_index{ uint32_t(-1) };
      uint32_t snno_texture_index{ uint32_t(-1) };
      uint32_t eeet_texture_index{ uint32_t(-1) };
      uint32_t mask_texture_index{ uint32_t(-1) };

    public:
      Mesh(size_t idx_count, uint32_t idx_align, const glm::uvec3& idx_order,
        CByteData pos_data, uint32_t pos_stride, CByteData pos_idx_data, uint32_t pos_idx_stride, const glm::fmat4x4& pos_transform,
        CByteData nrm_data, uint32_t nrm_stride, CByteData nrm_idx_data, uint32_t nrm_idx_stride, const glm::fmat3x3& nrm_transform,
        CByteData tc0_data, uint32_t tc0_stride, CByteData tc0_idx_data, uint32_t tc0_idx_stride, const glm::fmat2x2& tc0_transform,
        uint32_t aaam_index, uint32_t snno_index, uint32_t eeet_index, uint32_t mask_index,
        const glm::f32vec4& param_0, const glm::f32vec4& param_1, const glm::f32vec4& param_2, const glm::f32vec4& param_3);
      ~Mesh() {}
    };


    class Mode
    {
    protected:
      Scope& scope;

    public:
      std::vector<Mesh> mesh_items;
      std::vector<Raw> aaam_items;
      std::vector<Raw> snno_items;
      std::vector<Raw> eeet_items;
      std::vector<Raw> mask_items;
      std::vector<Box> boxes;

      size_t meshlet_opt_count {0u};

    public:
      void CalculateTangents();
      void CalculateAtlas();
      void CalculateMeshlets();
      void CalculateBoxes();

    public:
      virtual void Import() = 0;
      virtual void Export() = 0;

    public:
      Mode(Scope& scope);
      virtual ~Mode();
    };


  }
}