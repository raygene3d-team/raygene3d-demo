
#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

VK_BINDING(0) cbuffer Camera : register(b0)
{
  uint cam_size_x       : packoffset(c0.x);
  uint cam_size_y       : packoffset(c0.y);
  float cam_fov_x       : packoffset(c0.z);
  float cam_fov_y       : packoffset(c0.w);
  float cam_lens_r      : packoffset(c1.x);
  float cam_dist_f      : packoffset(c1.y);
  uint cam_base         : packoffset(c1.z);
  uint cam_seed         : packoffset(c1.w);
  float4x4 cam_view     : packoffset(c2.x);
  float4x4 cam_proj     : packoffset(c6.x);
  float4x4 cam_view_inv : packoffset(c10.x);
  float4x4 cam_proj_inv : packoffset(c14.x);
}


VK_BINDING(1) Texture2D<float4> render_target_0 : register(t0);
VK_BINDING(2) RWTexture2D<float4> presentation_buffer : register(u0);



[numthreads(8, 8, 1)]
void cs_main(uint3 dispatch_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID, uint3 thread_id : SV_GroupThreadID)
{
  const uint2 pixel_id = { dispatch_id.x, dispatch_id.y };
  const uint item_id = cam_size_x * dispatch_id.y + dispatch_id.x;

  const float4 pixel_color = render_target_0[pixel_id];
  presentation_buffer[pixel_id] = pow(max(0.0, pixel_color), 1.0 / 2.2);
}
