#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#define USE_NORMAL_MAP
//#define USE_ALPHA_CLIP


#include "common.hlsl"
#include "surface.hlsl"
#include "brdf.hlsl"

VK_BINDING(0) sampler sampler0 : register(s0);
VK_BINDING(1) sampler sampler1 : register(s1);

VK_BINDING(2) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}

VK_BINDING(3) cbuffer constant1 : register(b1)
{
  float4x4 camera_view     : packoffset(c0.x);
  float4x4 camera_proj     : packoffset(c4.x);
  float4x4 camera_view_inv : packoffset(c8.x);
  float4x4 camera_proj_inv : packoffset(c12.x);
}

VK_BINDING(4) cbuffer constant2 : register(b2)
{
  float4x4 shadow_view     : packoffset(c0.x);
  float4x4 shadow_proj     : packoffset(c4.x);
  float4x4 shadow_view_inv : packoffset(c8.x);
  float4x4 shadow_proj_inv : packoffset(c12.x);
}

VK_BINDING(5) cbuffer constant3 : register(b3)
{
  float4x3 transform  : packoffset(c0.x);

  uint prim_offset    : packoffset(c3.x);
  uint prim_count     : packoffset(c3.y);
  uint vert_offset    : packoffset(c3.z);
  uint vert_count     : packoffset(c3.w);

  float3 emission     : packoffset(c4.x);
  float intensity     : packoffset(c4.w);
  float3 diffuse      : packoffset(c5.x);
  float shininess     : packoffset(c5.w);
  float3 specular     : packoffset(c6.x);
  float alpha         : packoffset(c6.w);

  int tex0_idx : packoffset(c7.x);
  int tex1_idx : packoffset(c7.y);
  int tex2_idx : packoffset(c7.z);
  int tex3_idx : packoffset(c7.w);

  float debug_color : packoffset(c8.x);
  uint geometry_idx   : packoffset(c8.w);

  uint4 padding[7]    : packoffset(c9.x);
};

VK_BINDING(6) Texture2DArray<float4> texture0_items : register(t0);
VK_BINDING(7) Texture2DArray<float4> texture1_items : register(t1);
VK_BINDING(8) Texture2DArray<float4> texture2_items : register(t2);
VK_BINDING(9) Texture2DArray<float4> texture3_items : register(t3);
VK_BINDING(10) TextureCube<float> shadow_map        : register(t4);

#define M_PI 3.14159265359

static const float4x4 poisson_disk = float4x4(
  float4(-0.37192261, -0.89699117, 0.47109537, 0.74092316), 
  float4(-0.6807477, 0.38263656, 0.69048062, -0.66119884),
  float4(0.11615683, -0.08193062, -0.91318524, -0.21275223),
  float4(0.93688596, 0.05578877, -0.16948928, 0.84225623)
);

struct VSInput
{
  VK_LOCATION(0) float3 pos : register0;
  VK_LOCATION(1) float4 col : register1;
  VK_LOCATION(2) float3 nrm : register2;
  VK_LOCATION(3) uint msk   : register3;
  VK_LOCATION(4) float3 tgn : register4;
  VK_LOCATION(5) float sgn  : register5;
  VK_LOCATION(6) float2 tc0 : register6;
  VK_LOCATION(7) float2 tc1 : register7;
};

struct VSOutput
{
  VK_LOCATION(0) float4 w_pos_d : register0;
  VK_LOCATION(1) float4 w_nrm_u : register1;
  VK_LOCATION(2) float4 w_tng_v : register2;
  VK_LOCATION(3) float4 pos   : SV_Position;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;

  output.pos = mul(camera_proj, mul(camera_view, float4(input.pos, 1.0)));
  output.w_pos_d = float4(input.pos, input.sgn);
  output.w_nrm_u = float4(input.nrm, input.tc0.x);
  output.w_tng_v = float4(input.tgn, input.tc0.y);
  return output;
}

struct PSInput
{
  VK_LOCATION(0) float4 w_pos_d : register0;
  VK_LOCATION(1) float4 w_nrm_u : register1;
  VK_LOCATION(2) float4 w_tng_v : register2;
};

struct PSOutput
{
  float4 target_0 : SV_Target0;
};

float Shadow(const float3 w_pos)
{
  const float3 light_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float3 view_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float3 tc = w_pos - light_pos;

  const float m22 = shadow_proj[2][2];
  const float m23 = shadow_proj[2][3];

  const float near = -m23 / m22;
  const float far = near / (m22 - 1.0);  

  const float3 atc = abs(tc);
  const float m = max(max(atc.x, atc.y), atc.z);
  const float3 tk = step(m, atc);

  const float3 dx = normalize(cross(tc, w_pos - view_pos));
  const float3 dy = normalize(cross(tc, dx));

  const float d = shadow_map.Sample(sampler1, normalize(tc));

  const float cd = far * (1.0 - near / (dot(tk, atc))) / (far - near);
  const float blur_radius = 0.005;

  float shadow = step(d, cd);
  for(uint x = 0; x < 8; ++x)
  {
    const float3 offset = (dx * poisson_disk[x / 2][2 * (x % 2)] + dy * poisson_disk[x / 2][2 * (x % 2) + 1]) * blur_radius;
    const float d = shadow_map.Sample(sampler1, normalize(tc) + offset);
    shadow += step(d, cd);
  }
  shadow /= 9.0;
  shadow *= sign(d);

  return 1.0 - shadow;
}

PSOutput ps_main(PSInput input)
{
  float3 n = normalize(input.w_nrm_u.xyz);
  float3 t = normalize(input.w_tng_v.xyz);
  float3 b = input.w_pos_d.w * cross(n, t);

  const float4 tex0_value = tex0_idx != -1 ? texture0_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex0_idx)) : float4(1.0, 1.0, 1.0, 1.0);
  const float4 tex1_value = tex1_idx != -1 ? texture1_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex1_idx)) : float4(1.0, 1.0, 1.0, 1.0);
  const float4 tex2_value = tex2_idx != -1 ? texture2_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex2_idx)) : float4(1.0, 1.0, 1.0, 1.0);
  const float4 tex3_value = tex3_idx != -1 ? texture3_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex3_idx)) : float4(0.5, 0.5, 0.0, 0.0);

  const Surface surface = Initialize_OBJ(emission, intensity, diffuse, shininess, specular, alpha, tex0_value, tex1_value, tex2_value, tex3_value);

#ifdef USE_ALPHA_CLIP
  if (surface.alpha < 0.1) discard;
#endif

#ifdef USE_NORMAL_MAP
  n = normalize(surface.normal.x * t + surface.normal.y * b + surface.normal.z * n);
  t = normalize(t - n * dot(t, n));
  b = cross(t, n);
#endif

  const float3x3 tbn = float3x3(t, b, n);
  const float3x3 inverse_tbn = InverseTBN(tbn);

  const float3 surface_pos = input.w_pos_d.xyz;

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - surface_pos);
  const float3 camera_dir = (camera_pos - surface_pos) / camera_dst;

  const float3 shadow_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float shadow_dst = length(shadow_pos - surface_pos);
  const float3 shadow_dir = (shadow_pos - surface_pos) / shadow_dst;

  const float3 wo = mul(tbn, camera_dir);
  const float3 lo = mul(tbn, shadow_dir);
  const float attenuation = 10.0 * 1.0 / (shadow_dst * shadow_dst) * max(0.0, lo.z) * Shadow(surface_pos);

  const float3 ambient = 0.025 * surface.diffuse;

  //const float3 diffuseColor = surface.Kd; // *(1.0 - surface.m);
  //const float3 specularColor = surface.Kd * surface.m; // lerp(kDielectricSpec.rgb, surface.Kd, surface.m);

  const float3 diffuse = Evaluate_Lambert(Initialize_Lambert(surface), lo, wo) * surface.diffuse;

  //data_blinn_phong.color = specularColor;
  //data_blinn_phong.shininess = 2 /(surface.r * surface.r) - 2;
  const float3 specular = Evaluate_BlinnPhong(Initialize_BlinnPhong(surface), lo, wo) * surface.diffuse;

  const float3 color = ambient + diffuse * attenuation + specular * attenuation;

  PSOutput output;
  output.target_0 = float4(color, 0.0);

  return output;
};
