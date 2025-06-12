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

  void SceneBroker::CreateSceneBuffers0()
  {
    const auto [data, count] = prop_buffers0->GetRawTyped<Vertex>(0);
    std::pair<const uint8_t*, size_t> interops[] = {
      { reinterpret_cast<const uint8_t*>(data), sizeof(Vertex) * count },
    };

    scene_buffers0 = wrap.GetCore()->GetDevice()->CreateResource("scene_buffers0",
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

  void SceneBroker::CreateSceneBuffers1()
  {
    const auto [data, count] = prop_buffers1->GetRawTyped<Triangle>(0);
    std::pair<const uint8_t*, size_t> interops[] = {
      { reinterpret_cast<const uint8_t*>(data), sizeof(Triangle) * count },
    };

    scene_buffers1 = wrap.GetCore()->GetDevice()->CreateResource("prop_buffers1",
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

  void SceneBroker::CreateSceneTextures0()
  {
    const auto& layers = prop_textures0->GetObjectItem("layers");
    const auto& mipmap = prop_textures0->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures0->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures0->GetObjectItem("extent_y");
    const auto& raws = prop_textures0->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
    for (size_t i = 0u; i < interops.size(); ++i)
    {
      interops[i] = raws->GetArrayItem(i)->GetRawBytes();
    }

    scene_textures0 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures0",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R8G8B8A8_SRGB,
        extent_x->GetUint(),
        extent_y->GetUint(),
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), interops.size() }
      );
  }

  void SceneBroker::CreateSceneTextures1()
  {
    const auto& layers = prop_textures1->GetObjectItem("layers");
    const auto& mipmap = prop_textures1->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures1->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures1->GetObjectItem("extent_y");
    const auto& raws = prop_textures1->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
    for (size_t i = 0u; i < interops.size(); ++i)
    {
      interops[i] = raws->GetArrayItem(i)->GetRawBytes(0);
    }

    scene_textures1 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures1",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R8G8B8A8_SRGB,
        extent_x->GetUint(),
        extent_y->GetUint(),
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), interops.size() }
      );
  }

  void SceneBroker::CreateSceneTextures2()
  {
    const auto& layers = prop_textures2->GetObjectItem("layers");
    const auto& mipmap = prop_textures2->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures2->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures2->GetObjectItem("extent_y");
    const auto& raws = prop_textures2->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
    for (size_t i = 0u; i < interops.size(); ++i)
    {
      interops[i] = raws->GetArrayItem(i)->GetRawBytes(0);
    }

    scene_textures2 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures2",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R8G8B8A8_SRGB,
        extent_x->GetUint(),
        extent_y->GetUint(),
      },
      Resource::Hint(Resource::HINT_LAYERED_IMAGE),
      { interops.data(), interops.size() }
      );
  }

  void SceneBroker::CreateSceneTextures3()
  {
    const auto& layers = prop_textures3->GetObjectItem("layers");
    const auto& mipmap = prop_textures3->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures3->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures3->GetObjectItem("extent_y");
    const auto& raws = prop_textures3->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const uint8_t*, size_t>>(raws->GetArraySize());
    for (size_t i = 0u; i < interops.size(); ++i)
    {
      interops[i] = raws->GetArrayItem(i)->GetRawBytes(0);
    }

    scene_textures3 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures3",
      Resource::Tex2DDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        mipmap->GetUint(),
        layers->GetUint(),
        FORMAT_R8G8B8A8_UNORM,
        extent_x->GetUint(),
        extent_y->GetUint(),
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

  void SceneBroker::DestroySceneBuffers0()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffers0);
    scene_buffers0.reset();
  }

  void SceneBroker::DestroySceneBuffers1()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffers1);
    scene_buffers1.reset();
  }

  void SceneBroker::DestroySceneBuffers2()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffers2);
    scene_buffers2.reset();
  }

  void SceneBroker::DestroySceneBuffers3()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_buffers3);
    scene_buffers3.reset();
  }

  void SceneBroker::DestroySceneTextures0()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures0);
    scene_textures0.reset();
  }

  void SceneBroker::DestroySceneTextures1()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures1);
    scene_textures1.reset();
  }

  void SceneBroker::DestroySceneTextures2()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures2);
    scene_textures2.reset();
  }

  void SceneBroker::DestroySceneTextures3()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures3);
    scene_textures3.reset();
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

      prop_buffers0 = prop_scene->GetObjectItem("buffers0")->GetObjectItem("raws")->GetArrayItem(0);
      prop_buffers1 = prop_scene->GetObjectItem("buffers1")->GetObjectItem("raws")->GetArrayItem(0);
      prop_buffers2 = prop_scene->GetObjectItem("buffers2")->GetObjectItem("raws")->GetArrayItem(0);
      prop_buffers3 = prop_scene->GetObjectItem("buffers3")->GetObjectItem("raws")->GetArrayItem(0);

      prop_textures0 = prop_scene->GetObjectItem("textures_0");
      prop_textures1 = prop_scene->GetObjectItem("textures_1");
      prop_textures2 = prop_scene->GetObjectItem("textures_2");
      prop_textures3 = prop_scene->GetObjectItem("textures_3");
    }

    CreateSceneInstances();

    CreateSceneBuffers0();
    CreateSceneBuffers1();
    //CreateSceneBuffers2();
    //CreateSceneBuffers3();

    CreateSceneTextures0();
    CreateSceneTextures1();
    CreateSceneTextures2();
    CreateSceneTextures3();
  }

  SceneBroker::~SceneBroker()
  {
    prop_scene.reset();

    prop_instances.reset();

    prop_buffers0.reset();
    prop_buffers1.reset();
    prop_buffers2.reset();
    prop_buffers3.reset();

    prop_textures0.reset();
    prop_textures1.reset();
    prop_textures2.reset();
    prop_textures3.reset();
  }
}