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
#include "../raygene3d-root/root.h"

namespace RayGene3D
{
  class Core;

  //class tinygltf::Model;

  class ImportBroker : public Broker
  {
  protected:
    std::string file_name;
    std::string path_name;

  protected:
    float position_scale{ 1.0f };
    bool coordinate_flip{ false };
    uint32_t texture_level{ 10u };

  protected:
    uint32_t degenerated_geom_tris_count{ 0u };
    uint32_t degenerated_wrap_tris_count{ 0u };

  protected:
    std::vector<std::vector<Vertex>> vertices;
    std::vector<std::vector<Triangle>> triangles;
    std::vector<Instance> instances;
    std::vector<Texture> textures_0;
    std::vector<Texture> textures_1;
    std::vector<Texture> textures_2;
    std::vector<Texture> textures_3;

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    void SetFileName(const std::string& file_name) { this->file_name = file_name; }
    void SetPathName(const std::string& path_name) { this->path_name = path_name; }

  public:
    void SetPositionScale(float position_scale) { this->position_scale = position_scale; }
    void SetCoordinateFlip(bool coordinate_flip) { this->coordinate_flip = coordinate_flip; }
    void SetTextureLevel(uint32_t texture_level) { this->texture_level = texture_level; }

  private:
    //std::shared_ptr<Property> ImportOBJM(const std::string& path, const std::string& name, bool flip, float scale, uint32_t mipmaps);

    
    //void ImportGLTF(const std::string& path, const std::string& name, bool flip, float scale, uint32_t mipmaps);

    void ImportGLTF();
    void ImportOBJM();

    void ProcessTangents();


  public:
    ImportBroker(Root& root)
      : Broker("import_broker", root) {}
    virtual ~ImportBroker();
  };
}