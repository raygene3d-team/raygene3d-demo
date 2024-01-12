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
#include "../render_3d_mode.h"

namespace RayGene3D
{
  class CubemapShadow : public Render3DMode
  {
  protected:
    SPtrPass shadowmap_passes[6];
    SPtrTechnique shadowmap_techniques[6];
    SPtrBatch shadowmap_batches[6];

    SPtrPass shadowed_pass;
    SPtrTechnique shadowed_technique;
    SPtrBatch shadowed_batch;

  protected:
    void CreateShadowmapPass(uint32_t index);
    void CreateShadowmapTechnique(uint32_t index);
    void CreateShadowmapBatch(uint32_t index);

    void CreateShadowedPass();
    void CreateShadowedTechnique();
    void CreateShadowedBatch();

  protected:
    void DestroyShadowmapBatch(uint32_t index);
    void DestroyShadowmapTechnique(uint32_t index);
    void DestroyShadowmapPass(uint32_t index);

    void DestroyShadowedBatch();
    void DestroyShadowedTechnique();
    void DestroyShadowedPass();

  public:
    void Enable() override;
    void Disable() override;

  public:
    CubemapShadow(const Render3DScope& scope);
    virtual ~CubemapShadow();
  };
}