
#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

VK_BINDING(0) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}


VK_BINDING(1) Texture2D<float3> render_target_0 : register(t0);
VK_BINDING(2) RWTexture2D<float4> presentation_buffer : register(u0);


[numthreads(8, 8, 1)]
void cs_main(uint3 dispatch_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID, uint3 thread_id : SV_GroupThreadID)
{
  const uint2 pixel_id = { dispatch_id.x, dispatch_id.y };
  const uint item_id = extent_x * dispatch_id.y + dispatch_id.x;

  const float4 pixel_color = float4(render_target_0[pixel_id], 1.0);
  presentation_buffer[pixel_id] = pow(max(0.0, pixel_color), 1.0 / 2.2);
}
