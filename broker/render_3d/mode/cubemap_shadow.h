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
#include "../mode.h"

namespace RayGene3D
{
  namespace Render3D
  {
    class CubemapShadow : public Mode
    {
    protected:
      SPtrPass shadowmap_pass;
      SPtrConfig shadowmap_config;
      SPtrBatch shadowmap_batch;

      SPtrPass shadowed_pass;
      SPtrConfig shadowed_config;
      SPtrBatch shadowed_batch;

    protected:
      void CreateShadowmapPass();
      void CreateShadowmapConfig();
      void CreateShadowmapBatch();

      void CreateShadowedPass();
      void CreateShadowedConfig();
      void CreateShadowedBatch();

    protected:
      void DestroyShadowmapBatch();
      void DestroyShadowmapConfig();
      void DestroyShadowmapPass();

      void DestroyShadowedBatch();
      void DestroyShadowedConfig();
      void DestroyShadowedPass();

    public:
      void Enable() override;
      void Disable() override;

    public:
      CubemapShadow(const Scope& scope);
      virtual ~CubemapShadow();
    };
  }
}