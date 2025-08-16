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

      const auto prop_buffer_0 = buffer_0.Export();
      prop_scene->SetObjectItem("buffer_0", prop_buffer_0);

      const auto prop_buffer_1 = buffer_1.Export();
      prop_scene->SetObjectItem("buffer_1", prop_buffer_1);

      const auto prop_buffer_2 = buffer_2.Export();
      prop_scene->SetObjectItem("buffer_2", prop_buffer_2);

      const auto prop_buffer_3 = buffer_3.Export();
      prop_scene->SetObjectItem("buffer_3", prop_buffer_3);

      const auto prop_array_0 = array_0.Export();
      prop_scene->SetObjectItem("array_0", prop_array_0);

      const auto prop_array_1 = array_1.Export();
      prop_scene->SetObjectItem("array_1", prop_array_1);

      const auto prop_array_2 = array_2.Export();
      prop_scene->SetObjectItem("array_2", prop_array_2);

      const auto prop_array_3 = array_3.Export();
      prop_scene->SetObjectItem("array_3", prop_array_3);

      util->GetStorage()->GetTree()->SetObjectItem("scene", prop_scene);
    }

    Scope::Scope(const std::unique_ptr<Core>& core, const std::unique_ptr<Util>& util)
      : core(core)
      , util(util)
      , array_0(Format::FORMAT_R8G8B8A8_UNORM, 4, 4, 1u)
      , array_1(Format::FORMAT_R8G8B8A8_UNORM, 4, 4, 1u)
      , array_2(Format::FORMAT_R8G8B8A8_UNORM, 4, 4, 1u)
      , array_3(Format::FORMAT_R8G8B8A8_UNORM, 4, 4, 1u)
    {
      array_0.Create(0);
      array_1.Create(0);
      array_2.Create(0);
      array_3.Create(0);
    }

    Scope::~Scope()
    {
    }
  }
}
