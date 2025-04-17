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

      const auto vrt_offset = ins_array[i].vert_offset;
      auto vrt_items = vrt_array + vrt_offset;
      const auto vrt_count = ins_array[i].vert_count;

      meshopt_optimizeVertexCache((uint32_t*)trg_items, (const uint32_t*)trg_items, trg_count * 3, vrt_count);
     
      meshopt_optimizeOverdraw((uint32_t*)trg_items, (const uint32_t*)trg_items, trg_count * 3, (const float*)vrt_items, vrt_count, sizeof(Vertex), 1.0f);

      meshopt_optimizeVertexFetch((float*)vrt_items, (uint32_t*)trg_items, trg_count * 3, (const float*)vrt_items, vrt_count, sizeof(Vertex));


      const uint32_t vrt_limit = 64;
      const uint32_t trg_limit = 64; // : 124;

      //// note: should be set to 0 unless cone culling is used at runtime!
      //const float cone_weight = flex ? -1.0f : 0.25f;
      //const float split_factor = flex ? 2.0f : 0.0f;

      //// note: input mesh is assumed to be optimized for vertex cache and vertex fetch
      //double start = timestamp();
      auto meshlet_count = meshopt_buildMeshletsBound(trg_count * 3, vrt_limit, trg_limit * 3);
      std::vector<meshopt_Meshlet> meshlets(meshlet_count);
      std::vector<uint32_t> meshlet_vrt(meshlet_count * vrt_limit);
      std::vector<uint8_t> meshlet_trg(meshlet_count * trg_limit * 3);

      //if (scan)
      //  meshlets.resize(meshopt_buildMeshletsScan(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size(), max_vertices, max_triangles));
      //else if (flex)
      //  meshlets.resize(meshopt_buildMeshletsFlex(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), max_vertices, min_triangles, max_triangles, cone_weight, split_factor));
      //else // note: equivalent to the call of buildMeshletsFlex() with non-negative cone_weight and split_factor = 0
      meshlets.resize(meshopt_buildMeshlets(&meshlets[0], &meshlet_vrt[0], &meshlet_trg[0], 
        (const uint32_t*)trg_items, trg_count * 3, (const float*)vrt_items, vrt_count, sizeof(Vertex), vrt_limit, trg_limit, 0.0f));

      for (size_t i = 0; i < meshlets.size(); ++i)
      {
        meshopt_optimizeMeshlet(&meshlet_vrt[meshlets[i].vertex_offset], &meshlet_trg[meshlets[i].triangle_offset],
          meshlets[i].triangle_count, meshlets[i].vertex_count);
      }

      //if (meshlets.size())
      //{
      //  const meshopt_Meshlet& last = meshlets.back();

      //  // this is an example of how to trim the vertex/triangle arrays when copying data out to GPU storage
      //  meshlet_vertices.resize(last.vertex_offset + last.vertex_count);
      //  meshlet_triangles.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));
      //}

      double avg_vertices = 0;
      double avg_triangles = 0;
      double avg_boundary = 0;
      double avg_connected = 0;
      size_t not_full = 0;

      std::vector<int> boundary(vrt_count);

      for (const auto& meshlet : meshlets)
      {
        avg_vertices += meshlet.vertex_count;
        avg_triangles += meshlet.triangle_count;
        not_full += meshlet.triangle_count < trg_limit;

        for (uint32_t j = 0; j < meshlet.vertex_count; ++j)
        {
          const auto counter = boundary[meshlet_vrt[meshlet.vertex_offset + j]]++;
          avg_boundary += counter == 2 ? 1 : 0;
        }

        std::array<int, vrt_limit> parents;
        for (auto j = 0u; j < meshlet.vertex_count; ++j)
        {
          parents[j] = int(j);
        }

        const auto follow = [&parents](int index)
        {
          while (index != parents[index])
          {
            const auto parent = parents[index];
            parents[index] = parents[parent];
            index = parent;
          }

          return index;
        };

        for (auto j = 0u; j < meshlet.triangle_count * 3; ++j)
        {
          const auto v0 = follow(meshlet_trg[meshlet.triangle_offset + j]);
          const auto v1 = follow(meshlet_trg[meshlet.triangle_offset + j + (j % 3 == 2 ? -2 : 1)]);

          parents[v0] = v1;
        }

        int roots = 0;
        for (auto j = 0u; j < meshlet.vertex_count; ++j)
        {
          roots += follow(j) == int(j);
        }

        assert(roots != 0);
        avg_connected += roots;
      }

      avg_vertices /= double(meshlets.size());
      avg_triangles /= double(meshlets.size());
      avg_boundary /= double(meshlets.size());
      avg_connected /= double(meshlets.size());

      //printf("Meshlets%c: %d meshlets (avg vertices %.1f, avg triangles %.1f, avg boundary %.1f, avg connected %.2f, not full %d) in %.2f msec\n",
      //  scan ? 'S' : (flex ? 'F' : (uniform ? 'U' : ' ')),
      //  int(meshlets.size()), avg_vertices, avg_triangles, avg_boundary, avg_connected, int(not_full), (end - start) * 1000);


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