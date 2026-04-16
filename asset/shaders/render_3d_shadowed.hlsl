#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#include "packing.hlsl"
#include "surface.hlsl"
#include "brdf.hlsl"

VK_BINDING(0)  SamplerComparisonState sampler0 : register(s0);

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
VK_BINDING(5) Texture2D<float4> gbuffer_1_texture : register(t1);
VK_BINDING(6) Texture2D<float> depth_texture : register(t2);
VK_BINDING(7) Texture2D<float> shadow_map  : register(t3);


static const float4x4 poisson_disk = float4x4(
  float4(-0.37192261, -0.89699117, 0.47109537, 0.74092316),
  float4(-0.6807477, 0.38263656, 0.69048062, -0.66119884),
  float4(0.11615683, -0.08193062, -0.91318524, -0.21275223),
  float4(0.93688596, 0.05578877, -0.16948928, 0.84225623)
  );

float Shadow(const float3 w_pos)
{
  float4x4 shadow_bias = float4x4
  (
    0.5, 0.0, 0.0, 0.5,
    0.0,-0.5, 0.0, 0.5,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
  );
    
  float4 s_pos = mul(shadow_bias, mul(shadow_proj, mul(shadow_view, float4(w_pos, 1.0))));
  
  
  //s_pos.x = s_pos.x * 0.5 + 0.5;
  //s_pos.y = 0.5 - 0.5 * s_pos.y;
  
  //const float3 light_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  //const float3 view_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  //const float3 tc = w_pos - light_pos;
  //
  //const float m22 = shadow_proj[2][2];
  //const float m23 = shadow_proj[2][3];
  //
  //const float near = -m23 / m22;
  //const float far = near / (m22 - 1.0);
  //
  //const float3 atc = abs(tc);
  //const float m = max(max(atc.x, atc.y), atc.z);
  //const float3 tk = step(m, atc);
  //
  //const float3 dx = normalize(cross(tc, w_pos - view_pos));
  //const float3 dy = normalize(cross(tc, dx));

  const float d = shadow_map.SampleCmpLevelZero(sampler0, s_pos.xy / s_pos.w, s_pos.z / s_pos.w);

  //const float cd = far * (1.0 - near / (dot(tk, atc))) / (far - near);
  //const float blur_radius = 0.005;
  //
  //float shadow = step(d, cd);
  //for (uint x = 0; x < 8; ++x)
  //{
  //  const float3 offset = (dx * poisson_disk[x / 2][2 * (x % 2)] + dy * poisson_disk[x / 2][2 * (x % 2) + 1]) * blur_radius;
  //  const float d = shadow_map.Sample(sampler0, normalize(tc) + offset);
  //  shadow += step(d, cd);
  //}
  //shadow /= 9.0;
  //shadow *= sign(d);
  
  //s_pos.z = 0;

  return d;
}


struct VSInput
{
  VK_LOCATION(0) float2 pos : register0;
};

struct VSOutput
{
  VK_LOCATION(0) float4 pos : SV_Position;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output = (VSOutput)0;

  output.pos = float4(input.pos, 1.0, 1.0);

  return output;
}

struct PSInput
{
  VK_LOCATION(0) float4 pos : SV_Position;
};

struct PSOutput
{
  float4 target_0 : SV_Target0;
};

PSOutput ps_main(PSInput input)
{
  PSOutput output = (PSOutput)0;

  const float depth = depth_texture.Load(int3(input.pos.xy, 0));

  if (depth == 1.0)
  {
    discard;
  }

  const float4 albedo_metallic = gbuffer_0_texture.Load(int3(input.pos.xy, 0));
  const float4 normal_smoothness = gbuffer_1_texture.Load(int3(input.pos.xy, 0));

  const float3 normal = UnpackNormal(normal_smoothness.rgb);
  const float metallic = albedo_metallic.a;
  const float smoothness = normal_smoothness.a;

  const float rx = 2.0 * input.pos.x / extent_x - 1.0;
  const float ry = 2.0 * input.pos.y / extent_y - 1.0;

  const float4 ndc_coord = float4(rx, -ry, depth, 1.0);
  const float4 view_pos = mul(camera_proj_inv, ndc_coord);
  const float3 surface_pos = mul(camera_view_inv, float4(view_pos.xyz / view_pos.w, 1.0)).xyz;

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - surface_pos);
  const float3 camera_dir = (camera_pos - surface_pos) / camera_dst;

  const float3 shadow_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float shadow_dst = length(shadow_pos - surface_pos);
  const float3 shadow_dir =-float3(shadow_view_inv[0][2], shadow_view_inv[1][2], shadow_view_inv[2][2]); 
  
  const float3 diffuse = max(0.0, dot(shadow_dir, normal)) * albedo_metallic.xyz;
  const float3 specular = pow(max(0.0, dot(normalize(camera_dir + shadow_dir), normal)), smoothness);
  const float3 attenuation = Shadow(surface_pos);
  
  BRDF_CookTorrance brdf; // = Initialize_CookTorrance();
  brdf.color = albedo_metallic.xyz;
  brdf.roughness = clamp(normal_smoothness.w, 0.001, 0.999);
  brdf.metallic = albedo_metallic.w;

  const float3 n = normalize(normal.xyz);
  const float3 t = normalize(n.y * n.y > n.x * n.x ? float3(0.0, -n.z, n.y) : float3(-n.z, 0.0, n.x));
  const float3 b = normalize(cross(n, t));
  const float3x3 tbn = float3x3(t, b, n);

  const float3 wo = mul((tbn), shadow_dir);
  const float3 lo = mul((tbn), camera_dir);

  const float3 m = Evaluate_CookTorrance(brdf, lo, wo) * brdf.color;

  const float oneMinusReflectivity = OneMinusReflectivityMetallic(brdf.metallic);
  const float reflectivity = 1.0 - oneMinusReflectivity;

  const float3 diff = brdf.color * oneMinusReflectivity;
  const float3 spec = lerp(kDielectricSpec.rgb, m, metallic);

  const float3 color = (diff + spec) * max(0.0, wo.z) * attenuation;
  output.target_0 = float4(color, 1.0);
  
  return output;
}