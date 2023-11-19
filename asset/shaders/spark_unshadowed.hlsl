
#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#include "packing.hlsl"

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


VK_BINDING(3) Texture2D<float4> render_target_0 : register(t0);
VK_BINDING(4) Texture2D<float4> render_target_1 : register(t1);
VK_BINDING(5) Texture2D<float4> render_target_2 : register(t2);

VK_BINDING(6) Texture2D<float> depth_texture : register(t3);

VK_BINDING(7) RWTexture2D<float4> out_buffer : register(u0);


[numthreads(8, 8, 1)]
void cs_main(uint3 dispatch_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID, uint3 thread_id : SV_GroupThreadID)
{
  const uint2 pixel_id = { dispatch_id.x, dispatch_id.y };
  const uint item_id = extent_x * dispatch_id.y + dispatch_id.x;

  const float4 w_alb_m = render_target_0[pixel_id];
  const float4 w_nrm_s = render_target_1[pixel_id];
  const float4 w_ems_a = render_target_2[pixel_id];

  const float3 normal = UnpackNormal(w_nrm_s.rgb);

  const float depth = depth_texture[pixel_id];
  float4 ndc_coord = float4(2.0 * pixel_id / float2(extent_x, extent_y) - 1.0, depth, 1.0);
  ndc_coord.y *= -1;
  const float4 view_pos = mul(camera_proj_inv, ndc_coord);
  const float3 surface_pos = mul(camera_view_inv, float4(view_pos.xyz / view_pos.w, 1.0)).xyz;

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - surface_pos);
  const float3 camera_dir = (camera_pos - surface_pos) / camera_dst;

  const float3 shadow_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float shadow_dst = length(shadow_pos - surface_pos);
  const float3 shadow_dir = (shadow_pos - surface_pos) / shadow_dst;

  const float3 diffuse = max(0.0, dot(shadow_dir, normal)) * w_alb_m.xyz;
  const float3 specular = pow(max(0.0, dot(normalize(camera_dir + shadow_dir), normal)), w_nrm_s.a);
  const float3 ambient = 0.025 * w_alb_m.xyz;
  const float attenuation = 1.0;

  const float3 color = ambient + diffuse * attenuation;

  out_buffer[pixel_id] = float4(normal, 1);
}