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

namespace RayGene3D
{
  namespace IO
  {
    class Mode
    {
    protected:
      Scope& scope;

    public:
      virtual void Import() = 0;
      virtual void Export() = 0;

    public:
      Mode(Scope& scope);
      virtual ~Mode();
    };


    std::tuple<std::vector<Vertex>, std::vector<Triangle>, glm::fvec3, glm::fvec3> PopulateInstance(size_t idx_count, uint32_t idx_align,
      const glm::uvec3& idx_order, const glm::fmat4x4& pos_transform, const glm::fmat3x3& nrm_transform, const glm::fmat2x2& tc0_transform,
      std::pair<const uint8_t*, size_t> pos_data, uint32_t pos_stride, std::pair<const uint8_t*, size_t> pos_idx_data, uint32_t pos_idx_stride,
      std::pair<const uint8_t*, size_t> nrm_data, uint32_t nrm_stride, std::pair<const uint8_t*, size_t> nrm_idx_data, uint32_t nrm_idx_stride,
      std::pair<const uint8_t*, size_t> tc0_data, uint32_t tc0_stride, std::pair<const uint8_t*, size_t> tc0_idx_data, uint32_t tc0_idx_stride);
  }
}