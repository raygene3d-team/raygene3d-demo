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

#include "../raygene3d-util/util/array.h"
#include "../raygene3d-util/util/buffer.h"

namespace RayGene3D
{
  namespace IO
  {
    class Scope
    {
    public:
      const std::unique_ptr<Core>& core;
      const std::unique_ptr<Util>& util;

    public:
      std::string file_name;
      std::string path_name;

    public:
      float position_scale{ 1.0f };
      bool conversion_rhs{ false };
      uint32_t texture_level{ 10u };

    public:
      std::vector<Instance> instances;

      std::unique_ptr<StructureBuffer> buffer_0; //vertices
      std::unique_ptr<StructureBuffer> buffer_1; //indices
      std::unique_ptr<StructureBuffer> buffer_2; //bones
      std::unique_ptr<StructureBuffer> buffer_3;

      std::unique_ptr<TextureArrayLDR> array_0; //albedo-metallic
      std::unique_ptr<TextureArrayLDR> array_1; //smoothness-normal-occlusion
      std::unique_ptr<TextureArrayLDR> array_2; //emission-transparency
      std::unique_ptr<TextureArrayLDR> array_3;

    public:
      std::shared_ptr<Property> prop_scene;

    public:
      void Import();
      void Export();

    public:
      Scope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util);
      ~Scope();
    };
  }
}