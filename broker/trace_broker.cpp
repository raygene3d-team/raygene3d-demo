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
#include "trace_broker.h"

namespace RayGene3D
{
  void RecursiveBuild2(std::vector<Box>& leaves, size_t begin, size_t end, std::vector<Box>& nodes, uint32_t depth)
  {
    Box node;

    for (uint32_t i = begin; i < end; ++i)
    {
      const auto& leaf = leaves[i];

      node.min = glm::min(node.min, leaf.min);
      node.max = glm::max(node.max, leaf.max);
    }

    const auto node_center = 0.5f * (node.max + node.min);
    const auto node_extent = 0.5f * (node.max - node.min);
    const auto node_area = 4.0f * std::abs(node_extent.x * node_extent.y + node_extent.y * node_extent.z + node_extent.z * node_extent.x);

    node.offset = begin;
    node.count = 2 * (end - begin) - 1;
    nodes.emplace_back(node);

    if (end - begin > 1)
    {
      Box area;

      for (uint32_t i = begin; i < end; ++i)
      {
        const auto& leaf = leaves[i];

        const auto leaf_center = 0.5f * (leaf.max + leaf.min);
        const auto leaf_extent = 0.5f * (leaf.max - leaf.min);

        area.min = glm::min(area.min, leaf_center);
        area.max = glm::max(area.max, leaf_center);
      }

      const auto area_center = 0.5f * (area.max + area.min);
      const auto area_extent = 0.5f * (area.max - area.min);

      const auto area_norm = ((area_extent.y > area_extent.x) || (area_extent.z > area_extent.x)) ? area_extent.z > area_extent.y ? area_extent.z : area_extent.y : area_extent.x;
      const auto axis = /* (depth + 0) % 3; */ ((area_extent.y > area_extent.x) || (area_extent.z > area_extent.x)) ? area_extent.z > area_extent.y ? 2 : 1 : 0;
      //nodes[nodes.size() - 1].flags = axis;

      std::stable_sort(&leaves[begin], &leaves[end - 1] + 1, [axis](const Box& a, const Box& b)
        {
          return (a.max[axis] + a.min[axis]) > (b.max[axis] + b.min[axis]);
        });
      auto median = (begin + end) / 2;


      bool use_sah = true;
      if (use_sah && (end - begin > 2))
      {
        constexpr auto bucket_count = 16;
        const auto bucket_stride = (leaves[end - 1].count - leaves[begin].count) / bucket_count + 1;

        std::array<Box, bucket_count> bucket_array;
        for (uint32_t i = begin; i < end; ++i)
        {
          const auto& leaf = leaves[i];

          const auto leaf_center = 0.5f * (leaf.max + leaf.min);
          const auto leaf_extent = 0.5f * (leaf.max - leaf.min);

          const auto leaf_norm = 0.5f * (leaf_center - area.min) / area_extent;
          const auto leaf_index = int(leaf_norm[axis] * bucket_count);
          auto& leaf_bucket = bucket_array[std::max(0, std::min(bucket_count - 1, leaf_index))];

          leaf_bucket.count += 1;
          leaf_bucket.min = glm::min(leaf_bucket.min, leaf.min);
          leaf_bucket.max = glm::max(leaf_bucket.max, leaf.max);
        }

        std::array<float, bucket_count - 1> cost_array;
        for (uint32_t i = 0; i < cost_array.size(); ++i)
        {
          Box box0;
          for (uint32_t j = 0; j <= i; ++j)
          {
            const auto& bucket = bucket_array[j];
            box0.min = glm::min(box0.min, bucket.min);
            box0.max = glm::max(box0.max, bucket.max);
            box0.count += bucket.count;
          }
          const auto extent0 = 0.5f * (box0.max - box0.min);
          const auto area0 = std::min(FLT_MAX, 4.0f * std::abs(extent0.x * extent0.y + extent0.y * extent0.z + extent0.z * extent0.x));

          Box box1;
          for (uint32_t j = i + 1; j < cost_array.size(); ++j)
          {
            const auto& bucket = bucket_array[j];
            box1.min = glm::min(box1.min, bucket.min);
            box1.max = glm::max(box1.max, bucket.max);
            box1.count += bucket.count;
          }
          const auto extent1 = 0.5f * (box1.max - box1.min);
          const auto area1 = std::min(FLT_MAX, 4.0f * std::abs(extent1.x * extent1.y + extent1.y * extent1.z + extent1.z * extent1.x));

          cost_array[i] = 1.0f + (box0.count * area0 + box1.count * area1) / node_area;
        }

        const auto median_index = std::distance(cost_array.begin(), std::min_element(cost_array.begin(), cost_array.end()));

        if (cost_array[median_index] < end - begin)
        {
          const auto median_leaf = std::stable_partition(&leaves[begin], &leaves[end - 1] + 1, [&](const Box& leaf)
            {
              const auto leaf_center = 0.5f * (leaf.max + leaf.min);
              const auto leaf_extent = 0.5f * (leaf.max - leaf.min);

              const auto leaf_norm = 0.5f * (leaf_center - area.min) / area_extent;
              const auto leaf_index = int(leaf_norm[axis] * bucket_count);

              return leaf_index > median_index;
            });
          median = uint32_t(median_leaf - &leaves[0]);
        }
      }

      RecursiveBuild2(leaves, begin, median, nodes, depth + 1);
      RecursiveBuild2(leaves, median, end, nodes, depth + 1);
    }
  }

  void MainBuild(std::pair<const Instance*, size_t> instances,
    std::pair<const Triangle*, size_t> triangles,
    std::pair<const Vertex*, size_t> vertices,
    std::vector<Box>& instance_boxes, std::vector<Box>& triangle_boxes)
  {
    instance_boxes.clear();
    triangle_boxes.clear();

    const auto& [instance_items, instance_count] = instances;
    const auto& [triangle_items, triangle_count] = triangles;
    const auto& [vertex_items, vertex_count] = vertices;

    for (uint32_t i = 0; i < instance_count; ++i)
    {
      const auto& instance = instance_items[i];

      std::vector<Box> triangle_leaves(instance.trng_count);
      for (uint32_t j = 0; j < triangle_leaves.size(); ++j)
      {
        const auto& triangle = triangle_items[j + instance.trng_offset];
        const auto& vertex0 = vertex_items[triangle.idx[0] + instance.vert_offset];
        const auto& vertex1 = vertex_items[triangle.idx[1] + instance.vert_offset];
        const auto& vertex2 = vertex_items[triangle.idx[2] + instance.vert_offset];

        auto& leaf = triangle_leaves[j];
        leaf.offset = j;
        leaf.count = 1;
        leaf.min = glm::min(vertex0.pos, glm::min(vertex1.pos, vertex2.pos));
        leaf.max = glm::max(vertex0.pos, glm::max(vertex1.pos, vertex2.pos));
      }

      std::vector<Box> nodes;
      RecursiveBuild2(triangle_leaves, 0u, triangle_leaves.size(), nodes, 0);

      uint32_t counter = 0;
      for (uint32_t j = 0; j < nodes.size(); ++j)
      {
        auto& node = nodes[j];
        if (node.count > 1) continue;

        node.offset = triangle_leaves[node.offset].offset;
        ++counter;
      }
      BLAST_ASSERT(counter == triangle_leaves.size());

      for (auto& node : nodes)
      {
        if (node.count == 1)
        {
          const auto& triangle = triangle_items[node.offset + instance.trng_offset];
          const auto& vertex0 = vertex_items[triangle.idx[0] + instance.vert_offset];
          const auto& vertex1 = vertex_items[triangle.idx[1] + instance.vert_offset];
          const auto& vertex2 = vertex_items[triangle.idx[2] + instance.vert_offset];

          auto center = (vertex0.pos + vertex1.pos + vertex2.pos) / 3.0f;
          center.x = glm::unpackHalf2x16(glm::packHalf2x16(glm::f32vec2(center.x, center.x))).x;
          center.y = glm::unpackHalf2x16(glm::packHalf2x16(glm::f32vec2(center.y, center.y))).x;
          center.z = glm::unpackHalf2x16(glm::packHalf2x16(glm::f32vec2(center.z, center.z))).x;

          const auto delta0 = (vertex0.pos - center);
          const auto delta1 = (vertex1.pos - center);
          const auto delta2 = (vertex2.pos - center);

          glm::uvec3 packed_min;
          packed_min.x = glm::packHalf2x16(glm::f32vec2(center.x, delta0.x));
          packed_min.y = glm::packHalf2x16(glm::f32vec2(center.y, delta0.y));
          packed_min.z = glm::packHalf2x16(glm::f32vec2(center.z, delta0.z));
          memcpy(&node.min, &packed_min, 3 * sizeof(uint32_t));

          glm::uvec3 packed_max;
          packed_max.x = glm::packHalf2x16(glm::f32vec2(delta1.x, delta2.x));
          packed_max.y = glm::packHalf2x16(glm::f32vec2(delta1.y, delta2.y));
          packed_max.z = glm::packHalf2x16(glm::f32vec2(delta1.z, delta2.z));
          memcpy(&node.max, &packed_max, 3 * sizeof(uint32_t));
        }
      }
      triangle_boxes.insert(triangle_boxes.end(), nodes.begin(), nodes.end());
    }



    std::vector<Box> instance_leaves(instance_count);
    for (size_t i = 0; i < instance_leaves.size(); ++i)
    {
      const auto& instance = instance_items[i];

      auto& leaf = instance_leaves[i];
      leaf.offset = i;
      leaf.count = 1;
      leaf.min = instance.aabb_min;
      leaf.max = instance.aabb_max;
    }

    std::vector<Box> nodes;
    RecursiveBuild2(instance_leaves, 0, instance_leaves.size(), nodes, 0);

    uint32_t counter = 0;
    for (uint32_t i = 0; i < nodes.size(); ++i)
    {
      auto& node = nodes[i];
      if (node.count > 1) continue;

      node.offset = instance_leaves[node.offset].offset;
      ++counter;
    }
    BLAST_ASSERT(counter == instance_leaves.size())

    for (auto& node : nodes)
    {
      if (node.count == 1)
      {
        const auto center = 0.5f * (node.max + node.min);
        const auto extent = 0.5f * (node.max - node.min);

        glm::uvec3 packed_min;
        packed_min.x = glm::packHalf2x16(glm::f32vec2(center.x, extent.x));
        packed_min.y = glm::packHalf2x16(glm::f32vec2(center.y, extent.y));
        packed_min.z = glm::packHalf2x16(glm::f32vec2(center.z, extent.z));
        memcpy(&node.min, &packed_min, 3 * sizeof(uint32_t));

        glm::uvec3 packed_max;
        packed_max.x = glm::packHalf2x16(glm::f32vec2(extent.x, center.x));
        packed_max.y = glm::packHalf2x16(glm::f32vec2(extent.y, center.y));
        packed_max.z = glm::packHalf2x16(glm::f32vec2(extent.z, center.z));
        memcpy(&node.max, &packed_max, 3 * sizeof(uint32_t));
      }
    }

    instance_boxes.insert(instance_boxes.end(), nodes.begin(), nodes.end());;
  }

  void TraceBroker::CreateTraceBufferTBox()
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

  void TraceBroker::CreateTraceBufferBBox()
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

  void TraceBroker::CreateTraceBufferInst()
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

  void TraceBroker::CreateTraceBufferTrng()
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

  void TraceBroker::CreateTraceBufferVert()
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

  void TraceBroker::DestroyTraceBufferTBox()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_tbox);
    trace_buffer_tbox.reset();
  }

  void TraceBroker::DestroyTraceBufferBBox()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_bbox);
    trace_buffer_bbox.reset();
  }

  void TraceBroker::DestroyTraceBufferInst()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_inst);
    trace_buffer_inst.reset();
  }

  void TraceBroker::DestroyTraceBufferTrng()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_trng);
    trace_buffer_trng.reset();
  }

  void TraceBroker::DestroyTraceBufferVert()
  {
    wrap.GetCore()->GetDevice()->DestroyResource(trace_buffer_vert);
    trace_buffer_vert.reset();
  }


  void TraceBroker::Initialize()
  {
    std::vector<Box> tbox_items;
    std::vector<Box> bbox_items;

    const auto inst_items = prop_buffer_inst->GetObjectItem("binary")->GetRawItems<Instance>(0);
    const auto trng_items = prop_buffer_trng->GetObjectItem("binary")->GetRawItems<Triangle>(0);
    const auto vert_items = prop_buffer_vert->GetObjectItem("binary")->GetRawItems<Vertex>(0);

    CreateTraceBufferInst();
    CreateTraceBufferTrng();
    CreateTraceBufferVert();

    MainBuild(inst_items, trng_items, vert_items, tbox_items, bbox_items);

    {
      const auto buffer_tbox = StructureBuffer<Box>({ tbox_items.data(), tbox_items.size() });
      prop_buffer_tbox = buffer_tbox.Export();
      prop_scene->SetObjectItem("buffer_tbox", prop_buffer_tbox);
    }
    CreateTraceBufferTBox();

    {
      const auto buffer_bbox = StructureBuffer<Box>({ bbox_items.data(), bbox_items.size() });
      prop_buffer_bbox = buffer_bbox.Export();
      prop_scene->SetObjectItem("buffer_bbox", prop_buffer_bbox);
    }
    CreateTraceBufferBBox();

    //CreateTraceBufferTBox();
    //CreateTraceBufferBBox();
  }

  void TraceBroker::Use()
  {

  }

  void TraceBroker::Discard()
  {
    //DestroyTraceInstances();
    //DestroyTraceTriangles();
    //DestroyTraceVertices();

    //DestroyTraceTBoxes();
    //DestroyTraceBBoxes();

    prop_buffer_tbox.reset();
    prop_buffer_bbox.reset();

    prop_buffer_inst.reset();
    prop_buffer_trng.reset();
    prop_buffer_vert.reset();

    prop_scene.reset();
  }

  TraceBroker::TraceBroker(Wrap& wrap)
    : Broker("trace_broker", wrap)
  {
    const auto& tree = wrap.GetUtil()->GetStorage()->GetTree();

    prop_scene = tree->GetObjectItem("scene");
    {
      prop_buffer_inst = prop_scene->GetObjectItem("buffer_inst");
      prop_buffer_trng = prop_scene->GetObjectItem("buffer_trng");
      prop_buffer_vert = prop_scene->GetObjectItem("buffer_vert");

      prop_buffer_tbox = prop_scene->GetObjectItem("buffer_tbox");
      prop_buffer_bbox = prop_scene->GetObjectItem("buffer_bbox");
    }
  }

  TraceBroker::~TraceBroker()
  {}

}