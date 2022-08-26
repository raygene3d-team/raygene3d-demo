#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#include "common.hlsl"

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

  float3 emission     : packoffset(c3.x);
  float intensity     : packoffset(c3.w);
  float3 diffuse      : packoffset(c4.x);
  float metallic      : packoffset(c4.w);
  float3 specular     : packoffset(c5.x);
  float roughness     : packoffset(c5.w);
  float3 transmit     : packoffset(c6.x);
  float ior           : packoffset(c6.w);

  int tex0_idx        : packoffset(c7.x);
  int tex1_idx        : packoffset(c7.y);
  int tex2_idx        : packoffset(c7.z);
  int tex3_idx        : packoffset(c7.w);

  float debug_color   : packoffset(c8.x);
  uint geometry_idx   : packoffset(c8.w);  

  uint prim_offset    : packoffset(c9.x);
  uint prim_count     : packoffset(c9.y);
  uint vert_offset    : packoffset(c9.z);
  uint vert_count     : packoffset(c9.w);

  uint4 padding[6]    : packoffset(c10.x);
};

VK_BINDING(6) Texture2DArray<float4> texture0_items : register(t0);
VK_BINDING(7) Texture2DArray<float4> texture1_items : register(t1);
VK_BINDING(8) Texture2DArray<float4> texture2_items : register(t2);
VK_BINDING(9) Texture2DArray<float4> texture3_items : register(t3);
VK_BINDING(10) TextureCube<float> shadow_map        : register(t4);

#define USE_NORMAL_MAP
#define USE_ALPHA_CLIP

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
  VK_LOCATION(1) float  u : register1;
  VK_LOCATION(2) float3 nrm : register2;
  VK_LOCATION(3) float  v : register3;
  VK_LOCATION(4) float3 tgn : register4;
  VK_LOCATION(5) float  sign : register5;
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
  output.w_pos_d = float4(input.pos, input.sign);
  output.w_nrm_u = float4(input.nrm, input.u);
  output.w_tng_v = float4(input.tgn, input.v);
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
  const float3 n = normalize(input.w_nrm_u.xyz);
  const float3 t = normalize(input.w_tng_v.xyz);
  const float3 b = input.w_pos_d.w * cross(n, t);
  float3x3 tbn = float3x3(t, b, n);

#ifdef USE_ALPHA_CLIP
  if (tex1_idx != -1)
  {
    const float4 tex_value = texture1_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex1_idx));
    if (tex_value.r < 0.1)
    {
      discard;
    }
  }
#endif

#ifdef USE_NORMAL_MAP
  if (tex3_idx != -1)
  {
    float3 tangent_n = float3(0.0, 0.0, 1.0);
    float3 tangent_t = float3(1.0, 0.0, 0.0);
    float3 tangent_b = float3(0.0, 1.0, 0.0);

    float4 tex_value = texture3_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex3_idx));

    tangent_n = normalize(float3(1.0 - tex_value.xy * 2.0, 0.25));
    tangent_t = normalize(tangent_t - tangent_n * dot(tangent_t, tangent_n));
    tangent_b = cross(tangent_n, tangent_b);
    tbn = mul((float3x3(tangent_t, tangent_b, tangent_n)), tbn);
  }
#endif

  Surface surface = (Surface)0;

  surface.Ke = emission;
  surface.p = intensity;
  
  surface.Kd = diffuse;
  if (tex0_idx != -1)
  {
    const float4 tex_value = texture0_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex0_idx));
    surface.Kd *= tex_value.xyz;
    //surface.d = surface.d * tex_value.a;
  }
  surface.m = metallic;

  surface.Ks = specular;
  if (tex2_idx != -1)
  {
    const float4 tex_value = texture2_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex2_idx));
    surface.Ks *= tex_value.xyz;
    //dissolve_value = dissolve_value * tex_value.a;
  }
  surface.r = roughness;

  surface.Kt = transmit;
  surface.i = ior;

  const float3 surface_pos = input.w_pos_d.xyz;

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - surface_pos);
  const float3 camera_dir = (camera_pos - surface_pos) / camera_dst;

  const float3 shadow_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float shadow_dst = length(shadow_pos - surface_pos);
  const float3 shadow_dir = (shadow_pos - surface_pos) / shadow_dst;
  
  const float3 wo = mul(tbn, camera_dir);
  const float3 lo = mul(tbn, shadow_dir);
  const float attenuation = 10.0 * 1.0 / (shadow_dst * shadow_dst) * Shadow(surface_pos);
  
  const float3 ambient = 0.025 * surface.Kd;
  const float3 diffuse = max(0.0, lo.z) * surface.Kd;
  const float3 specular = pow(max(0.0, normalize(lo + wo).z), surface.r) * surface.Ks;

  const float3 color = ambient + diffuse * attenuation + specular * attenuation;

  PSOutput output;
  output.target_0 = float4(color, 0.0);

  return output;
};
