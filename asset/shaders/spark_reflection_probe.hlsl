#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#define CUBEMAP_POSITIVE_X 0
#define CUBEMAP_NEGATIVE_X 1
#define CUBEMAP_POSITIVE_Y 2
#define CUBEMAP_NEGATIVE_Y 3
#define CUBEMAP_POSITIVE_Z 4
#define CUBEMAP_NEGATIVE_Z 5

VK_BINDING(0) sampler sampler0 : register(s0);

VK_BINDING(1) cbuffer constant0 : register(b0)
{
  uint mip_level     : packoffset(c0.x);
  uint mip_size      : packoffset(c0.y);
  uint dummy[62]     : packoffset(c0.z);
}

VK_BINDING(2) TextureCube<float4> skybox_texture : register(t0);

#include "ImageBasedLighting/reflection_probe.hlsl"

struct VSInput
{
  VK_LOCATION(0) float2 pos : register0;
  VK_LOCATION(1) float2 uv : register1;
  VK_LOCATION(2) uint inst_id : SV_InstanceID;
};

struct VSOutput
{
  VK_LOCATION(0) float4 pos : SV_Position;
  VK_LOCATION(1) float2 uv : register0;
  VK_LOCATION(2) uint view_id : register1;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;
  output.pos = float4(input.pos, 1.0, 1.0);
  output.uv = input.uv;
  output.view_id = input.inst_id;
  return output;
}

struct GSOutput
{
  VK_LOCATION(0) float4 pos : SV_Position;
  VK_LOCATION(1) uint rtv_id : SV_RenderTargetArrayIndex;
  VK_LOCATION(2) float2 uv : register0;
  VK_LOCATION(3) uint inst_id : register1;
};

[maxvertexcount(3)]
void gs_main(triangle VSOutput input[3], inout TriangleStream<GSOutput> outStream)
{
  GSOutput output;
  [unroll(3)]
  for (int i = 0; i < 3; ++i)
  {
    output.pos = input[i].pos;
    output.uv = input[i].uv;
    output.rtv_id = input[i].view_id;
    output.inst_id = input[i].view_id;
    outStream.Append(output);
  }
}

float3 uv_to_cube(float u, float v, uint layer)
{
  float3 text_coord = float3(0.0, 0.0, 0.0);
  switch (layer)
  {
  case CUBEMAP_POSITIVE_X:
    text_coord.x = 1.0f;
    text_coord.y = -(2.0f * v - 1.0f);
    text_coord.z = -(2.0f * u - 1.0f);
    break;
  case CUBEMAP_NEGATIVE_X:
    text_coord.x = -1.0f;
    text_coord.y = -(2.0f * v - 1.0f);
    text_coord.z = (2.0f * u - 1.0f);
    break;
  case CUBEMAP_POSITIVE_Y:
    text_coord.x = (2.0f * u - 1.0f);
    text_coord.y = 1.0f;
    text_coord.z = (2.0f * v - 1.0f);
    break;
  case CUBEMAP_NEGATIVE_Y:
    text_coord.x = (2.0f * u - 1.0f);
    text_coord.y = -1.0f;
    text_coord.z = -(2.0f * v - 1.0f);
    break;
  case CUBEMAP_POSITIVE_Z:
    text_coord.x = (2.0f * u - 1.0f);
    text_coord.y = -(2.0f * v - 1.0f);
    text_coord.z = 1.0f;
    break;
  case CUBEMAP_NEGATIVE_Z:
    text_coord.x = -(2.0f * u - 1.0f);
    text_coord.y = -(2.0f * v - 1.0f);
    text_coord.z = -1.0f;
    break;
  }

  return normalize(text_coord);
}

float4 ps_main(GSOutput input) : SV_Target
{
  float2 uv = input.pos.xy / float2(mip_size, mip_size);
  float3 texcoord = uv_to_cube(uv.x, uv.y, input.inst_id);

  uint cubemapWidth = 0;
  uint cubemapHeight = 0;
  skybox_texture.GetDimensions(cubemapWidth, cubemapHeight);

  float omegaP = FOUR_PI / (6.0 * cubemapWidth * cubemapWidth);
  float invOmegaP = 1.0 / omegaP;

  float3 N = texcoord;
  float3 V = N;
  bool prefilter = true;

  float perceptualRoughness = MipmapLevelToPerceptualRoughness(mip_level);
  float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);
  uint  sampleCount = GetIBLRuntimeFilterSampleCount(mip_level);

  float4 color = skybox_texture.Sample(sampler0, texcoord);

  if (mip_level != 0)
  {
    color = IntegrateLD(
      V,
      N,
      roughness,
      invOmegaP,
      sampleCount,
      prefilter);
  }

  return float4(color.xyz, 1.0);
}