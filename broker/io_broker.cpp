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
#include "io_broker.h"

namespace RayGene3D
{
  void IOBroker::Initialize()
  {
  }

  void IOBroker::Use()
  {
    switch (operation)
    {
    case SAVE_OPERATION:
    {
    } break;
    case LOAD_OPERATION:
    {
      const auto extension = ExtractExtension(scope.file_name);
      if (std::strcmp(extension.c_str(), "obm") == 0) { mode = std::unique_ptr<IO::Mode>(new IO::OBJConverter(scope)); } else
      if (std::strcmp(extension.c_str(), "gltf") == 0) { mode = std::unique_ptr<IO::Mode>(new IO::GLTFConverter(scope)); }
      mode->Import();
      scope.Export();
    } break;
    }
  }

  void IOBroker::Discard()
  {
  }

  IOBroker::IOBroker(Wrap& wrap)
    : Broker("io_broker", wrap)
    , scope(wrap.GetCore(), wrap.GetUtil())
  {}

  IOBroker::~IOBroker()
  {}
}