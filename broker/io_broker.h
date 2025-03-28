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


#pragma once
#include "io/mode/obj_converter.h"
#include "io/mode/gltf_converter.h"

namespace RayGene3D
{
  class IOBroker : public Broker
  {
  protected:
    IO::Scope scope;

  protected:
    std::unique_ptr<IO::Mode> mode;

  public:
    enum DataOperation
    {
      NO_OPERATTION = 0,
      SAVE_OPERATION = 1,
      LOAD_OPERATION = 2,
    };

  protected:
    DataOperation operation{ NO_OPERATTION };

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    void SetDataOperation(DataOperation operation) { this->operation = operation; }
    DataOperation GetDataOperation() const { return operation; }

  public:
    void SetFileName(const std::string& file_name) { scope.file_name = file_name; }
    void SetPathName(const std::string& path_name) { scope.path_name = path_name; }

  public:
    void SetPositionScale(float position_scale) { scope.position_scale = position_scale; }
    void SetConversionRHS(bool conversion_rhs) { scope.conversion_rhs = conversion_rhs; }
    void SetTextureLevel(uint32_t texture_level) { scope.texture_level = texture_level; }

  //public:
  //  void SetPropScene(const std::shared_ptr<Property>& prop_scene) { scope.prop_scene = prop_scene; }

  public:
    IOBroker(Wrap& wrap);
    virtual ~IOBroker();
  };
}