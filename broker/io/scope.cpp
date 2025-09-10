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

      const auto prop_vert_buffer = vert_buffer.Export();
      prop_scene->SetObjectItem("buffer_vert", prop_vert_buffer);

      const auto prop_trng_buffer = trng_buffer.Export();
      prop_scene->SetObjectItem("buffer_trng", prop_trng_buffer);

      const auto prop_mlet_buffer = mlet_buffer.Export();
      prop_scene->SetObjectItem("buffer_mlet", prop_mlet_buffer);


      const auto prop_aaam_array = aaam_array.Export();
      prop_scene->SetObjectItem("array_aaam", prop_aaam_array);

      const auto prop_snno_array = snno_array.Export();
      prop_scene->SetObjectItem("array_snno", prop_snno_array);

      const auto prop_eeet_array = eeet_array.Export();
      prop_scene->SetObjectItem("array_eeet", prop_eeet_array);


      const auto prop_inst_buffer = inst_buffer.Export();
      prop_scene->SetObjectItem("buffer_inst", prop_inst_buffer);

      const auto prop_tbox_buffer = tbox_buffer.Export();
      prop_scene->SetObjectItem("buffer_tbox", prop_tbox_buffer);

      const auto prop_bbox_buffer = bbox_buffer.Export();
      prop_scene->SetObjectItem("buffer_bbox", prop_bbox_buffer);

      const auto prop_knot_buffer = knot_buffer.Export();
      prop_scene->SetObjectItem("buffer_knot", prop_knot_buffer);

      util->GetStorage()->GetTree()->SetObjectItem("scene", prop_scene);
    }

    Scope::Scope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util)
      : core(core)
      , util(util)
      , aaam_array(Format::FORMAT_R8G8B8A8_UNORM, 4, 4, 1u)
      , snno_array(Format::FORMAT_R8G8B8A8_UNORM, 4, 4, 1u)
      , eeet_array(Format::FORMAT_R8G8B8A8_UNORM, 4, 4, 1u)
      , mask_array(Format::FORMAT_R8G8B8A8_UNORM, 4, 4, 1u)
    {
    }

    Scope::~Scope()
    {
    }
  }
}
