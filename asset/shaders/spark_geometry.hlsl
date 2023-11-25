#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#define USE_NORMAL_MAP
//#define USE_ALPHA_CLIP
#define USE_SPECULAR_SETUP
#define USE_NORMAL_OCT_QUAD_ENCODING

#include "common.hlsl"
#include "surface.hlsl"
#include "brdf.hlsl"
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

VK_BINDING(4) cbuffer constant3 : register(b3)
{
  float4x3 transform  : packoffset(c0.x);

  uint prim_offset    : packoffset(c3.x);
  uint prim_count     : packoffset(c3.y);
  uint vert_offset    : packoffset(c3.z);
  uint vert_count     : packoffset(c3.w);

  float3 emission     : packoffset(c4.x);
  float intensity : packoffset(c4.w);
  float3 diffuse      : packoffset(c5.x);
  float shininess : packoffset(c5.w);
  float3 specular     : packoffset(c6.x);
  float alpha : packoffset(c6.w);

  int tex0_idx : packoffset(c7.x);
  int tex1_idx : packoffset(c7.y);
  int tex2_idx : packoffset(c7.z);
  int tex3_idx : packoffset(c7.w);

  float debug_color : packoffset(c8.x);
  uint geometry_idx   : packoffset(c8.w);

  uint4 padding[7]    : packoffset(c9.x);
};

VK_BINDING(5) Texture2DArray<float4> texture0_items : register(t0);
VK_BINDING(6) Texture2DArray<float4> texture1_items : register(t1);
VK_BINDING(7) Texture2DArray<float4> texture2_items : register(t2);
VK_BINDING(8) Texture2DArray<float4> texture3_items : register(t3);

//VK_BINDING(9) Texture2DArray<float4> texture4_items : register(t4);



struct VSInput
{
  VK_LOCATION(0) float3 pos : register0;
  VK_LOCATION(1) float4 col : register1;
  VK_LOCATION(2) float3 nrm : register2;
  VK_LOCATION(3) uint msk 	: register3;
  VK_LOCATION(4) float3 tgn : register4;
  VK_LOCATION(5) float sgn : register5;
  VK_LOCATION(6) float2 tc0 : register6;
  VK_LOCATION(7) float2 tc1 : register7;
};

struct VSOutput
{
  VK_LOCATION(0) float4 w_pos_d : register0;
  VK_LOCATION(1) float4 w_nrm_u : register1;
  VK_LOCATION(2) float4 w_tng_v : register2;
  VK_LOCATION(3) float2 tc1		: register3;
  VK_LOCATION(4) uint mask 		: register4;
  VK_LOCATION(5) float4 pos   : SV_Position;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;

  output.pos = mul(camera_proj, mul(camera_view, float4(input.pos, 1.0)));
  output.w_pos_d = float4(input.pos, input.sgn);
  output.w_nrm_u = float4(input.nrm, input.tc0.x);
  output.w_tng_v = float4(input.tgn, input.tc0.y);
  output.tc1 = input.tc1;
  output.mask = input.msk;
  return output;
}

struct PSInput
{
  VK_LOCATION(0) float4 w_pos_d : register0;
  VK_LOCATION(1) float4 w_nrm_u : register1;
  VK_LOCATION(2) float4 w_tng_v : register2;
  VK_LOCATION(3) float2 tc1		: register3;
  VK_LOCATION(4) uint mask 		: register4;
};

struct PSOutput
{
  float4 target_0 : SV_Target0;
  float4 target_1 : SV_Target1;
  float3 target_2 : SV_Target2;
};

PSOutput ps_main(PSInput input)
{
  PSOutput output;

  float3 n = normalize(input.w_nrm_u.xyz);
  float3 t = normalize(input.w_tng_v.xyz);
  float3 b = input.w_pos_d.w * cross(n, t);

#ifdef USE_SPECULAR_SETUP
  const float4 tex0_value = tex0_idx != -1 ? texture0_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex0_idx)) : float4(1.0, 1.0, 1.0, 1.0);
  const float4 tex1_value = tex1_idx != -1 ? texture1_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex1_idx)) : float4(1.0, 1.0, 1.0, 1.0);
  const float4 tex2_value = tex2_idx != -1 ? texture2_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex2_idx)) : float4(1.0, 1.0, 1.0, 1.0);
  const float4 tex3_value = tex3_idx != -1 ? texture3_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex3_idx)) : float4(0.5, 0.5, 0.0, 0.0);

  const Surface surface = Initialize_OBJ(emission, intensity, diffuse, shininess, specular, alpha, tex0_value, tex1_value, tex2_value, tex3_value);
#else
  const float4 tex0_value = tex0_idx != -1 ? texture0_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex0_idx)) : float4(1.0, 1.0, 1.0, 1.0);
  const float4 tex1_value = tex1_idx != -1 ? texture1_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex1_idx)) : float4(0.0, 0.0, 0.0, 0.0);
  const float4 tex2_value = tex2_idx != -1 ? texture2_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex2_idx)) : float4(1.0, 1.0, 1.0, 1.0);
  const float4 tex3_value = tex3_idx != -1 ? texture3_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex3_idx)) : float4(0.5, 0.5, 0.5, 0.0);

  const Surface surface = Initialize_GLTF(emission, intensity, diffuse, shininess, specular, alpha, tex0_value, tex1_value, tex2_value, tex3_value);
#endif

  //#ifdef USE_ALPHA_CLIP
  //  if (surface.alpha < 0.1) discard;
  //#endif
  //
#ifdef USE_NORMAL_MAP
  n = normalize(surface.normal.x * t + surface.normal.y * b + surface.normal.z * n);
#endif

  
  
  const float smoothness = surface.shininess;
  const float3 albedo = surface.diffuse;
  const float metallic = surface.metallic;

#ifdef USE_NORMAL_OCT_QUAD_ENCODING
  const float3 packed_normal = float3(PackNormal(n)) / 255.0;
#else
  const float3 packed_normal = n; 
#endif

  const float3 global_illumination = 0.025 * albedo;

  output.target_0 = float4(albedo, smoothness);
  output.target_1 = float4(packed_normal, metallic);
  output.target_2 = float3(surface.emission + global_illumination);

#ifdef TEST
#endif

  return output;
};