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
#include "meshoptimizer_broker.h"

#include <meshoptimizer/src/meshoptimizer.h>


namespace RayGene3D
{

  void MeshoptimizerBroker::Initialize()
  {
  }

  void MeshoptimizerBroker::Use()
  {
    auto [ins_array, ins_count] = prop_instances->AccessRawTyped<Instance>();
    auto [trg_array, trg_count] = prop_triangles->AccessRawTyped<Triangle>();
    auto [vrt_array, vrt_count] = prop_vertices->AccessRawTyped<Vertex>();


    for (uint32_t i = 0; i < ins_count; ++i)
    {
      const auto trg_offset = ins_array[i].prim_offset;
      auto trg_items = trg_array + trg_offset;
      const auto trg_count = ins_array[i].prim_count;
      //auto trg_holder = std::vector<Triangle>(trg_items, trg_items + trg_count);

      const auto vrt_offset = ins_array[i].vert_offset;
      auto vrt_items = vrt_array + vrt_offset;
      const auto vrt_count = ins_array[i].vert_count;
      //auto vrt_holder = std::vector<Vertex>(vrt_items, vrt_items + vrt_count);

      meshopt_optimizeVertexCache((uint32_t*)trg_items, (const uint32_t*)trg_items, trg_count, vrt_count);
     
      meshopt_optimizeOverdraw((uint32_t*)trg_items, (const uint32_t*)trg_items, trg_count, (const float*)vrt_items, vrt_count, sizeof(Vertex), 1.0f);

      meshopt_optimizeVertexFetch((float*)vrt_items, (uint32_t*)trg_items, trg_count, (const float*)vrt_items, vrt_count, sizeof(Vertex));


    }
  }

  void MeshoptimizerBroker::Discard()
  {
  }

  MeshoptimizerBroker::MeshoptimizerBroker(Wrap& wrap)
    : Broker("meshoptimizer_broker", wrap)
  {
    const auto& prop_tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_scene = prop_tree->GetObjectItem("scene");
    {
      prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);
      prop_triangles = prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->GetArrayItem(0);
      prop_vertices = prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->GetArrayItem(0);
    }
  }

  MeshoptimizerBroker::~MeshoptimizerBroker()
  {
    prop_vertices.reset();
    prop_triangles.reset();
    prop_instances.reset();

    prop_scene.reset();
  }
}