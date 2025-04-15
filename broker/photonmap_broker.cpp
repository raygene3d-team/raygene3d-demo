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
#include "photonmap_broker.h"


namespace RayGene3D
{
  void PhotonmapBroker::Initialize()
  {
    const auto tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_scene = tree->GetObjectItem("scene_property");
    {
      prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);
      prop_triangles = prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->GetArrayItem(0);
      prop_vertices = prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->GetArrayItem(0);
    }
  }

  void PhotonmapBroker::Use()
  {
    const auto [ins_array, ins_count] = prop_instances->GetRawTyped<Instance>(0);
    const auto [trg_array, trg_count] = prop_triangles->GetRawTyped<Triangle>(0);
    const auto [vrt_array, vrt_count] = prop_vertices->GetRawTyped<Vertex>(0);

    //glm::f32vec3 src = { 34.217, 0.957, 7.187 };

    //const auto dst = glm::packUnorm4x8(glm::packRGBM(glm::sqrt(src)));

    //const auto res = glm::unpackRGBM(glm::unpackUnorm4x8(dst)) * glm::unpackRGBM(glm::unpackUnorm4x8(dst));

    //int g = 8;
  }

  void PhotonmapBroker::Discard()
  {
    prop_vertices.reset();
    prop_triangles.reset();
    prop_instances.reset();

    prop_scene.reset();
  }

  PhotonmapBroker::PhotonmapBroker(Wrap& wrap)
    : Broker("photonmap_broker", wrap)
  {
  }

  PhotonmapBroker::~PhotonmapBroker()
  {
  }
}