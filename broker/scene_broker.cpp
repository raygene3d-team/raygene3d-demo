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
  //void SceneBroker::CreateSceneInstances()
  //{
  //  const auto [data, count] = prop_instances->GetRawItems<Instance>(0);
  //  const auto stride = sizeof(Instance);

  //  scene_instances = wrap.GetCore()->GetDevice()->CreateResource("scene_instances",
  //    Resource::BufferDesc
  //    {
  //      Usage(USAGE_CONSTANT_DATA),
  //      stride,
  //      count,
  //    },
  //    Resource::Hint(Resource::Hint::HINT_UNKNOWN),
  //    { reinterpret_cast<const uint8_t*>(data), stride * count }
  //    );
  //}

  void SceneBroker::CreateSceneBufferVert()
  {
    const auto [data, count] = prop_buffer_vert->GetObjectItem("binary")->GetRawItems<Vertex>(0);
    const auto stride = sizeof(Vertex);

    scene_buffer_vert = wrap.GetCore()->GetDevice()->CreateResource("scene_buffer_vert",
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

  void SceneBroker::CreateSceneBufferTrng()
  {
    const auto [data, count] = prop_buffer_trng->GetObjectItem("binary")->GetRawItems<Triangle>(0);
    const auto stride = sizeof(Triangle);

    scene_buffer_trng = wrap.GetCore()->GetDevice()->CreateResource("scene_buffer_trng",
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

  void SceneBroker::CreateSceneBufferInst()
  {
    const auto [data, count] = prop_buffer_inst->GetObjectItem("binary")->GetRawItems<Instance>(0);
    const auto stride = sizeof(Triangle);

    scene_buffer_inst = wrap.GetCore()->GetDevice()->CreateResource("scene_buffer_inst",
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

  void SceneBroker::CreateSceneArrayAAAM()
  {
    const auto format = Format(prop_array_aaam->GetObjectItem("format")->GetUint());
    const auto layers = prop_array_aaam->GetObjectItem("layers")->GetUint();
    const auto levels = prop_array_aaam->GetObjectItem("levels")->GetUint();
    const auto size_x = prop_array_aaam->GetObjectItem("size_x")->GetUint();
    const auto size_y = prop_array_aaam->GetObjectItem("size_y")->GetUint();
    const auto bytes = prop_array_aaam->GetObjectItem("binary")->GetRawBytes();

    scene_array_aaam = wrap.GetCore()->GetDevice()->CreateResource("scene_array_aaam",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        levels,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      bytes);
  }

  void SceneBroker::CreateSceneArraySNNO()
  {
    const auto format = Format(prop_array_snno->GetObjectItem("format")->GetUint());
    const auto layers = prop_array_snno->GetObjectItem("layers")->GetUint();
    const auto levels = prop_array_snno->GetObjectItem("levels")->GetUint();
    const auto size_x = prop_array_snno->GetObjectItem("size_x")->GetUint();
    const auto size_y = prop_array_snno->GetObjectItem("size_y")->GetUint();
    const auto bytes = prop_array_snno->GetObjectItem("binary")->GetRawBytes();

    scene_array_snno = wrap.GetCore()->GetDevice()->CreateResource("scene_array_snno",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        levels,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      bytes);
  }

  void SceneBroker::CreateSceneArrayEEET()
  {
    const auto format = Format(prop_array_eeet->GetObjectItem("format")->GetUint());
    const auto layers = prop_array_eeet->GetObjectItem("layers")->GetUint();
    const auto levels = prop_array_eeet->GetObjectItem("levels")->GetUint();
    const auto size_x = prop_array_eeet->GetObjectItem("size_x")->GetUint();
    const auto size_y = prop_array_eeet->GetObjectItem("size_y")->GetUint();
    const auto bytes = prop_array_eeet->GetObjectItem("binary")->GetRawBytes();

    scene_array_eeet = wrap.GetCore()->GetDevice()->CreateResource("scene_array_eeet",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        levels,
        layers,
        format,
        size_x,
        size_y,
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      bytes);
  }

  //void SceneBroker::CreateSceneArray3()
  //{
  //  const auto format = Format(prop_array_3->GetObjectItem("format")->GetUint());
  //  const auto layers = prop_array_3->GetObjectItem("layers")->GetUint();
  //  const auto levels = prop_array_3->GetObjectItem("levels")->GetUint();
  //  const auto size_x = prop_array_3->GetObjectItem("size_x")->GetUint();
  //  const auto size_y = prop_array_3->GetObjectItem("size_y")->GetUint();
  //  const auto bytes = prop_array_3->GetObjectItem("raw")->GetRawBytes();

  //  scene_array_3 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_3",
  //    Resource::Tex2DDesc
  //    {
  //      Usage(USAGE_SHADER_RESOURCE),
  //      levels,
  //      layers,
  //      format,
  //      size_x,
  //      size_y,
  //    },
  //    Resource::Hint(Resource::HINT_LAYERED_IMAGE),
  //    bytes);
  //} 

  //void SceneBroker::DestroySceneInstances()
  //{
  //  wrap.GetCore()->GetDevice()->DestroyResource(scene_instances);
  //  scene_instances.reset();
  //}

  void SceneBroker::DestroySceneBufferVert()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffer_vert);
    scene_buffer_vert.reset();
  }

  void SceneBroker::DestroySceneBufferTrng()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffer_trng);
    scene_buffer_trng.reset();
  }

  void SceneBroker::DestroySceneBufferInst()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffer_inst);
    scene_buffer_inst.reset();
  }

  //void SceneBroker::DestroySceneBuffer3()
  //{
  //  wrap.GetCore()->GetDevice()->DestroyResource(scene_buffer_3);
  //  scene_buffer_3.reset();
  //}

  void SceneBroker::DestroySceneArrayAAAM()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_array_aaam);
    scene_array_aaam.reset();
  }

  void SceneBroker::DestroySceneArraySNNO()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_array_snno);
    scene_array_snno.reset();
  }

  void SceneBroker::DestroySceneArrayEEET()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_array_eeet);
    scene_array_eeet.reset();
  }

  //void SceneBroker::DestroySceneArray3()
  //{
  //  wrap.GetCore()->GetDevice()->DestroyResource(scene_array_3);
  //  scene_array_3.reset();
  //}

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
      //prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);

      prop_buffer_vert = prop_scene->GetObjectItem("buffer_vert");
      prop_buffer_trng = prop_scene->GetObjectItem("buffer_trng");
      prop_buffer_inst = prop_scene->GetObjectItem("buffer_inst");
      //prop_buffer_3 = prop_scene->GetObjectItem("buffer_3");

      prop_array_aaam = prop_scene->GetObjectItem("array_aaam");
      prop_array_snno = prop_scene->GetObjectItem("array_snno");
      prop_array_eeet = prop_scene->GetObjectItem("array_eeet");
      //prop_array_3 = prop_scene->GetObjectItem("array_3");
    }

    //CreateSceneInstances();

    CreateSceneBufferVert();
    CreateSceneBufferTrng();
    CreateSceneBufferInst();
    //CreateSceneBuffers3();

    CreateSceneArrayAAAM();
    CreateSceneArraySNNO();
    CreateSceneArrayEEET();
    //CreateSceneArray3();
  }

  SceneBroker::~SceneBroker()
  {
    prop_scene.reset();

    prop_buffer_vert.reset();
    prop_buffer_trng.reset();
    prop_buffer_inst.reset();

    prop_array_aaam.reset();
    prop_array_snno.reset();
    prop_array_eeet.reset();
  }
}