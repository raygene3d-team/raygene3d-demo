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
  class EnvironmentBroker : public Broker
  {
  protected:
    std::shared_ptr<Property> prop_environment;

  protected:
    static const uint32_t mipmap{ 7u };

  protected:
    const std::unique_ptr<Core>& core;
    const std::unique_ptr<Util>& util;

  protected:
    SPtrPass pass;
    SPtrTechnique technique;
    SPtrBatch batch;

  protected:
    SPtrResource vtx_array;
    SPtrResource idx_array;
    SPtrResource constant_data;
    SPtrResource argument_list;
    SPtrResource skybox_cubemap;
    SPtrResource reflection_map;

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  protected:
    void CreatePass(uint32_t mipmap);
    void CreateTechnique(uint32_t mipmap);
    void CreateBatch(uint32_t mipmap);

  protected:
    void CreateVtxArray();
    void CreateIdxArray();
    void CreateConstantData();
    void CreateArgumentList();
    void CreateSkyboxCubemap();
    void CreateReflectionMap();

  protected:
    void DestroyPass(uint32_t mipmap);
    void DestroyTechnique(uint32_t mipmap);
    void DestroyBatch(uint32_t mipmap);

  protected:
    void DestroyConstantArray();
    void DestroySkyboxCubemap();
    void DestroyReflectionMap();


  public:
    EnvironmentBroker(Wrap& wrap);
    virtual ~EnvironmentBroker();
  };
}