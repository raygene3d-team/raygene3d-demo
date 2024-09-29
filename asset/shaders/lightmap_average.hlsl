
#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

//VK_BINDING(0) cbuffer constant0 : register(b0)
//{
//  uint extent_x       : packoffset(c0.x);
//  uint extent_y       : packoffset(c0.y);
//  uint rnd_base       : packoffset(c0.z);
//  uint rnd_seed       : packoffset(c0.w);
//}


VK_BINDING(0) Texture2DArray<float4> lightmap_accum : register(t0);
VK_BINDING(1) RWTexture2DArray<float4> lightmap_final : register(u0);


[numthreads(8, 8, 1)]
void cs_main(uint3 dispatch_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID, uint3 thread_id : SV_GroupThreadID)
{
  const uint3 pixel_id = { dispatch_id.x, dispatch_id.y, dispatch_id.z };

  const float4 pixel_value = lightmap_accum[pixel_id];
  lightmap_final[pixel_id] = pixel_value;
}
