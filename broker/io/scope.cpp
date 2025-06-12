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


#include "scope.h"

namespace RayGene3D
{
  namespace IO
  {
    void Scope::Import()
    {
      const auto& prop_scene = util->GetStorage()->GetTree()->GetObjectItem("scene");

    }

    void Scope::Export()
    {
      auto prop_scene = std::shared_ptr<Property>(new Property(Property::TYPE_OBJECT));

      auto prop_instances = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
      prop_instances->SetArraySize(instances.size());      
      for (size_t i = 0u; i < instances.size(); ++i)
      {
        const auto& instance = instances[i];

        const auto item_property = std::shared_ptr<Property>(new Property(Property::TYPE_OBJECT));

        const auto transform_property = CreateFMat3x4Property();   transform_property->FromFMat3x4(instance.transform);     item_property->SetObjectItem("transform", transform_property);

        const auto prim_offset_property = CreateUIntProperty();    prim_offset_property->FromUInt(instance.prim_offset);    item_property->SetObjectItem("prim_offset", prim_offset_property);
        const auto prim_count_property = CreateUIntProperty();     prim_count_property->FromUInt(instance.prim_count);      item_property->SetObjectItem("prim_count", prim_count_property);
        const auto vert_offset_property = CreateUIntProperty();    vert_offset_property->FromUInt(instance.vert_offset);    item_property->SetObjectItem("vert_offset", vert_offset_property);
        const auto vert_count_property = CreateUIntProperty();     vert_count_property->FromUInt(instance.vert_count);      item_property->SetObjectItem("vert_count", vert_count_property);

        const auto brdf_param0_property = CreateFVec4Property();   brdf_param0_property->FromFVec4(instance.brdf_param0);   item_property->SetObjectItem("brdf_param0", brdf_param0_property);
        const auto brdf_param1_property = CreateFVec4Property();   brdf_param1_property->FromFVec4(instance.brdf_param1);   item_property->SetObjectItem("brdf_param1", brdf_param1_property);
        const auto brdf_param2_property = CreateFVec4Property();   brdf_param2_property->FromFVec4(instance.brdf_param2);   item_property->SetObjectItem("brdf_param2", brdf_param2_property);
        const auto brdf_param3_property = CreateFVec4Property();   brdf_param3_property->FromFVec4(instance.brdf_param3);   item_property->SetObjectItem("brdf_param3", brdf_param3_property);

        const auto texture0_idx_property = CreateUIntProperty();   texture0_idx_property->FromUInt(instance.texture0_idx);  item_property->SetObjectItem("texture0_idx", texture0_idx_property);
        const auto texture1_idx_property = CreateUIntProperty();   texture1_idx_property->FromUInt(instance.texture1_idx);  item_property->SetObjectItem("texture1_idx", texture1_idx_property);
        const auto texture2_idx_property = CreateUIntProperty();   texture2_idx_property->FromUInt(instance.texture2_idx);  item_property->SetObjectItem("texture2_idx", texture2_idx_property);
        const auto texture3_idx_property = CreateUIntProperty();   texture3_idx_property->FromUInt(instance.texture3_idx);  item_property->SetObjectItem("texture3_idx", texture3_idx_property);
        const auto texture4_idx_property = CreateUIntProperty();   texture4_idx_property->FromUInt(instance.texture4_idx);  item_property->SetObjectItem("texture4_idx", texture4_idx_property);
        const auto texture5_idx_property = CreateUIntProperty();   texture5_idx_property->FromUInt(instance.texture5_idx);  item_property->SetObjectItem("texture5_idx", texture5_idx_property);
        const auto texture6_idx_property = CreateUIntProperty();   texture6_idx_property->FromUInt(instance.texture6_idx);  item_property->SetObjectItem("texture6_idx", texture6_idx_property);
        const auto texture7_idx_property = CreateUIntProperty();   texture7_idx_property->FromUInt(instance.texture7_idx);  item_property->SetObjectItem("texture7_idx", texture7_idx_property);

        const auto aabb_min_property = CreateFVec3Property();      aabb_min_property->FromFVec3(instance.aabb_min);         item_property->SetObjectItem("aabb_min", aabb_min_property);
        const auto geom_idx_property = CreateUIntProperty();       geom_idx_property->FromUInt(instance.geom_idx);          item_property->SetObjectItem("geom_idx", geom_idx_property);
        const auto aabb_max_property = CreateFVec3Property();      aabb_max_property->FromFVec3(instance.aabb_max);         item_property->SetObjectItem("aabb_max", aabb_max_property);
        const auto brdf_idx_property = CreateUIntProperty();       brdf_idx_property->FromUInt(instance.brdf_idx);          item_property->SetObjectItem("brdf_idx", brdf_idx_property);

        const auto buffer0_idx_property = CreateUIntProperty();    buffer0_idx_property->FromUInt(instance.buffer0_idx);    item_property->SetObjectItem("buffer0_idx", buffer0_idx_property);
        const auto buffer1_idx_property = CreateUIntProperty();    buffer1_idx_property->FromUInt(instance.buffer1_idx);    item_property->SetObjectItem("buffer1_idx", buffer1_idx_property);
        const auto buffer2_idx_property = CreateUIntProperty();    buffer2_idx_property->FromUInt(instance.buffer2_idx);    item_property->SetObjectItem("buffer2_idx", buffer2_idx_property);
        const auto buffer3_idx_property = CreateUIntProperty();    buffer3_idx_property->FromUInt(instance.buffer3_idx);    item_property->SetObjectItem("buffer3_idx", buffer3_idx_property);
        const auto buffer4_idx_property = CreateUIntProperty();    buffer4_idx_property->FromUInt(instance.buffer4_idx);    item_property->SetObjectItem("buffer4_idx", buffer4_idx_property);
        const auto buffer5_idx_property = CreateUIntProperty();    buffer5_idx_property->FromUInt(instance.buffer5_idx);    item_property->SetObjectItem("buffer5_idx", buffer5_idx_property);
        const auto buffer6_idx_property = CreateUIntProperty();    buffer6_idx_property->FromUInt(instance.buffer6_idx);    item_property->SetObjectItem("buffer6_idx", buffer6_idx_property);
        const auto buffer7_idx_property = CreateUIntProperty();    buffer7_idx_property->FromUInt(instance.buffer7_idx);    item_property->SetObjectItem("buffer7_idx", buffer7_idx_property);

        prop_instances->SetArrayItem(i, item_property);
      }
      prop_scene->SetObjectItem("instances", prop_instances);

      auto prop_buffers_0 = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
      prop_buffers_0->SetArraySize(buffers_0.size());
      for (size_t i = 0u; i < buffers_0.size(); ++i)
      {
        auto& [raw, stride, count] = buffers_0[i];
        const auto item_property = CreateBufferProperty(std::move(raw), stride, count);
        prop_buffers_0->SetArrayItem(i, item_property);
      }
      prop_scene->SetObjectItem("buffers_0", prop_buffers_0);

      auto prop_buffers_1 = std::shared_ptr<Property>(new Property(Property::TYPE_ARRAY));
      prop_buffers_1->SetArraySize(buffers_1.size());
      for (size_t i = 0u; i < buffers_1.size(); ++i)
      {
        auto& [raw, stride, count] = buffers_1[i];
        const auto item_property = CreateBufferProperty(std::move(raw), stride, count);
        prop_buffers_1->SetArrayItem(i, item_property);
      }
      prop_scene->SetObjectItem("buffers_1", prop_buffers_1);

      util->GetStorage()->GetTree()->SetObjectItem("scene", prop_scene);
    }

    Scope::Scope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util)
      : core(core)
      , util(util)
    {
      //prop_scene = std::shared_ptr<Property>(new Property(Property::TYPE_OBJECT));
    }

    Scope::~Scope()
    {
      //prop_scene.reset();
    }
  }
}
