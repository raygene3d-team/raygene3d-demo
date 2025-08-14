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
    const auto [data, count] = prop_instances->GetRawTyped<Instance>(0);
    std::pair<const uint8_t*, size_t> interops[] = {
      { reinterpret_cast<const uint8_t*>(data), sizeof(Instance) * count },
    };

    scene_instances = wrap.GetCore()->GetDevice()->CreateResource("scene_instances",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        sizeof(Instance),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, std::size(interops) }
      );
  }

  void SceneBroker::CreateSceneBuffer0()
  {
    const auto [data, count] = prop_buffer_0->GetRawTyped<Vertex>(0);
    std::pair<const uint8_t*, size_t> interops[] = {
      { reinterpret_cast<const uint8_t*>(data), sizeof(Vertex) * count },
    };

    scene_buffer_0 = wrap.GetCore()->GetDevice()->CreateResource("scene_buffer_0",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        sizeof(Vertex),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, std::size(interops) }
      );
  }

  void SceneBroker::CreateSceneBuffer1()
  {
    const auto [data, count] = prop_buffer_1->GetRawTyped<Triangle>(0);
    std::pair<const uint8_t*, size_t> interops[] = {
      { reinterpret_cast<const uint8_t*>(data), sizeof(Triangle) * count },
    };

    scene_buffer_1 = wrap.GetCore()->GetDevice()->CreateResource("scene_buffer_1",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        sizeof(Triangle),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, std::size(interops) }
      );
  }

  void SceneBroker::CreateSceneArray0()
  {
    const auto& format = prop_array_0->GetObjectItem("format");
    const auto& layers = prop_array_0->GetObjectItem("layers");
    const auto& mipmap = prop_array_0->GetObjectItem("mipmap");
    const auto& size_x = prop_array_0->GetObjectItem("size_x");
    const auto& size_y = prop_array_0->GetObjectItem("size_y");
    const auto& raw = prop_array_0->GetObjectItem("raw");

    auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
    for (size_t i = 0u; i < interops.size(); ++i)
    {
      interops[i] = raws->GetArrayItem(i)->GetRawBytes();
    }

    scene_array_0 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_0",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R8G8B8A8_SRGB,
        size_x->GetUint(),
        size_y->GetUint(),
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), interops.size() }
      );
  }

  void SceneBroker::CreateSceneArray1()
  {
    const auto& format = prop_array_1->GetObjectItem("format");
    const auto& layers = prop_array_1->GetObjectItem("layers");
    const auto& mipmap = prop_array_1->GetObjectItem("mipmap");
    const auto& size_x = prop_array_1->GetObjectItem("size_x");
    const auto& size_y = prop_array_1->GetObjectItem("size_y");
    const auto& raw = prop_array_1->GetObjectItem("raw");

    auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
    for (size_t i = 0u; i < interops.size(); ++i)
    {
      interops[i] = raws->GetArrayItem(i)->GetRawBytes();
    }

    scene_array_1 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_1",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R8G8B8A8_UNORM,
        size_x->GetUint(),
        size_y->GetUint(),
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), interops.size() }
      );
  }

  void SceneBroker::CreateSceneArray2()
  {
    const auto& format = prop_array_2->GetObjectItem("format");
    const auto& layers = prop_array_2->GetObjectItem("layers");
    const auto& mipmap = prop_array_2->GetObjectItem("mipmap");
    const auto& size_x = prop_array_2->GetObjectItem("size_x");
    const auto& size_y = prop_array_2->GetObjectItem("size_y");
    const auto& raw = prop_array_2->GetObjectItem("raw");

    auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
    for (size_t i = 0u; i < interops.size(); ++i)
    {
      interops[i] = raws->GetArrayItem(i)->GetRawBytes();
    }

    scene_array_2 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_2",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R8G8B8A8_UNORM,
        size_x->GetUint(),
        size_y->GetUint(),
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), interops.size() }
      );
  }

  void SceneBroker::CreateSceneArray3()
  {
    const auto& format = prop_array_3->GetObjectItem("format");
    const auto& layers = prop_array_3->GetObjectItem("layers");
    const auto& mipmap = prop_array_3->GetObjectItem("mipmap");
    const auto& size_x = prop_array_3->GetObjectItem("size_x");
    const auto& size_y = prop_array_3->GetObjectItem("size_y");
    const auto& raw = prop_array_3->GetObjectItem("raw");

    auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
    for (size_t i = 0u; i < interops.size(); ++i)
    {
      interops[i] = raws->GetArrayItem(i)->GetRawBytes();
    }

    scene_array_3 = wrap.GetCore()->GetDevice()->CreateResource("scene_array_3",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R8G8B8A8_UNORM,
        size_x->GetUint(),
        size_y->GetUint(),
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), interops.size() }
      );
  }

  //void SceneBroker::CreateSceneTextures4()
  //{
  //  const auto& layers = prop_textures4->GetObjectItem("layers");
  //  const auto& mipmap = prop_textures4->GetObjectItem("mipmap");
  //  const auto& extent_x = prop_textures4->GetObjectItem("extent_x");
  //  const auto& extent_y = prop_textures4->GetObjectItem("extent_y");
  //  const auto& raws = prop_textures4->GetObjectItem("raws");

  //  auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
  //  for (size_t i = 0u; i < interops.size(); ++i)
  //  {
  //    interops[i] = raws->GetArrayItem(i)->GetRawBytes(0);
  //  }

  //  scene_textures4 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures4",
  //    Resource::Tex2DDesc
  //    {
  //      Usage(USAGE_SHADER_RESOURCE),
  //      mipmap->GetUint(),
  //      layers->GetUint(),
  //      FORMAT_R8G8B8A8_SRGB,
  //      extent_x->GetUint(),
  //      extent_y->GetUint(),
  //    },
  //    Resource::Hint(Resource::HINT_LAYERED_IMAGE),
  //    { interops.data(), interops.size() }
  //    );
  //}

  //void SceneBroker::CreateSceneTextures5()
  //{
  //  const auto& layers = prop_textures5->GetObjectItem("layers");
  //  const auto& mipmap = prop_textures5->GetObjectItem("mipmap");
  //  const auto& extent_x = prop_textures5->GetObjectItem("extent_x");
  //  const auto& extent_y = prop_textures5->GetObjectItem("extent_y");
  //  const auto& raws = prop_textures5->GetObjectItem("raws");

  //  auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
  //  for (size_t i = 0u; i < interops.size(); ++i)
  //  {
  //    interops[i] = raws->GetArrayItem(i)->GetRawBytes();
  //  }

  //  scene_textures5 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures5",
  //    Resource::Tex2DDesc
  //    {
  //      Usage(USAGE_SHADER_RESOURCE),
  //      mipmap->GetUint(),
  //      layers->GetUint(),
  //      FORMAT_R8G8B8A8_UNORM,
  //      extent_x->GetUint(),
  //      extent_y->GetUint(),
  //    },
  //    Resource::Hint(Resource::HINT_LAYERED_IMAGE),
  //    { interops.data(), interops.size() }
  //    );
  //}

  //void SceneBroker::CreateSceneTextures6()
  //{
  //  const auto& layers = prop_textures6->GetObjectItem("layers");
  //  const auto& mipmap = prop_textures6->GetObjectItem("mipmap");
  //  const auto& extent_x = prop_textures6->GetObjectItem("extent_x");
  //  const auto& extent_y = prop_textures6->GetObjectItem("extent_y");
  //  const auto& raws = prop_textures6->GetObjectItem("raws");

  //  auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
  //  for (size_t i = 0u; i < interops.size(); ++i)
  //  {
  //    interops[i] = raws->GetArrayItem(i)->GetRawBytes(0);
  //  }

  //  scene_textures6 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures6",
  //    Resource::Tex2DDesc
  //    {
  //      Usage(USAGE_SHADER_RESOURCE),
  //      mipmap->GetUint(),
  //      layers->GetUint(),
  //      FORMAT_R8G8B8A8_UNORM,
  //      extent_x->GetUint(),
  //      extent_y->GetUint(),
  //    },
  //    Resource::Hint(Resource::HINT_LAYERED_IMAGE),
  //    { interops.data(), interops.size() }
  //    );
  //}

  //void SceneBroker::CreateSceneTextures7()
  //{
  //  const auto& layers = prop_textures7->GetObjectItem("layers");
  //  const auto& mipmap = prop_textures7->GetObjectItem("mipmap");
  //  const auto& extent_x = prop_textures7->GetObjectItem("extent_x");
  //  const auto& extent_y = prop_textures7->GetObjectItem("extent_y");
  //  const auto& raws = prop_textures7->GetObjectItem("raws");

  //  auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
  //  for (size_t i = 0u; i < interops.size(); ++i)
  //  {
  //    interops[i] = raws->GetArrayItem(i)->GetRawBytes(0);
  //  }

  //  scene_textures7 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures7",
  //    Resource::Tex2DDesc
  //    {
  //      Usage(USAGE_SHADER_RESOURCE),
  //      mipmap->GetUint(),
  //      layers->GetUint(),
  //      FORMAT_R8G8B8A8_UNORM,
  //      extent_x->GetUint(),
  //      extent_y->GetUint(),
  //    },
  //    Resource::Hint(Resource::HINT_LAYERED_IMAGE),
  //    { interops.data(), interops.size() }
  //    );
  //}

 

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