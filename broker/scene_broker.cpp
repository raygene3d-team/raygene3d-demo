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
#include "scene_broker.h"


namespace RayGene3D
{
  void SceneBroker::CreateSceneInstances()
  {
    const auto [data, count] = prop_instances->GetRawItems<Instance>(0);
    const auto stride = sizeof(Instance);

    scene_instances = wrap.GetCore()->GetDevice()->CreateResource("scene_instances",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(data), stride * count }
      );
  }

  void SceneBroker::CreateSceneBuffer0()
  {
    const auto [data, count] = prop_buffer_0->GetRawItems<Vertex>(0);
    const auto stride = sizeof(Vertex);

    scene_buffer_0 = wrap.GetCore()->GetDevice()->CreateResource("scene_buffer_0",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(data), stride * count }
      );
  }

  void SceneBroker::CreateSceneBuffer1()
  {
    const auto [data, count] = prop_buffer_1->GetRawItems<Triangle>(0);
    const auto stride = sizeof(Triangle);

    scene_buffer_1 = wrap.GetCore()->GetDevice()->CreateResource("scene_buffer_1",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(data), stride * count }
      );
  }

  void SceneBroker::CreateSceneArray0()
  {
    const auto format = Format(prop_array_0->GetObjectItem("format")->GetUint());
    const auto layers = prop_array_0->GetObjectItem("layers")->GetUint();
    const auto mipmap = prop_array_0->GetObjectItem("mipmap")->GetUint();
    const auto size_x = prop_array_0->GetObjectItem("size_x")->GetUint();
    const auto size_y = prop_array_0->GetObjectItem("size_y")->GetUint();
    const auto bytes = prop_array_0->GetObjectItem("raw")->GetRawBytes();

    scene_array_0 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_0",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      bytes);
  }

  void SceneBroker::CreateSceneArray1()
  {
    const auto format = Format(prop_array_1->GetObjectItem("format")->GetUint());
    const auto layers = prop_array_1->GetObjectItem("layers")->GetUint();
    const auto mipmap = prop_array_1->GetObjectItem("mipmap")->GetUint();
    const auto size_x = prop_array_1->GetObjectItem("size_x")->GetUint();
    const auto size_y = prop_array_1->GetObjectItem("size_y")->GetUint();
    const auto bytes = prop_array_1->GetObjectItem("raw")->GetRawBytes();

    scene_array_2 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_2",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      bytes);
  }

  void SceneBroker::CreateSceneArray2()
  {
    const auto format = Format(prop_array_2->GetObjectItem("format")->GetUint());
    const auto layers = prop_array_2->GetObjectItem("layers")->GetUint();
    const auto mipmap = prop_array_2->GetObjectItem("mipmap")->GetUint();
    const auto size_x = prop_array_2->GetObjectItem("size_x")->GetUint();
    const auto size_y = prop_array_2->GetObjectItem("size_y")->GetUint();
    const auto bytes = prop_array_2->GetObjectItem("raw")->GetRawBytes();

    scene_array_2 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_2",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      bytes);
  }

  void SceneBroker::CreateSceneArray3()
  {
    const auto format = Format(prop_array_3->GetObjectItem("format")->GetUint());
    const auto layers = prop_array_3->GetObjectItem("layers")->GetUint();
    const auto mipmap = prop_array_3->GetObjectItem("mipmap")->GetUint();
    const auto size_x = prop_array_3->GetObjectItem("size_x")->GetUint();
    const auto size_y = prop_array_3->GetObjectItem("size_y")->GetUint();
    const auto bytes = prop_array_3->GetObjectItem("raw")->GetRawBytes();

    scene_array_3 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_3",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      bytes);
  } 

  void SceneBroker::DestroySceneInstances()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_instances);
    scene_instances.reset();
  }

  void SceneBroker::DestroySceneBuffer0()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffer_0);
    scene_buffer_0.reset();
  }

  void SceneBroker::DestroySceneBuffer1()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffer_1);
    scene_buffer_1.reset();
  }

  void SceneBroker::DestroySceneBuffer2()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffer_2);
    scene_buffer_2.reset();
  }

  void SceneBroker::DestroySceneBuffer3()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffer_3);
    scene_buffer_3.reset();
  }

  void SceneBroker::DestroySceneArray0()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_array_0);
    scene_array_0.reset();
  }

  void SceneBroker::DestroySceneArray1()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_array_1);
    scene_array_1.reset();
  }

  void SceneBroker::DestroySceneArray2()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_array_2);
    scene_array_2.reset();
  }

  void SceneBroker::DestroySceneArray3()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_array_3);
    scene_array_3.reset();
  }

  void SceneBroker::Initialize()
  {}

  void SceneBroker::Use()
  {}

  void SceneBroker::Discard()
  {}

  SceneBroker::SceneBroker(Wrap& wrap)
    : Broker("scene_broker", wrap)
  {
    const auto& util = wrap.GetUtil();

    prop_scene = util->GetStorage()->GetTree()->GetObjectItem("scene");
    {
      prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);

      prop_buffer_0 = prop_scene->GetObjectItem("buffer_0");
      prop_buffer_1 = prop_scene->GetObjectItem("buffer_1");
      prop_buffer_2 = prop_scene->GetObjectItem("buffer_2");
      prop_buffer_3 = prop_scene->GetObjectItem("buffer_3");

      prop_array_0 = prop_scene->GetObjectItem("array_0");
      prop_array_1 = prop_scene->GetObjectItem("array_1");
      prop_array_2 = prop_scene->GetObjectItem("array_2");
      prop_array_3 = prop_scene->GetObjectItem("array_3");
    }

    CreateSceneInstances();

    CreateSceneBuffer0();
    CreateSceneBuffer1();
    //CreateSceneBuffers2();
    //CreateSceneBuffers3();

    CreateSceneArray0();
    CreateSceneArray1();
    CreateSceneArray2();
    CreateSceneArray3();
  }

  SceneBroker::~SceneBroker()
  {
    prop_scene.reset();

    prop_instances.reset();

    prop_buffer_0.reset();
    prop_buffer_1.reset();
    prop_buffer_2.reset();
    prop_buffer_3.reset();

    prop_array_0.reset();
    prop_array_1.reset();
    prop_array_2.reset();
    prop_array_3.reset();
  }
}