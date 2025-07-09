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

        const auto layer_0_property = CreateUIntProperty();   layer_0_property->FromUInt(instance.layer_0);  item_property->SetObjectItem("layer_0", layer_0_property);
        const auto layer_1_property = CreateUIntProperty();   layer_1_property->FromUInt(instance.layer_1);  item_property->SetObjectItem("layer_1", layer_1_property);
        const auto layer_2_property = CreateUIntProperty();   layer_2_property->FromUInt(instance.layer_2);  item_property->SetObjectItem("layer_2", layer_2_property);
        const auto layer_3_property = CreateUIntProperty();   layer_3_property->FromUInt(instance.layer_3);  item_property->SetObjectItem("layer_3", layer_3_property);

        const auto offset_0_property = CreateUIntProperty();  offset_0_property->FromUInt(instance.offset_0);   item_property->SetObjectItem("offset_0", offset_0_property);
        const auto count_0_property = CreateUIntProperty();   count_0_property->FromUInt(instance.count_0);     item_property->SetObjectItem("count_0", count_0_property);
        const auto offset_1_property = CreateUIntProperty();  offset_1_property->FromUInt(instance.offset_1);   item_property->SetObjectItem("offset_1", offset_1_property);
        const auto count_1_property = CreateUIntProperty();   count_1_property->FromUInt(instance.count_1);     item_property->SetObjectItem("count_1", count_1_property);
        const auto offset_2_property = CreateUIntProperty();  offset_2_property->FromUInt(instance.offset_2);   item_property->SetObjectItem("offset_2", offset_2_property);
        const auto count_2_property = CreateUIntProperty();   count_2_property->FromUInt(instance.count_2);     item_property->SetObjectItem("count_2", count_2_property);
        const auto offset_3_property = CreateUIntProperty();  offset_3_property->FromUInt(instance.offset_3);   item_property->SetObjectItem("offset_3", offset_3_property);
        const auto count_3_property = CreateUIntProperty();   count_3_property->FromUInt(instance.count_3);     item_property->SetObjectItem("count_3", count_3_property);
        
        const auto aabb_min_property = CreateFVec3Property(); aabb_min_property->FromFVec3(instance.aabb_min);  item_property->SetObjectItem("aabb_min", aabb_min_property);
        const auto geom_idx_property = CreateUIntProperty();  geom_idx_property->FromUInt(instance.geom_idx);   item_property->SetObjectItem("geom_idx", geom_idx_property);
        const auto aabb_max_property = CreateFVec3Property(); aabb_max_property->FromFVec3(instance.aabb_max);  item_property->SetObjectItem("aabb_max", aabb_max_property);
        const auto brdf_idx_property = CreateUIntProperty();  brdf_idx_property->FromUInt(instance.brdf_idx);   item_property->SetObjectItem("brdf_idx", brdf_idx_property);

        const auto param_0_property = CreateFVec4Property();   param_0_property->FromFVec4(instance.param_0);   item_property->SetObjectItem("param_0", param_0_property);
        const auto param_1_property = CreateFVec4Property();   param_1_property->FromFVec4(instance.param_1);   item_property->SetObjectItem("param_1", param_1_property);
        const auto param_2_property = CreateFVec4Property();   param_2_property->FromFVec4(instance.param_2);   item_property->SetObjectItem("param_2", param_2_property);
        const auto param_3_property = CreateFVec4Property();   param_3_property->FromFVec4(instance.param_3);   item_property->SetObjectItem("param_3", param_3_property);        

        prop_instances->SetArrayItem(i, item_property);
      }
      prop_scene->SetObjectItem("instances", prop_instances);


      const auto prop_buffer_0 = CreateBufferProperty(std::move(buffer_0->raw), buffer_0->stride, buffer_0->count);
      prop_scene->SetObjectItem("buffer_0", prop_buffer_0);

      const auto prop_buffer_1 = CreateBufferProperty(std::move(buffer_1->raw), buffer_1->stride, buffer_1->count);
      prop_scene->SetObjectItem("buffer_1", prop_buffer_1);

      //CreateTextureProperty()

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
