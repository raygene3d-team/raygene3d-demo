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
#include "render_3d/mode.h"


namespace RayGene3D
{
  class Render3DBroker : public Broker
  {
  protected:
    Render3D::Scope scope;

  protected:
    std::unique_ptr<Render3D::Mode> mode;

  public:
    enum ShadowType
    {
      NO_SHADOW = 0,
      CUBEMAP_SHADOW = 1,
      SW_TRACED_SHADOW = 2,
      HW_TRACED_SHADOW = 3,
    };

    enum PipelineType
    {
      IA_PIPELINE = 0,
      MESH_PIPELINE = 1,
    };

  protected:
    ShadowType shadow{ NO_SHADOW };
    PipelineType pipeline{ IA_PIPELINE };

  public:
    void SetShadowType(ShadowType shadow) { this->shadow = shadow; }
    ShadowType GetShadowType() const { return shadow; }
    void SetPipelineType(PipelineType pipeline) { this->pipeline = pipeline; }
    PipelineType GetPipelineType() const { return pipeline; }

  public:
    void Update();

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    Render3DBroker(Wrap& wrap);
    virtual ~Render3DBroker();
  };
}