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
#include "render_3d_scope.h"

namespace RayGene3D
{
  class Render3DTechnique
  {
  protected:
    const Render3DScope& scope;

  protected:
    SPtrPass geometry_pass;
    SPtrState geometry_state;
    SPtrBatch geometry_batch;

    SPtrState skybox_state;
    SPtrBatch skybox_batch;

    SPtrPass present_pass;
    SPtrState present_state;
    SPtrBatch present_batch;

  protected:
    void CreateGeometryPass();
    void CreateGeometryState();
    void CreateGeometryBatch();

    void CreateSkyboxState();
    void CreateSkyboxBatch();

    void CreatePresentPass();
    void CreatePresentState();
    void CreatePresentBatch();

  protected:
    void DestroyGeometryBatch();
    void DestroyGeometryState();
    void DestroyGeometryPass();

    void DestroySkyboxBatch();
    void DestroySkyboxState();

    void DestroyPresentBatch();
    void DestroyPresentState();
    void DestroyPresentPass();

  public:
    Render3DTechnique(const Render3DScope& scope);
    virtual ~Render3DTechnique();
  };
}