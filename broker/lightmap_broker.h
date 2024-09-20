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
#include "lightmap/mode/sw_traced_atlas.h"
#include "lightmap/mode/hw_traced_atlas.h"

namespace RayGene3D
{
  class LightmapBroker : public Broker
  {

  protected:
    std::unique_ptr<Lightmap::Mode> sw_traced_lightmap;
    std::unique_ptr<Lightmap::Mode> hw_traced_lightmap;

  protected:
    std::shared_ptr<Property> prop_scene;
    std::shared_ptr<Property> prop_illumination;

  protected:
    std::shared_ptr<Property> prop_atlas_size_x;
    std::shared_ptr<Property> prop_atlas_size_y;
    std::shared_ptr<Property> prop_atlas_layers;

  protected:
    std::shared_ptr<Property> prop_atlas;

  protected:
    SPtrResource scene_instances;
    SPtrResource scene_triangles;
    SPtrResource scene_vertices;

    SPtrResource scene_textures0;
    SPtrResource scene_textures1;
    SPtrResource scene_textures2;
    SPtrResource scene_textures3;
    SPtrResource scene_textures4;
    SPtrResource scene_textures5;
    SPtrResource scene_textures6;
    SPtrResource scene_textures7;

    SPtrResource trace_t_boxes;
    SPtrResource trace_b_boxes;

    //Copies of original resources bacause of DX11 limitations
    SPtrResource trace_instances;
    SPtrResource trace_triangles;
    SPtrResource trace_vertices;

    SPtrResource lightmaps_input;
    SPtrResource lightmaps_accum;
    SPtrResource lightmaps_final;

    SPtrPass compute_pass;
    SPtrConfig compute_config;
    SPtrBatch compute_batch;

    SPtrResource compute_arguments;

  protected:
    void CreateSceneInstances();
    void CreateSceneTriangles();
    void CreateSceneVertices();

    void CreateSceneTextures0();
    void CreateSceneTextures1();
    void CreateSceneTextures2();
    void CreateSceneTextures3();
    void CreateSceneTextures4();
    void CreateSceneTextures5();
    void CreateSceneTextures6();
    void CreateSceneTextures7();

    void CreateTraceTBoxes();
    void CreateTraceBBoxes();

    void CreateTraceInstances();
    void CreateTraceTriangles();
    void CreateTraceVertices();

    void CreateLightmapsInput();
    void CreateLightmapsAccum();
    void CreateLightmapsFinal();

    void CreateComputePass();
    void CreateComputeConfig();
    void CreateComputeBatch();

    void CreateComputeArguments();

  protected:
    void DestroySceneInstances();
    void DestroySceneTriangles();
    void DestroySceneVertices();

    void DestroySceneTextures0();
    void DestroySceneTextures1();
    void DestroySceneTextures2();
    void DestroySceneTextures3();
    void DestroySceneTextures4();
    void DestroySceneTextures5();
    void DestroySceneTextures6();
    void DestroySceneTextures7();

    void DestroyTraceTBoxes();
    void DestroyTraceBBoxes();

    void DestroyTraceInstances();
    void DestroyTraceTriangles();
    void DestroyTraceVertices();

    void DestroyLightmapsInput();
    void DestroyLightmapsAccum();
    void DestroyLightmapsFinal();

    void DiscardComputePass();
    void DiscardComputeConfig();
    void DiscardComputeBatch();

    void DiscardComputeArguments();

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    LightmapBroker(Wrap& wrap);
    virtual ~LightmapBroker();
  };
}