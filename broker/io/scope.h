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
  namespace IO
  {
    class Scope
    {
      struct TextureArrayLDR
      {
        std::vector<Raw> raws;

        uint32_t extent_x{ 0u };
        uint32_t extent_y{ 0u };
        size_t mipmap{ 0u };
        size_t layers{ 0u };
        Raw raw;

      public:
        void Insert(size_t layer, Raw&& raw) { raws.insert(raws.cbegin() + layer, std::move(raw)); }
        Raw Remove(size_t layer) { auto raw = std::move(raws.at(layer)); raws.erase(raws.cbegin() + layer); return raw; }
        size_t Length() const { return raws.size(); }
        void Finalize(uint32_t extent_x, uint32_t extent_y, size_t mipmap = 1, size_t layers = 1);

      public:
        TextureArrayLDR() {}
        ~TextureArrayLDR() {}
      };

      struct TextureArrayHDR
      {
        std::vector<Raw> raws;

        uint32_t extent_x{ 0u };
        uint32_t extent_y{ 0u };
        size_t mipmap{ 0u };
        size_t layers{ 0u };
        Raw raw;

      public:
        void Insert(size_t layer, Raw&& raw) { raws.insert(raws.cbegin() + layer, std::move(raw)); }
        Raw Remove(size_t layer) { auto raw = std::move(raws.at(layer)); raws.erase(raws.cbegin() + layer); return raw; }
        size_t Length() const { return raws.size(); }
        void Finalize(uint32_t extent_x, uint32_t extent_y, size_t mipmap = 1, size_t layers = 1);

      public:
        TextureArrayHDR() {}
        ~TextureArrayHDR() {}
      };


      struct StructureBuffer
      {
        std::list<Raw> raws;

        size_t stride{ 0u };
        size_t count{ 0u };
        Raw raw;

      public:
        void Append(Raw&& raw) { raws.push_back(std::move(raw)); }
        Raw Consume() { auto raw = std::move(raws.back()); raws.pop_back(); return raw; }
        size_t Length() const { return raws.size(); }
        void Finalize(size_t stride, size_t count = 1);

      public:
        StructureBuffer() {}
        ~StructureBuffer() {}
      };

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

      StructureBuffer buffer_0; //vertices
      StructureBuffer buffer_1; //indices
      StructureBuffer buffer_2; //bones
      StructureBuffer buffer_3;

      TextureArrayLDR array_0; //albedo-metallic
      TextureArrayLDR array_1; //smoothness-normal-occlusion
      TextureArrayLDR array_2; //emission-transparency
      TextureArrayLDR array_3;

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