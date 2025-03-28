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
    const auto [data, count] = prop_instances->GetTypedBytes<Instance>(0);
    std::pair<const void*, uint32_t> interops[] = {
      { data, uint32_t(sizeof(Instance)) * count },
    };

    scene_instances = wrap.GetCore()->GetDevice()->CreateResource("scene_instances",
      Resource::BufferDesc
      {
        Usage(USAGE_CONSTANT_DATA),
        uint32_t(sizeof(Instance)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
      );
  }

  void SceneBroker::CreateSceneTriangles()
  {
    const auto [data, count] = prop_triangles->GetTypedBytes<Triangle>(0);
    std::pair<const void*, uint32_t> interops[] = {
      { data, uint32_t(sizeof(Triangle)) * count },
    };

    scene_triangles = wrap.GetCore()->GetDevice()->CreateResource("scene_triangles",
      Resource::BufferDesc
      {
        Usage(USAGE_INDEX_ARRAY),
        uint32_t(sizeof(Triangle)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
      );
  }

  void SceneBroker::CreateSceneVertices()
  {
    const auto [data, count] = prop_vertices->GetTypedBytes<Vertex>(0);
    std::pair<const void*, uint32_t> interops[] = {
      { data, uint32_t(sizeof(Vertex)) * count },
    };

    scene_vertices = wrap.GetCore()->GetDevice()->CreateResource("scene_vertices",
      Resource::BufferDesc
      {
        Usage(USAGE_VERTEX_ARRAY),
        uint32_t(sizeof(Vertex)),
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { interops, uint32_t(std::size(interops)) }
      );
  }

  void SceneBroker::CreateSceneTextures0()
  {
    const auto& layers = prop_textures0->GetObjectItem("layers");
    const auto& mipmap = prop_textures0->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures0->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures0->GetObjectItem("extent_y");
    const auto& raws = prop_textures0->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
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
      { interops.data(), uint32_t(interops.size()) }
      );
  }

  void SceneBroker::CreateSceneTextures1()
  {
    const auto& layers = prop_textures1->GetObjectItem("layers");
    const auto& mipmap = prop_textures1->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures1->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures1->GetObjectItem("extent_y");
    const auto& raws = prop_textures1->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
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
      { interops.data(), uint32_t(interops.size()) }
      );
  }

  void SceneBroker::CreateSceneTextures2()
  {
    const auto& layers = prop_textures2->GetObjectItem("layers");
    const auto& mipmap = prop_textures2->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures2->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures2->GetObjectItem("extent_y");
    const auto& raws = prop_textures2->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
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
      { interops.data(), uint32_t(interops.size()) }
      );
  }

  void SceneBroker::CreateSceneTextures3()
  {
    const auto& layers = prop_textures3->GetObjectItem("layers");
    const auto& mipmap = prop_textures3->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures3->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures3->GetObjectItem("extent_y");
    const auto& raws = prop_textures3->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
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
      { interops.data(), uint32_t(interops.size()) }
      );
  }

  void SceneBroker::CreateSceneTextures4()
  {
    const auto& layers = prop_textures4->GetObjectItem("layers");
    const auto& mipmap = prop_textures4->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures4->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures4->GetObjectItem("extent_y");
    const auto& raws = prop_textures4->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
    }

    scene_textures4 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures4",
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
      { interops.data(), uint32_t(interops.size()) }
      );
  }

  void SceneBroker::CreateSceneTextures5()
  {
    const auto& layers = prop_textures5->GetObjectItem("layers");
    const auto& mipmap = prop_textures5->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures5->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures5->GetObjectItem("extent_y");
    const auto& raws = prop_textures5->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
    }

    scene_textures5 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures5",
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
      { interops.data(), uint32_t(interops.size()) }
      );
  }

  void SceneBroker::CreateSceneTextures6()
  {
    const auto& layers = prop_textures6->GetObjectItem("layers");
    const auto& mipmap = prop_textures6->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures6->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures6->GetObjectItem("extent_y");
    const auto& raws = prop_textures6->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
    }

    scene_textures6 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures6",
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
      { interops.data(), uint32_t(interops.size()) }
      );
  }

  void SceneBroker::CreateSceneTextures7()
  {
    const auto& layers = prop_textures7->GetObjectItem("layers");
    const auto& mipmap = prop_textures7->GetObjectItem("mipmap");
    const auto& extent_x = prop_textures7->GetObjectItem("extent_x");
    const auto& extent_y = prop_textures7->GetObjectItem("extent_y");
    const auto& raws = prop_textures7->GetObjectItem("raws");

    auto interops = std::vector<std::pair<const void*, uint32_t>>(raws->GetArraySize());
    for (auto i = 0u; i < uint32_t(interops.size()); ++i)
    {
      const auto& raw = raws->GetArrayItem(i);
      interops[i] = raw->GetRawBytes(0);
    }

    scene_textures7 = wrap.GetCore()->GetDevice()->CreateResource("scene_textures7",
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
      { interops.data(), uint32_t(interops.size()) }
      );
  }

 

  void SceneBroker::DestroySceneInstances()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_instances);
    scene_instances.reset();
  }

  void SceneBroker::DestroySceneTriangles()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_triangles);
    scene_triangles.reset();
  }

  void SceneBroker::DestroySceneVertices()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_vertices);
    scene_vertices.reset();
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

  void SceneBroker::DestroySceneTextures4()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures4);
    scene_textures4.reset();
  }

  void SceneBroker::DestroySceneTextures5()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures5);
    scene_textures5.reset();
  }

  void SceneBroker::DestroySceneTextures6()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures6);
    scene_textures6.reset();
  }

  void SceneBroker::DestroySceneTextures7()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(scene_textures7);
    scene_textures7.reset();
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
      prop_triangles = prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->GetArrayItem(0);
      prop_vertices = prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->GetArrayItem(0);

      prop_textures0 = prop_scene->GetObjectItem("textures_0");
      prop_textures1 = prop_scene->GetObjectItem("textures_1");
      prop_textures2 = prop_scene->GetObjectItem("textures_2");
      prop_textures3 = prop_scene->GetObjectItem("textures_3");
      prop_textures4 = prop_scene->GetObjectItem("textures_4");
      prop_textures5 = prop_scene->GetObjectItem("textures_5");
      prop_textures6 = prop_scene->GetObjectItem("textures_6");
      prop_textures7 = prop_scene->GetObjectItem("textures_7");
    }

    CreateSceneInstances();
    CreateSceneTriangles();
    CreateSceneVertices();

    CreateSceneTextures0();
    CreateSceneTextures1();
    CreateSceneTextures2();
    CreateSceneTextures3();
    CreateSceneTextures4();
    CreateSceneTextures5();
    CreateSceneTextures6();
    CreateSceneTextures7();
  }

  SceneBroker::~SceneBroker()
  {
    prop_scene.reset();

    prop_instances.reset();
    prop_triangles.reset();
    prop_vertices.reset();

    prop_textures0.reset();
    prop_textures1.reset();
    prop_textures2.reset();
    prop_textures3.reset();
    prop_textures4.reset();
    prop_textures5.reset();
    prop_textures6.reset();
    prop_textures7.reset();

    //DestroySceneTextures0();
    //DestroySceneTextures1();
    //DestroySceneTextures2();
    //DestroySceneTextures3();
    //DestroySceneTextures4();
    //DestroySceneTextures5();
    //DestroySceneTextures6();
    //DestroySceneTextures7();

    //DestroySceneInstances();
    //DestroySceneTriangles();
    //DestroySceneVertices();
  }
}