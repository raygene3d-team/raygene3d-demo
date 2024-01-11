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
  class HWTracedShadow : public Render3DMode
  {
  protected:
    SPtrPass hw_traced_pass;
    SPtrTechnique hw_traced_technique;
    SPtrBatch hw_traced_batch;

  protected:
    void CreateHWTracedPass();
    void CreateHWTracedTechnique();
    void CreateHWTracedBatch();

  protected:
    void DestroyHWTracedBatch();
    void DestroyHWTracedTechnique();
    void DestroyHWTracedPass();

  public:
    void Enable() override;
    void Disable() override;

  public:
    HWTracedShadow(const Render3DScope& scope);
    virtual ~HWTracedShadow();
  };
}