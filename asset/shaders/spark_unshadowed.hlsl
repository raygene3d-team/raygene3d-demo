
#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#include "packing.hlsl"

VK_BINDING(0) sampler sampler0 : register(s0);

VK_BINDING(1) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}

VK_BINDING(2) cbuffer constant1 : register(b1)
{
  float4x4 camera_view     : packoffset(c0.x);
  float4x4 camera_proj     : packoffset(c4.x);
  float4x4 camera_view_inv : packoffset(c8.x);
  float4x4 camera_proj_inv : packoffset(c12.x);
}

VK_BINDING(3) cbuffer constant2 : register(b2)
{
  float4x4 shadow_view     : packoffset(c0.x);
  float4x4 shadow_proj     : packoffset(c4.x);
  float4x4 shadow_view_inv : packoffset(c8.x);
  float4x4 shadow_proj_inv : packoffset(c12.x);
}


VK_BINDING(4) Texture2D<float4> gbuffer_0_texture : register(t0);
VK_BINDING(5) Texture2D<uint4> gbuffer_1_texture : register(t1);
VK_BINDING(6) Texture2D<float> depth_texture : register(t2);

struct VSInput
{
  VK_LOCATION(0) float2 pos : register0;
  VK_LOCATION(1) float2 uv : register1;
};

struct VSOutput
{
  VK_LOCATION(0) float4 pos : SV_Position;
  VK_LOCATION(1) float2 uv : register0;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;
  output.pos = float4(input.pos, 1.0, 1.0);
  output.uv = input.uv;
  return output;
}

struct PSInput
{
  VK_LOCATION(0) float4 pos : SV_Position;
  VK_LOCATION(1) float2 uv : register0;
};

struct PSOutput
{
  float3 target_0 : SV_Target0;
};

PSOutput ps_main(PSInput input)
{
  PSOutput output;

  const float rx = (input.pos.x + 0.0) / extent_x;
  const float ry = (input.pos.y + 0.0) / extent_y;
  const float2 tex_coord = float2(rx, ry);

  const float4 albedo_smoothness = gbuffer_0_texture.Sample(sampler0, tex_coord);
  const uint4 normal_metallic = gbuffer_1_texture.Sample(sampler0, tex_coord);

  const float3 normal = UnpackNormal(normal_metallic.rgb);
  const float metallic = float(normal_metallic.a) / 255.0;
  const float smoothness = albedo_smoothness.a;

  const float depth = depth_texture.Sample(sampler0, tex_coord);
  const float4 ndc_coord = float4(2.0 * tex_coord.x - 1.0, 1.0 - 2.0 * tex_coord.y, depth, 1.0);
  const float4 view_pos = mul(camera_proj_inv, ndc_coord);
  const float3 surface_pos = mul(camera_view_inv, float4(view_pos.xyz / view_pos.w, 1.0)).xyz;

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - surface_pos);
  const float3 camera_dir = (camera_pos - surface_pos) / camera_dst;

  const float3 shadow_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float shadow_dst = length(shadow_pos - surface_pos);
  const float3 shadow_dir = (shadow_pos - surface_pos) / shadow_dst;

  const float3 diffuse = max(0.0, dot(shadow_dir, normal)) * albedo_smoothness.xyz;
  const float3 specular = pow(max(0.0, dot(normalize(camera_dir + shadow_dir), normal)), smoothness);
  const float attenuation = 1.0;

  const float3 color = diffuse * attenuation;

  output.target_0 = float3(color);
  return output;
}