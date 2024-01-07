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
#include "render_3d_technique.h"

namespace RayGene3D
{
  class Render3DBroker : public Broker
  {
  protected:
    Render3DScope scope;

  protected:
    uint32_t shadow_resolution { 1024 };
    uint32_t reflection_probe_size { 1024 };
    glm::f32vec3 light_position{ -0.605f, 3.515f, 0.387f };

  public:
    enum ShadowType
    {
      DISABLED_SHADOW = 0,
      CUBEMAP_SHADOW = 1,
      SW_TRACED_SHADOW = 2,
      HW_TRACED_SHADOW = 3,
    };

    enum ShadingSubpass
    {
      SUBPASS_OPAQUE,
      SUBPASS_SKYBOX,
      SUBPASS_MAX_COUNT
    };

  protected:
    ShadowType shadows{ DISABLED_SHADOW };

  public:
    void SetShadowType(ShadowType shadows) { this->shadows = shadows; }
    ShadowType GetShadowType() const { return shadows; }

  protected:
    bool use_normal_oct_quad_encoding{ false };

  protected:
    void CreateColorTarget();
    void CreateDepthTarget();
    void CreateShadowMap();

    void CreateGBufferTarget0();
    void CreateGBufferTarget1();

    void CreateScreenData();
    void CreateCameraData();
    void CreateShadowData();

    void CreateSceneInstances();
    void CreateSceneTriangles();
    void CreateSceneVertices();

    void CreateSceneTBoxes();
    void CreateSceneBBoxes();

    void CreateTraceInstances();
    void CreateTraceTriangles();
    void CreateTraceVertices();

    void CreateSceneTextures0();
    void CreateSceneTextures1();
    void CreateSceneTextures2();
    void CreateSceneTextures3();

    void CreateLightMaps();

    void CreateScreenQuadVertices();
    void CreateScreenQuadTriangles();
    void CreateSkyboxTexture();

    void CreateGraphicArguments();
    void CreateComputeArguments();
    
  protected:
    void DestroyColorTarget();
    void DestroyDepthTarget();
    void DestroyShadowMap();

    void DestroyGBufferTarget0();
    void DestroyGBufferTarget1();

    void DestroyScreenData();
    void DestroyCameraData();
    void DestroyShadowData();

    void DestroySceneInstances();
    void DestroySceneTriangles();
    void DestroySceneVertices();

    void DestroySceneTBoxes();
    void DestroySceneBBoxes();

    void DestroyTraceInstances();
    void DestroyTraceTriangles();
    void DestroyTraceVertices();

    void DestroySceneTextures0();
    void DestroySceneTextures1();
    void DestroySceneTextures2();
    void DestroySceneTextures3();

    void DestroyLightMaps();

    void DestroyScreenQuadVertices();
    void DestroyScreenQuadTriangles();
    void DestroySkyboxTexture();

    void DestroyGraphicArguments();
    void DestroyComputeArguments();


  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    Render3DBroker(Wrap& wrap);
    virtual ~Render3DBroker();
  };
}