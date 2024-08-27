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
  class LightmapScope
  {
  public:
    const std::unique_ptr<Core>& core;
    const std::unique_ptr<Util>& util;

  protected:
    std::shared_ptr<Property> prop_scene;

  protected:
    std::shared_ptr<Property> prop_instances;
    std::shared_ptr<Property> prop_triangles;
    std::shared_ptr<Property> prop_vertices;

  protected:
    std::shared_ptr<Property> prop_atlas;

  protected:
    std::shared_ptr<Property> prop_lightmaps;

  protected:
    SPtrResource scene_instances;
    SPtrResource scene_triangles;
    SPtrResource scene_vertices;

    //Copies of original resources bacause of DX11 limitations
    SPtrResource trace_instances;
    SPtrResource trace_triangles;
    SPtrResource trace_vertices;

    SPtrResource print_lightmaps;
    SPtrResource accum_lightmaps;
    SPtrResource final_lightmaps;

    SPtrPass graphic_pass;
    SPtrConfig graphic_config;
    SPtrBatch graphic_batch;
    SPtrResource graphic_arguments;

    SPtrPass compute_pass;
    SPtrConfig compute_config;
    SPtrBatch compute_batch;
    SPtrResource compute_arguments;

  protected:
    void CreateSceneInstances();
    void CreateSceneTriangles();
    void CreateSceneVertices();

    void CreateTraceInstances();
    void CreateTraceTriangles();
    void CreateTraceVertices();

    void CreatePrintLightmaps();
    void CreateAccumLightmaps();
    void CreateFinalLightmaps();

    void CreateGraphicPass();
    void CreateGraphicConfig();
    void CreateGraphicBatch();
    void CreateGraphicArguments();

    void CreateComputePass();
    void CreateComputeConfig();
    void CreateComputeBatch();
    void CreateComputeArguments();

  protected:
    void DiscardSceneInstances();
    void DiscardSceneTriangles();
    void DiscardSceneVertices();

    void DiscardTraceInstances();
    void DiscardTraceTriangles();
    void DiscardTraceVertices();

    void DiscardPrintLightmaps();
    void DiscardAccumLightmaps();
    void DiscardFinalLightmaps();

    void DiscardGraphicPass();
    void DiscardGraphicConfig();
    void DiscardGraphicBatch();
    void DiscardGraphicArguments();

    void DiscardComputePass();
    void DiscardComputeConfig();
    void DiscardComputeBatch();
    void DiscardComputeArguments();

  public:
    LightmapScope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util);
    virtual ~LightmapScope();
  };
}