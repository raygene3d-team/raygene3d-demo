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
#include "mikktspace_broker.h"

#include <mikktspace/mikktspace.h>


namespace RayGene3D
{
  std::vector<Vertex> CalculateTangents(const std::pair<const Triangle*, uint32_t>& triangles, 
    const std::pair<const Vertex*, uint32_t>& vertices)
  {
    auto result = std::vector<Vertex>(vertices.first, vertices.first + vertices.second);

    struct SMikkTSpaceUserData
    {
      std::pair<const Triangle*, uint32_t> prims;
      std::pair<const Vertex*, uint32_t> verts;
      std::pair<Vertex*, uint32_t> tangent_verts;
    } data{ triangles, vertices, {result.data(), uint32_t(result.size())}};

    SMikkTSpaceInterface input = { 0 };
    input.m_getNumFaces = [](const SMikkTSpaceContext* ctx)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        return int32_t(data->prims.second);
      };

    input.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* ctx, const int iFace)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        return 3;
      };

    //input.m_getPosition = &GetPositionCb;
    //input.m_getNormal = &GetNormalCb;
    //input.m_getTexCoord = &GetTexCoordCb;
    //input.m_setTSpaceBasic = &SetTspaceBasicCb;
    //input.m_setTSpace = NULL;


    input.m_getPosition = [](const SMikkTSpaceContext* ctx, float fvPosOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& pos = data->verts.first[data->prims.first[iFace].idx[iVert]].pos;
        fvPosOut[0] = pos.x;
        fvPosOut[1] = pos.y;
        fvPosOut[2] = pos.z;
      };

    input.m_getNormal = [](const SMikkTSpaceContext* ctx, float fvNormOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& nrm = data->verts.first[data->prims.first[iFace].idx[iVert]].nrm;
        fvNormOut[0] = nrm.x;
        fvNormOut[1] = nrm.y;
        fvNormOut[2] = nrm.z;
      };

    input.m_getTexCoord = [](const SMikkTSpaceContext* ctx, float fvTexcOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& tc0 = data->verts.first[data->prims.first[iFace].idx[iVert]].tc0;
        fvTexcOut[0] = tc0.x;
        fvTexcOut[1] = tc0.y;
      };

    input.m_setTSpaceBasic = [](const SMikkTSpaceContext* ctx, const float fvTangent[], float fSign, int iFace, int iVert)
      {
        auto data = reinterpret_cast<SMikkTSpaceUserData*>(ctx->m_pUserData);
        auto& tng = data->tangent_verts.first[data->prims.first[iFace].idx[iVert]].tng;
        tng.x = fvTangent[0];
        tng.y = fvTangent[1];
        tng.z = fvTangent[2];
        auto& sgn = data->tangent_verts.first[data->prims.first[iFace].idx[iVert]].sgn;
        sgn = fSign;
      };

    SMikkTSpaceContext context;
    context.m_pInterface = &input;
    context.m_pUserData = &data;

    BLAST_ASSERT(1 == genTangSpaceDefault(&context));

    return result;
  }

  void MikktspaceBroker::Initialize()
  {
    const auto tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_scene = tree->GetObjectItem("scene");
    {
      prop_instances = prop_scene->GetObjectItem("instances")->GetObjectItem("raws")->GetArrayItem(0);
      prop_triangles = prop_scene->GetObjectItem("triangles")->GetObjectItem("raws")->GetArrayItem(0);
      prop_vertices = prop_scene->GetObjectItem("vertices")->GetObjectItem("raws")->GetArrayItem(0);
    }
  }

  void MikktspaceBroker::Use()
  {
    const auto [ins_array, ins_count] = prop_instances->GetRawTyped<Instance>(0);
    const auto [trg_array, trg_count] = prop_triangles->GetRawTyped<Triangle>(0);
    const auto [vrt_array, vrt_count] = prop_vertices->GetRawTyped<Vertex>(0);

    for (uint32_t i = 0; i < ins_count; ++i)
    {
      const auto trg_offset = ins_array[i].prim_offset;
      const auto trg_items = trg_array + trg_offset;
      const auto trg_count = ins_array[i].prim_count;

      const auto vrt_offset = ins_array[i].vert_offset;
      const auto vrt_items = vrt_array + vrt_offset;
      const auto vrt_count = ins_array[i].vert_count;

      auto result = std::vector<Vertex>(vrt_items, vrt_items + vrt_count);

      struct SMikkTSpaceUserData
      {
        std::pair<const Triangle*, uint32_t> prims;
        std::pair<const Vertex*, uint32_t> verts;
        std::pair<Vertex*, uint32_t> tangent_verts;
      } data{ {trg_items, trg_count}, {vrt_items, vrt_count}, {result.data(), uint32_t(result.size())} };

      SMikkTSpaceInterface input = { 0 };
      input.m_getNumFaces = [](const SMikkTSpaceContext* ctx)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        return int32_t(data->prims.second);
      };

      input.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* ctx, const int iFace)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        return 3;
      };

      //input.m_getPosition = &GetPositionCb;
      //input.m_getNormal = &GetNormalCb;
      //input.m_getTexCoord = &GetTexCoordCb;
      //input.m_setTSpaceBasic = &SetTspaceBasicCb;
      //input.m_setTSpace = NULL;


      input.m_getPosition = [](const SMikkTSpaceContext* ctx, float fvPosOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& pos = data->verts.first[data->prims.first[iFace].idx[iVert]].pos;
        fvPosOut[0] = pos.x;
        fvPosOut[1] = pos.y;
        fvPosOut[2] = pos.z;
      };

      input.m_getNormal = [](const SMikkTSpaceContext* ctx, float fvNormOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& nrm = data->verts.first[data->prims.first[iFace].idx[iVert]].nrm;
        fvNormOut[0] = nrm.x;
        fvNormOut[1] = nrm.y;
        fvNormOut[2] = nrm.z;
      };

      input.m_getTexCoord = [](const SMikkTSpaceContext* ctx, float fvTexcOut[], int iFace, int iVert)
      {
        const auto data = reinterpret_cast<const SMikkTSpaceUserData*>(ctx->m_pUserData);
        const auto& tc0 = data->verts.first[data->prims.first[iFace].idx[iVert]].tc0;
        fvTexcOut[0] = tc0.x;
        fvTexcOut[1] = tc0.y;
      };

      input.m_setTSpaceBasic = [](const SMikkTSpaceContext* ctx, const float fvTangent[], float fSign, int iFace, int iVert)
      {
        auto data = reinterpret_cast<SMikkTSpaceUserData*>(ctx->m_pUserData);
        auto& tng = data->tangent_verts.first[data->prims.first[iFace].idx[iVert]].tng;
        tng.x = fvTangent[0];
        tng.y = fvTangent[1];
        tng.z = fvTangent[2];
        auto& sgn = data->tangent_verts.first[data->prims.first[iFace].idx[iVert]].sgn;
        sgn = fSign;
      };

      SMikkTSpaceContext context;
      context.m_pInterface = &input;
      context.m_pUserData = &data;

      BLAST_ASSERT(1 == genTangSpaceDefault(&context));

      if (uint32_t(result.size()) == vrt_count)
      {
        prop_vertices->SetRawTyped<Vertex>({ result.data(), vrt_count }, vrt_offset);
      }
    }
  }

  void MikktspaceBroker::Discard()
  {
    prop_vertices.reset();
    prop_triangles.reset();
    prop_instances.reset();

    prop_scene.reset();
  }

  MikktspaceBroker::MikktspaceBroker(Wrap& wrap)
    : Broker("mikktspace_broker", wrap)
  {}

  MikktspaceBroker::~MikktspaceBroker()
  {}
}