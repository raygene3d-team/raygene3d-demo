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
#include "io/mode/obj_converter.h"
#include "io/mode/gltf_converter.h"

namespace RayGene3D
{
  class IOBroker : public Broker
  {
  protected:
    IO::Scope scope;

  protected:
    std::unique_ptr<IO::Mode> mode;

  //protected:
  //  uint32_t degenerated_geom_tris_count{ 0u };
  //  uint32_t degenerated_wrap_tris_count{ 0u };

  //protected:
  //  std::vector<Vertex> vertices;
  //  std::vector<Triangle> triangles;
  //  std::vector<Instance> instances;

  //  std::vector<std::tuple<Raw, uint32_t, uint32_t>> textures_0;
  //  std::vector<std::tuple<Raw, uint32_t, uint32_t>> textures_1;
  //  std::vector<std::tuple<Raw, uint32_t, uint32_t>> textures_2;
  //  std::vector<std::tuple<Raw, uint32_t, uint32_t>> textures_3;
  //  std::vector<std::tuple<Raw, uint32_t, uint32_t>> textures_4;
  //  std::vector<std::tuple<Raw, uint32_t, uint32_t>> textures_5;
  //  std::vector<std::tuple<Raw, uint32_t, uint32_t>> textures_6;
  //  std::vector<std::tuple<Raw, uint32_t, uint32_t>> textures_7;

    //Scene scene;

   //protected:
   // std::vector<Buffer> buffers;
   // std::vector<Texture> textures;
   // std::vector<Instance> instances;

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    void SetFileName(const std::string& file_name) { scope.file_name = file_name; }
    void SetPathName(const std::string& path_name) { scope.path_name = path_name; }

  public:
    void SetPositionScale(float position_scale) { scope.position_scale = position_scale; }
    void SetConversionRHS(bool conversion_rhs) { scope.conversion_rhs = conversion_rhs; }
    void SetTextureLevel(uint32_t texture_level) { scope.texture_level = texture_level; }

  public:
    void Export(std::shared_ptr<Property>& property) const;
    void Import(const std::shared_ptr<Property>& property);

  public:
    IOBroker(Wrap& wrap);
    virtual ~IOBroker();
  };
}