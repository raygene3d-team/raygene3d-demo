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
    SPtrProperty prop_scene;

  protected:
    SPtrProperty prop_buffer_trng;
    SPtrProperty prop_buffer_vert;
    SPtrProperty prop_buffer_mlet;

    SPtrProperty prop_buffer_inst;
    SPtrProperty prop_buffer_tbox;
    SPtrProperty prop_buffer_bbox;
    SPtrProperty prop_buffer_knot;

    SPtrProperty prop_array_aaam;
    SPtrProperty prop_array_snno;
    SPtrProperty prop_array_eeet;

  protected:
    SPtrResource scene_buffer_vert;
    SPtrResource scene_buffer_trng;   
    SPtrResource scene_buffer_inst;
    
    SPtrResource trace_buffer_vert;
    SPtrResource trace_buffer_trng;
    SPtrResource trace_buffer_mlet;

    SPtrResource trace_buffer_inst;
    SPtrResource trace_buffer_tbox;
    SPtrResource trace_buffer_bbox;
    SPtrResource trace_buffer_knot;

    SPtrResource scene_array_aaam;
    SPtrResource scene_array_snno;
    SPtrResource scene_array_eeet;

  protected:
    void CreateSceneBufferVert();
    void CreateSceneBufferTrng();
    void CreateSceneBufferInst();

    void CreateTraceBufferVert();
    void CreateTraceBufferTrng();
    void CreateTraceBufferMlet();    

    void CreateTraceBufferInst();
    void CreateTraceBufferTBox();
    void CreateTraceBufferBBox();
    void CreateTraceBufferKnot();

    void CreateSceneArrayAAAM();
    void CreateSceneArraySNNO();
    void CreateSceneArrayEEET();

  protected:
    void DestroySceneBufferVert();    
    void DestroySceneBufferTrng();
    void DestroySceneBufferInst();

    void DestroyTraceBufferVert();
    void DestroyTraceBufferTrng();
    void DestroyTraceBufferMlet();

    void DestroyTraceBufferInst();
    void DestroyTraceBufferTBox();
    void DestroyTraceBufferBBox();
    void DestroyTraceBufferKnot();

    void DestroySceneArrayAAAM();
    void DestroySceneArraySNNO();
    void DestroySceneArrayEEET();

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    SceneBroker(Wrap& wrap);
    virtual ~SceneBroker();
  };
}