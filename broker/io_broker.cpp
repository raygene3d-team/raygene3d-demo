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
    const auto extension = ExtractExtension(scope.file_name);

    if (std::strcmp(extension.c_str(), "obm") == 0)
    {
      mode = std::unique_ptr<IO::Mode>(new IO::OBJConverter(scope));
    }
    else if (std::strcmp(extension.c_str(), "gltf") == 0)
    {
      mode = std::unique_ptr<IO::Mode>(new IO::GLTFConverter(scope));
    }

    mode->Import();


  }

  void IOBroker::Discard()
  {
  }





  void IOBroker::Export(std::shared_ptr<Property>& property) const
  {
    auto root = std::shared_ptr<Property>(new Property(Property::TYPE_OBJECT));
    //property->setSetValue(Property::object());


  
    {
      const auto data = instances.data();
      const auto stride = uint32_t(sizeof(Instance));
      const auto count = uint32_t(instances.size());
      auto raw = Raw({ data, stride * count });
      const auto property = CreateBufferProperty({ &raw, 1u }, stride, count);
      root->SetObjectItem("instances", property);
    }

    {
      const auto data = triangles.data();
      const auto stride = uint32_t(sizeof(Triangle));
      const auto count = uint32_t(triangles.size());
      auto raw = Raw({ data, stride * count });
      const auto property = CreateBufferProperty({ &raw, 1u }, stride, count);
      root->SetObjectItem("triangles", property);
    }

    {
      const auto data = vertices.data();
      const auto stride = uint32_t(sizeof(Vertex));
      const auto count = uint32_t(vertices.size());
      auto raw = Raw({ data, stride * count });
      const auto property = CreateBufferProperty({ &raw, 1u }, stride, count);
      root->SetObjectItem("vertices", property);
    }
  
    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_0.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] = 
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_0[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty() 
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_0.size()));
      root->SetObjectItem("textures_0", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_1.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_1[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_1.size()));
      root->SetObjectItem("textures_1", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_2.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_2[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_2.size()));
      root->SetObjectItem("textures_2", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_3.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_3[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_3.size()));
      root->SetObjectItem("textures_3", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_4.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_4[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_4.size()));
      root->SetObjectItem("textures_4", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_5.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_5[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_5.size()));
      root->SetObjectItem("textures_5", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_6.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_6[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_6.size()));
      root->SetObjectItem("textures_6", property);
    }

    {
      auto extent_x = 1u << int(texture_level) - 1;
      auto extent_y = 1u << int(texture_level) - 1;
      auto raws = std::vector<Raw>();
      for (uint32_t i = 0; i < uint32_t(textures_7.size()); ++i)
      {
        auto [mipmaps, size_x, size_y] =
          MipmapTextureLDR(texture_level,
            ResizeTextureLDR(extent_x, extent_y, textures_7[i]));
        std::move(mipmaps.begin(), mipmaps.end(), std::back_inserter(raws));
      }
      auto raw = Raw(1u * uint32_t(sizeof(glm::u8vec4)));
      raw.SetElement<glm::u8vec4>({ 1u, 1u, 1u, 1u }, 0u);
      const auto property = raws.empty()
        ? CreateTextureProperty({ &raw, 1u }, 1u, 1u, 1u, 1u)
        : CreateTextureProperty({ raws.data(), uint32_t(raws.size()) },
          extent_x, extent_y, texture_level, uint32_t(textures_7.size()));
      root->SetObjectItem("textures_7", property);
    }

    property = root;
  }

  IOBroker::IOBroker(Wrap& wrap)
    : Broker("io_broker", wrap)
    , scope(wrap.GetCore(), wrap.GetUtil())
  {}

  IOBroker::~IOBroker()
  {}
}