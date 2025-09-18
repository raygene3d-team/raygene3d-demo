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
    const auto stride = sizeof(Instance);

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

  void SceneBroker::CreateTraceBufferVert()
  {
    const auto [data, count] = prop_buffer_vert->GetObjectItem("binary")->GetRawItems<Vertex>(0);
    const auto stride = sizeof(Vertex);

    trace_buffer_vert = wrap.GetCore()->GetDevice()->CreateResource("trace_buffer_vert",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE | USAGE_RAYTRACING_INPUT),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_ADDRESS_BUFFER),
      { reinterpret_cast<const uint8_t*>(data), count * stride }
      );
  }

  void SceneBroker::CreateTraceBufferTrng()
  {
    const auto [items, count] = prop_buffer_trng->GetObjectItem("binary")->GetRawItems<Triangle>(0);
    const auto stride = sizeof(Triangle);

    trace_buffer_trng = wrap.GetCore()->GetDevice()->CreateResource("trace_buffer_trng",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE | USAGE_RAYTRACING_INPUT),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_ADDRESS_BUFFER),
      { reinterpret_cast<const uint8_t*>(items), count * stride }
      );
  }

  void SceneBroker::CreateTraceBufferMlet()
  {
    const auto [items, count] = prop_buffer_mlet->GetObjectItem("binary")->GetRawItems<Meshlet>(0);
    const auto stride = sizeof(Meshlet);

    trace_buffer_mlet = wrap.GetCore()->GetDevice()->CreateResource("trace_buffer_mlet",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(items), count * stride }
      );
  }

  void SceneBroker::CreateTraceBufferInst()
  {
    const auto [items, count] = prop_buffer_inst->GetObjectItem("binary")->GetRawItems<Instance>(0);
    const auto stride = sizeof(Instance);

    trace_buffer_inst = wrap.GetCore()->GetDevice()->CreateResource("trace_buffer_inst",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(items), count * stride }
      );
  }

  void SceneBroker::CreateTraceBufferTBox()
  {
    const auto [items, count] = prop_buffer_tbox->GetObjectItem("binary")->GetRawItems<Box>(0);
    const auto stride = sizeof(Box);

    trace_buffer_tbox = wrap.GetCore()->GetDevice()->CreateResource("trace_buffer_tbox",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(items), count * stride }
      );
  }

  void SceneBroker::CreateTraceBufferBBox()
  {
    const auto [items, count] = prop_buffer_bbox->GetObjectItem("binary")->GetRawItems<Box>(0);
    const auto stride = sizeof(Box);

    trace_buffer_bbox = wrap.GetCore()->GetDevice()->CreateResource("trace_buffer_bbox",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(items), count * stride }
      );
  }

  void SceneBroker::CreateTraceBufferVIdx()
  {
    const auto [items, count] = prop_buffer_vidx->GetObjectItem("binary")->GetRawItems<uint32_t>(0);
    const auto stride = sizeof(uint32_t);

    trace_buffer_vidx = wrap.GetCore()->GetDevice()->CreateResource("trace_buffer_vidx",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(items), count * stride }
      );
  }

  void SceneBroker::CreateTraceBufferTIdx()
  {
    const auto [items, count] = prop_buffer_tidx->GetObjectItem("binary")->GetRawItems<uint8_t>(0);
    const auto stride = sizeof(uint8_t);

    trace_buffer_tidx = wrap.GetCore()->GetDevice()->CreateResource("trace_buffer_tidx",
      Resource::BufferDesc
      {
        Usage(USAGE_SHADER_RESOURCE),
        stride,
        count,
      },
      Resource::Hint(Resource::Hint::HINT_UNKNOWN),
      { reinterpret_cast<const uint8_t*>(items), count * stride }
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

  void SceneBroker::DestroyTraceBufferVert()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_vert);
    trace_buffer_vert.reset();
  }

  void SceneBroker::DestroyTraceBufferTrng()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_trng);
    trace_buffer_trng.reset();
  }

  void SceneBroker::DestroyTraceBufferMlet()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_mlet);
    trace_buffer_mlet.reset();
  }

  void SceneBroker::DestroyTraceBufferInst()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_inst);
    trace_buffer_inst.reset();
  }

  void SceneBroker::DestroyTraceBufferTBox()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_tbox);
    trace_buffer_tbox.reset();
  }

  void SceneBroker::DestroyTraceBufferBBox()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_bbox);
    trace_buffer_bbox.reset();
  }

  void SceneBroker::DestroyTraceBufferVIdx()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_vidx);
    trace_buffer_vidx.reset();
  }

  void SceneBroker::DestroyTraceBufferTIdx()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_tidx);
    trace_buffer_tidx.reset();
  }

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
      prop_buffer_vert = prop_scene->GetObjectItem("buffer_vert");
      prop_buffer_trng = prop_scene->GetObjectItem("buffer_trng");
      prop_buffer_mlet = prop_scene->GetObjectItem("buffer_mlet");

      prop_buffer_inst = prop_scene->GetObjectItem("buffer_inst");
      prop_buffer_tbox = prop_scene->GetObjectItem("buffer_tbox");
      prop_buffer_bbox = prop_scene->GetObjectItem("buffer_bbox");

      prop_buffer_vidx = prop_scene->GetObjectItem("buffer_vidx");
      prop_buffer_tidx = prop_scene->GetObjectItem("buffer_tidx");

      prop_array_aaam = prop_scene->GetObjectItem("array_aaam");
      prop_array_snno = prop_scene->GetObjectItem("array_snno");
      prop_array_eeet = prop_scene->GetObjectItem("array_eeet");
    }

    CreateSceneBufferVert();
    CreateSceneBufferTrng();
    CreateSceneBufferInst();
    
    CreateTraceBufferVert();
    CreateTraceBufferTrng();
    CreateTraceBufferMlet();

    CreateTraceBufferInst();
    CreateTraceBufferTBox();
    CreateTraceBufferBBox();

    CreateTraceBufferVIdx();
    CreateTraceBufferTIdx();

    CreateSceneArrayAAAM();
    CreateSceneArraySNNO();
    CreateSceneArrayEEET();
  }

  SceneBroker::~SceneBroker()
  {
    prop_scene.reset();

    prop_buffer_vert.reset();
    prop_buffer_trng.reset();
    prop_buffer_mlet.reset();

    prop_buffer_inst.reset();
    prop_buffer_bbox.reset();
    prop_buffer_tbox.reset();

    prop_buffer_vidx.reset();
    prop_buffer_tidx.reset();

    prop_array_aaam.reset();
    prop_array_snno.reset();
    prop_array_eeet.reset();
  }
}