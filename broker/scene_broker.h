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
  class SceneBroker : public Broker
  {
  protected:
    std::shared_ptr<Property> prop_scene;

  protected:
    std::shared_ptr<Property> prop_instances;

    std::shared_ptr<Property> prop_buffers0;
    std::shared_ptr<Property> prop_buffers1;
    std::shared_ptr<Property> prop_buffers2;
    std::shared_ptr<Property> prop_buffers3;

    std::shared_ptr<Property> prop_textures0;
    std::shared_ptr<Property> prop_textures1;
    std::shared_ptr<Property> prop_textures2;
    std::shared_ptr<Property> prop_textures3;

  protected:
    SPtrResource scene_instances;

    SPtrResource scene_buffers0;
    SPtrResource scene_buffers1;
    SPtrResource scene_buffers2;
    SPtrResource scene_buffers3;

    SPtrResource scene_textures0;
    SPtrResource scene_textures1;
    SPtrResource scene_textures2;
    SPtrResource scene_textures3;
    //SPtrResource scene_textures4;
    //SPtrResource scene_textures5;
    //SPtrResource scene_textures6;
    //SPtrResource scene_textures7;

  protected:
    void CreateSceneInstances();

    void CreateSceneBuffers0();
    void CreateSceneBuffers1();
    //void CreateSceneBuffers2();
    //void CreateSceneBuffers3();

    void CreateSceneTextures0();
    void CreateSceneTextures1();
    void CreateSceneTextures2();
    void CreateSceneTextures3();
    //void CreateSceneTextures4();
    //void CreateSceneTextures5();
    //void CreateSceneTextures6();
    //void CreateSceneTextures7();

  protected:
    void DestroySceneInstances();

    void DestroySceneBuffers0();
    void DestroySceneBuffers1();
    void DestroySceneBuffers2();
    void DestroySceneBuffers3();

    void DestroySceneTextures0();
    void DestroySceneTextures1();
    void DestroySceneTextures2();
    void DestroySceneTextures3();
    //void DestroySceneTextures4();
    //void DestroySceneTextures5();
    //void DestroySceneTextures6();
    //void DestroySceneTextures7();

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    SceneBroker(Wrap& wrap);
    virtual ~SceneBroker();
  };
}