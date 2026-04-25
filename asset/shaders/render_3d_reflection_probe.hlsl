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

#define REFLECTION_TEXTURE_CUBE_LOD_COUNTER 6

VK_BINDING(0) sampler sampler0 : register(s0);

VK_BINDING(1) cbuffer constant0 : register(b0)
{
  uint mip_level     : packoffset(c0.x);
  uint mip_size      : packoffset(c0.y);
  uint dummy_z       : packoffset(c0.z);
  uint dummy_w       : packoffset(c0.w);
}

VK_BINDING(2) TextureCube<float4> skybox_ri_texture : register(t0);
VK_BINDING(3) RWTexture2DArray<float4> skybox_wi_texture : register(u0);

#include "ImageBasedLighting/reflection_probe.hlsl"


float3 UVtoCube(float u, float v, uint layer)
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
    text_coord.z =  (2.0f * u - 1.0f);
    break;
  case CUBEMAP_POSITIVE_Y:
    text_coord.x =  (2.0f * u - 1.0f);
    text_coord.y =  1.0f;
    text_coord.z =  (2.0f * v - 1.0f);
    break;
  case CUBEMAP_NEGATIVE_Y:
    text_coord.x =  (2.0f * u - 1.0f);
    text_coord.y = -1.0f;
    text_coord.z = -(2.0f * v - 1.0f);
    break;
  case CUBEMAP_POSITIVE_Z:
    text_coord.x =  (2.0f * u - 1.0f);
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

[numthreads(1, 1, 6)]
void cs_main(uint3 dispatch_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID, uint3 thread_id : SV_GroupThreadID)
{
  
  float2 uv = dispatch_id.xy / float2(mip_size, mip_size);
  float3 cube_texcoord = UVtoCube(uv.x, uv.y, thread_id.z);

  uint cubemap_width = 0;
  uint cubemap_height = 0;
  skybox_ri_texture.GetDimensions(cubemap_width, cubemap_height);

  float inv_omega_p = (6.0 * cubemap_width * cubemap_height) / FOUR_PI;
  float3 N = cube_texcoord;
  float3 V = N;

  if (mip_level == 0)
  {
    float4 skybox_value = skybox_ri_texture.SampleLevel(sampler0, cube_texcoord, 0);
    skybox_wi_texture[uint3(dispatch_id.xy, thread_id.z)] = float4(skybox_value.xyz, 1.0);
  }

  float perceptual_roughness = MipmapLevelToPerceptualRoughness(mip_level, REFLECTION_TEXTURE_CUBE_LOD_COUNTER);
  float roughness = PerceptualRoughnessToRoughness(perceptual_roughness);
  uint sample_count = GetSampleCount(mip_level);

  float3 color = IntegrateReflectionProbe(V, N, roughness, inv_omega_p, sample_count);

  skybox_wi_texture[uint3(dispatch_id.xy, thread_id.z)] = float4(color, 1.0);
}