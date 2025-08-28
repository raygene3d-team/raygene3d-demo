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
    struct Geometry
    {
      std::vector<Vertex> vertices;
      std::vector<Triangle> triangles;
      std::vector<Meshlet> meshlets;
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

    public:
      Geometry(size_t idx_count, uint32_t idx_align, const glm::uvec3& idx_order,
        CByteData pos_data, uint32_t pos_stride, CByteData pos_idx_data, uint32_t pos_idx_stride, const glm::fmat4x4& pos_transform,
        CByteData nrm_data, uint32_t nrm_stride, CByteData nrm_idx_data, uint32_t nrm_idx_stride, const glm::fmat3x3& nrm_transform,
        CByteData tc0_data, uint32_t tc0_stride, CByteData tc0_idx_data, uint32_t tc0_idx_stride, const glm::fmat2x2& tc0_transform);
      ~Geometry() {}
    };



    struct Material
    {
      uint32_t texture_index_0;
      uint32_t texture_index_1;
      uint32_t texture_index_2;
      uint32_t texture_index_3;

    public:
      Material(const std::string& name_0, const std::string& name_1, const std::string& name_2, const std::string& name_3);
    };


    class Mode
    {
    protected:
      Scope& scope;

    protected:
      xatlas::Atlas* atlas;

    public:
      virtual void Import() = 0;
      virtual void Export() = 0;

    protected:
      std::vector<Geometry> geometries;
      std::vector<Material> materials;

    public:
      void CalculateTangents(Geometry& geometry);
      void CalculateAtlas(Geometry& geometry);
      void CalculateMeshlets(Geometry& geometry);
      void CalculateBoxes(Geometry& geometry);

    public:
      Mode(Scope& scope);
      virtual ~Mode();
    };


  }
}