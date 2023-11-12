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


VK_BINDING(4) Texture2D<float4> render_target_0 : register(t0);
VK_BINDING(5) Texture2D<float4> render_target_1 : register(t1);
VK_BINDING(6) Texture2D<float4> render_target_2 : register(t2);

VK_BINDING(7) Texture2D<float> depth_texture : register(t3);
VK_BINDING(8) TextureCube<float> shadow_map  : register(t4);
VK_BINDING(9) RWTexture2D<float4> out_buffer : register(u0);


static const float4x4 poisson_disk = float4x4(
  float4(-0.37192261, -0.89699117, 0.47109537, 0.74092316),
  float4(-0.6807477, 0.38263656, 0.69048062, -0.66119884),
  float4(0.11615683, -0.08193062, -0.91318524, -0.21275223),
  float4(0.93688596, 0.05578877, -0.16948928, 0.84225623)
  );

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

  const float d = shadow_map.SampleLevel(sampler0, normalize(tc), 0);

  const float cd = far * (1.0 - near / (dot(tk, atc))) / (far - near);
  const float blur_radius = 0.005;

  float shadow = step(d, cd);
  for (uint x = 0; x < 8; ++x)
  {
    const float3 offset = (dx * poisson_disk[x / 2][2 * (x % 2)] + dy * poisson_disk[x / 2][2 * (x % 2) + 1]) * blur_radius;
    const float d = shadow_map.SampleLevel(sampler0, normalize(tc) + offset, 0);
    shadow += step(d, cd);
  }
  shadow /= 9.0;
  shadow *= sign(d);

  return 1.0 - shadow;
}

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
  const float2 screen_coords = 2.0 * (float2(pixel_id.x, extent_y - pixel_id.y) + float2(0.5, -0.5)) / float2(extent_x, extent_y) - 1.0;
  float4 world_pos = mul(camera_view_inv, mul(camera_proj_inv, float4(screen_coords, depth, 1.0)));
  float3 surface_pos = world_pos.xyz / world_pos.w;

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - surface_pos);
  const float3 camera_dir = (camera_pos - surface_pos) / camera_dst;

  const float3 shadow_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float shadow_dst = length(shadow_pos - surface_pos);
  const float3 shadow_dir = (shadow_pos - surface_pos) / shadow_dst;

  const float3 diffuse = max(0.0, dot(shadow_dir, normal)) * w_alb_m.xyz;
  const float3 specular = pow(max(0.0, dot(normalize(camera_dir + shadow_dir), normal)), w_nrm_s.a);
  const float3 ambient = 0.025 * w_alb_m.xyz;
  const float attenuation = Shadow(surface_pos);

  const float3 color = ambient + diffuse * attenuation;

  out_buffer[pixel_id] = float4(color, 1);
}