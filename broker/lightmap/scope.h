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
  namespace Lightmap
  {
    class Scope
    {
    public:
      const std::unique_ptr<Core>& core;
      const std::unique_ptr<Util>& util;

    public:
      std::shared_ptr<Property> prop_scene;
      std::shared_ptr<Property> prop_camera;
      std::shared_ptr<Property> prop_lighting;
      std::shared_ptr<Property> prop_environment;
      std::shared_ptr<Property> prop_illumination;

      std::shared_ptr<Property> prop_inst;
      std::shared_ptr<Property> prop_trng;
      std::shared_ptr<Property> prop_vert;

      std::shared_ptr<Property> prop_counter;

    //  std::shared_ptr<Property> prop_theta;
    //  std::shared_ptr<Property> prop_phi;
    //  std::shared_ptr<Property> prop_intensity;

    public:
      std::shared_ptr<Property> prop_atlas_size_x;
      std::shared_ptr<Property> prop_atlas_size_y;
      std::shared_ptr<Property> prop_atlas_layers;

    public:
      SPtrResource screen_data;

      SPtrResource scene_buffer_inst;
      SPtrResource scene_buffer_trng;
      SPtrResource scene_buffer_vert;

      SPtrResource scene_array_aaam;
      SPtrResource scene_array_snno;
      SPtrResource scene_array_eeet;

      SPtrResource trace_buffer_tbox;
      SPtrResource trace_buffer_bbox;

      //Copies of original resources bacause of DX11 limitations
      SPtrResource trace_buffer_inst;
      SPtrResource trace_buffer_trng;
      SPtrResource trace_buffer_vert;

      SPtrResource lightmaps_input;
      SPtrResource lightmaps_accum;
      SPtrResource lightmaps_final;

    protected:
      void CreateScreenData();

      void CreateSceneBufferInst();
      void CreateSceneBufferTrng();
      void CreateSceneBufferVert();

      void CreateSceneArrayAAAM();
      void CreateSceneArraySNNO();
      void CreateSceneArrayEEET();

      void CreateTraceBufferTBox();
      void CreateTraceBufferBBox();

      void CreateTraceBufferInst();
      void CreateTraceBufferTrng();
      void CreateTraceBufferVert();

      void CreateLightmapsInput();
      void CreateLightmapsAccum();
      void CreateLightmapsFinal();

    protected:
      void DestroyScreenData();

      void DestroySceneBufferInst();
      void DestroySceneBufferTrng();
      void DestroySceneBufferVert();

      void DestroySceneArrayAAAM();
      void DestroySceneArraySNNO();
      void DestroySceneArrayEEET();

      void DestroyTraceBufferTBox();
      void DestroyTraceBufferBBox();

      void DestroyTraceBufferInst();
      void DestroyTraceBufferTrng();
      void DestroyTraceBufferVert();

      void DestroyLightmapsInput();
      void DestroyLightmapsAccum();
      void DestroyLightmapsFinal();

    public:
      Scope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util);
      ~Scope();
    };
  }
}