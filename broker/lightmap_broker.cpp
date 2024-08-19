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
    const auto tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_scene = tree->GetObjectItem("scene");
    {
      prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);
      prop_triangles = prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->GetArrayItem(0);
      prop_vertices = prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->GetArrayItem(0);

      prop_atlas = prop_scene->GetObjectItem("atlas");
    }
  }

  void LightmapBroker::Use()
  {
    const auto [ins_array, ins_count] = prop_instances->GetTypedBytes<Instance>(0);
    const auto [trg_array, trg_count] = prop_triangles->GetTypedBytes<Triangle>(0);
    const auto [vrt_array, vrt_count] = prop_vertices->GetTypedBytes<Vertex>(0);

    const auto extent_x = prop_atlas->GetObjectItem("extent_x")->GetUint();
    const auto extent_y = prop_atlas->GetObjectItem("extent_y")->GetUint();
    const auto layers = prop_atlas->GetObjectItem("layers")->GetUint();

    BLAST_ASSERT(extent_x > 0u && extent_y > 0u && layers > 0u)

    {
      auto raws = std::vector<Raw>();
      for (uint32_t k = 0; k < layers; ++k)
      {
        auto raw = Raw(extent_x * extent_y * uint32_t(sizeof(glm::u8vec4)));
        for (uint32_t i = 0; i < extent_x * extent_y; ++i)
        {
          const auto x = (i % extent_x) >> 4;
          const auto y = (i / extent_x) >> 4;
          const auto pattern = (x + (y % 2)) % 2;
          const auto color = glm::u8vec4{
            255 - 128 * pattern,
            255 - 128 * pattern,
            255 - 128 * pattern,
            255
          };
          raw.SetElement<glm::u8vec4>(color, i);
        }
        raws.push_back(std::move(raw));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      prop_lightmaps = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, 1u, layers);
      
    }


  }

  void LightmapBroker::Discard()
  {
    prop_scene->SetObjectItem("lightmaps", prop_lightmaps);

    prop_vertices.reset();
    prop_triangles.reset();
    prop_instances.reset();

    prop_scene.reset();
  }

  LightmapBroker::LightmapBroker(Wrap& wrap)
    : Broker("lightmap_broker", wrap)
  {
  }

  LightmapBroker::~LightmapBroker()
  {
  }
}