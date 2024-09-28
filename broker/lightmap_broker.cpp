
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
#include "lightmap_broker.h"


namespace RayGene3D
{

  void LightmapBroker::Initialize()
  {
  }

  void LightmapBroker::Use()
  {
    //{
    //  const auto extent_x = scope.prop_atlas_size_x->GetUint();
    //  const auto extent_y = scope.prop_atlas_size_x->GetUint();

    //  const auto counter = scope.prop_counter->GetUint();

    //  Screen screen;
    //  screen.extent_x = extent_x;
    //  screen.extent_y = extent_y;
    //  screen.rnd_base = counter;
    //  screen.rnd_seed = rand();

    //  auto screen_mapped = scope.screen_data->Map();
    //  memcpy(screen_mapped, &screen, sizeof(Screen));
    //  scope.screen_data->Unmap();
    //}

    sw_traced_lightmap->Disable();
    //hw_traced_lightmap->Disable();

    switch (mode)
    {
    case SW_TRACED_BAKING:
      sw_traced_lightmap->Enable(); break;
    case HW_TRACED_BAKING:
      //hw_traced_lightmap->Enable(); break;
    default:
      break;
    }

    scope.core->GetDevice()->Use();

    sw_traced_lightmap->Disable();
    //hw_traced_lightmap->Disable();
  }

  void LightmapBroker::Discard()
  {
  }

  LightmapBroker::LightmapBroker(Wrap& wrap)
    : Broker("lightmap_broker", wrap)
    , scope(wrap.GetCore(), wrap.GetUtil())
  {
    sw_traced_lightmap = std::unique_ptr<Lightmap::Mode>(new Lightmap::SWTracedAtlas(scope));
    //hw_traced_lightmap = std::unique_ptr<Lightmap::Mode>(new Lightmap::HWTracedAtlas(scope));
  }

  LightmapBroker::~LightmapBroker()
  {
    sw_traced_lightmap.reset();
    hw_traced_lightmap.reset();
  }
}