
#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#include "traverse.hlsl"

VK_BINDING(0)  SamplerComparisonState sampler0 : register(s0);

VK_BINDING(1) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}

VK_BINDING(2) StructuredBuffer<Box> 		t_boxes : register(t0);
VK_BINDING(3) StructuredBuffer<Box> 		b_boxes : register(t1);
VK_BINDING(4) StructuredBuffer<Instance> 	inst_items : register(t2);
VK_BINDING(5) StructuredBuffer<Primitive> 	prim_items : register(t3);
VK_BINDING(6) StructuredBuffer<Vertex> 		vert_items : register(t4);

VK_BINDING(7) Texture2DArray<float4> texture0_items : register(t5);
VK_BINDING(8) Texture2DArray<float4> texture1_items : register(t6);
VK_BINDING(9) Texture2DArray<float4> texture2_items : register(t7);
VK_BINDING(10) Texture2DArray<float4> texture3_items : register(t8);
VK_BINDING(11) Texture2DArray<float4> texture4_items : register(t9);
VK_BINDING(12) Texture2DArray<float4> texture5_items : register(t10);
VK_BINDING(13) Texture2DArray<float4> texture6_items : register(t11);
VK_BINDING(14) Texture2DArray<float4> texture7_items : register(t12);

VK_BINDING(15) Texture2DArray<uint4> lightmap_input : register(t13);
VK_BINDING(16) RWTexture2DArray<float4> lightmap_accum : register(u0);


[numthreads(8, 8, 1)]
void cs_main(uint3 dispatch_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID, uint3 thread_id : SV_GroupThreadID)
{
  const uint3 pixel_id = { dispatch_id.x, dispatch_id.y, dispatch_id.z };

  const uint4 pixel_value = lightmap_input[pixel_id];
  
  const uint inst_idx = pixel_value.x;
  const uint prim_idx = pixel_value.y;
  
  if(inst_idx == -1 || prim_idx == -1) return;
  
  const Instance instance = inst_items[inst_idx];
  const Primitive primitive = prim_items[instance.prim_offset + prim_idx];
  const Vertex vertex0 = vert_items[instance.vert_offset + primitive.idx0];
  const Vertex vertex1 = vert_items[instance.vert_offset + primitive.idx1];
  const Vertex vertex2 = vert_items[instance.vert_offset + primitive.idx2];
  
  const float u = asfloat(pixel_value.z);
  const float v = asfloat(pixel_value.w);
  const float w = 1.0 - u - v;
  
  const float3 pos = vertex0.pos * u + vertex1.pos * v + vertex2.pos * w;
  const float3 nrm = vertex0.nrm * u + vertex1.nrm * v + vertex2.nrm * w;
  const float2 tc0 = vertex0.tc0 * u + vertex1.tc0 * v + vertex2.tc0 * w;
  
  //const float2 tc1 = vertex0.tc1 * u + vertex1.tc1 * v + vertex2.tc1 * w;
  //const float3 col = vertex0.col * u + vertex1.col * v + vertex2.col * w;
  
  lightmap_accum[pixel_id] = float4(nrm, 0.0);//pixel_value;
}
