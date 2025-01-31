#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#include "packing.hlsl"
#include "traverse.hlsl"
#include "surface.hlsl"
#include "brdf.hlsl"

VK_BINDING(0) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}

VK_BINDING(1) cbuffer constant1 : register(b1)
{
  float4x4 camera_view     : packoffset(c0.x);
  float4x4 camera_proj     : packoffset(c4.x);
  float4x4 camera_view_inv : packoffset(c8.x);
  float4x4 camera_proj_inv : packoffset(c12.x);
}

VK_BINDING(2) cbuffer constant2 : register(b2)
{
  float4x4 shadow_view     : packoffset(c0.x);
  float4x4 shadow_proj     : packoffset(c4.x);
  float4x4 shadow_view_inv : packoffset(c8.x);
  float4x4 shadow_proj_inv : packoffset(c12.x);
}

VK_BINDING(3) StructuredBuffer<Box> 		t_boxes : register(t0);
VK_BINDING(4) StructuredBuffer<Box> 		b_boxes : register(t1);
VK_BINDING(5) StructuredBuffer<Instance> 	inst_items : register(t2);
VK_BINDING(6) StructuredBuffer<Primitive> 	prim_items : register(t3);
VK_BINDING(7) StructuredBuffer<Vertex> 		vert_items : register(t4);

VK_BINDING(8) Texture2D<float4> gbuffer_0_texture : register(t5);
VK_BINDING(9) Texture2D<float4> gbuffer_1_texture : register(t6);
VK_BINDING(10) Texture2D<float> depth_texture : register(t7);

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
  if (depth == 1.0) discard;
  
  const float rx = 2.0 * input.pos.x / extent_x - 1.0;
  const float ry = 2.0 * input.pos.y / extent_y - 1.0;

  const float4 pnt = mul(camera_proj_inv, float4(rx, -ry, depth, 1.0));
  const float3 pos = pnt.xyz / pnt.w;
  
  const float3 camera_org = mul(camera_view_inv, float4(0.0, 0.0, 0.0, 1.0)).xyz;
  const float3 camera_dir = mul(camera_view_inv, float4(normalize(pos), 0.0)).xyz;

  const float3 shadow_org = mul(shadow_view_inv, float4(0.0, 0.0, 0.0, 1.0)).xyz;
  const float3 shadow_dir = mul(shadow_view_inv, float4(0.0, 0.0, 1.0, 0.0)).xyz;
  
  const float3 surface_pos = mul(camera_view_inv, float4(pos, 1.0)).xyz;
  const float3 surface_dir = -shadow_dir;

  RayHit rayhit = (RayHit)0;
  rayhit.org = surface_pos;
  rayhit.tmin = RAY_TMIN;
  rayhit.dir = surface_dir;
  rayhit.tmax = RAY_TMAX;

  
  OccludeScene(rayhit, t_boxes, b_boxes, inst_items, prim_items, vert_items);
  const float attenuation = rayhit.dist == rayhit.tmax ? 1.0 : 0.0;
  
  const float4 albedo_metallic = gbuffer_0_texture.Load(int3(input.pos.xy, 0));
  const float4 normal_smoothness = gbuffer_1_texture.Load(int3(input.pos.xy, 0));

  const float3 normal = 2.0 * normal_smoothness.rgb - 1.0;
  const float metallic = albedo_metallic.a;
  const float smoothness = normal_smoothness.a;
  
  BRDF_CookTorrance brdf; // = Initialize_CookTorrance();
  brdf.color = albedo_metallic.xyz;
  brdf.roughness = clamp(normal_smoothness.w, 0.001, 0.999);
  brdf.metallic = albedo_metallic.w;

  const float3 n = normalize(normal.xyz);
  const float3 t = normalize(n.y * n.y > n.x * n.x ? float3(0.0, -n.z, n.y) : float3(-n.z, 0.0, n.x));
  const float3 b = normalize(cross(n, t));
  const float3x3 tbn = float3x3(t, b, n);

  const float3 wo = mul(tbn, -shadow_dir);
  const float3 lo = mul(tbn, -camera_dir);

  const float3 m = Evaluate_CookTorrance(brdf, lo, wo) * brdf.color;

  const float oneMinusReflectivity = OneMinusReflectivityMetallic(brdf.metallic);
  const float reflectivity = 1.0 - oneMinusReflectivity;

  const float3 diff = brdf.color * oneMinusReflectivity;
  const float3 spec = lerp(kDielectricSpec.rgb, m, metallic);

  const float3 color = (diff + spec) * max(0.0, wo.z) * attenuation;

  //const float3 color = attenuation;
  output.target_0 = float4(color, 1.0);
  
  return output;
}