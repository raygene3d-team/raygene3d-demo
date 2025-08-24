
#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

static const float PI = 3.14159265;
static const uint MAX_SAMPLES = 8192 * 8;
static const float3 RAY_TOFF = float3(0.005, 0.005, 0.005);
static const uint BOUNCE_COUNT = 8; 

#include "traverse.hlsl"
#include "random.hlsl"

VK_BINDING(0)  SamplerComparisonState sampler0 : register(s0);

VK_BINDING(1) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}

VK_BINDING(2) StructuredBuffer<Box> 		    buffer_tbox : register(t0);
VK_BINDING(3) StructuredBuffer<Box> 		    buffer_bbox : register(t1);
VK_BINDING(4) StructuredBuffer<Instance> 	  buffer_inst : register(t2);
VK_BINDING(5) StructuredBuffer<Primitive> 	buffer_trng : register(t3);
VK_BINDING(6) StructuredBuffer<Vertex> 		  buffer_vert : register(t4);

VK_BINDING(7) Texture2DArray<float4> array_aaam : register(t5);
VK_BINDING(8) Texture2DArray<float4> array_snno : register(t6);
VK_BINDING(9) Texture2DArray<float4> array_eeet : register(t7);

VK_BINDING(10) Texture2DArray<uint4> lightmap_input : register(t8);
VK_BINDING(11) RWTexture2DArray<float4> lightmap_accum : register(u0);


float nrand(float2 uv)
{
  return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

float3 SampleLambertBRDF(in float3 color, in float3 wo, in float s0, in float s1, out float3 wi, out float pdf)
{
  const float cos_theta = sqrt(s0);
  const float sin_theta = sqrt(1.0 - s0);
  const float cos_phi = cos(2.0 * PI * s1);
  const float sin_phi = sin(2.0 * PI * s1);
  wi = float3(cos_phi * sin_theta, sin_phi * sin_theta, cos_theta);
  pdf = cos_theta * (1.0 / PI);

  //const float cos_theta = s0;
  //const float sin_theta = sqrt(1.0 - s0 * s0);
  //const float sin_phi = sin(2.0 * PI * s1);
  //const float cos_phi = cos(2.0 * PI * s1);
  //wi = float3(cos_phi * sin_theta, sin_phi * sin_theta, cos_theta);
  //pdf = 1.0 / (2.0 * PI);

  const float3 tput = 1.0 / (2.0 * PI) * cos_theta * color; // / pdf;
  return tput;
}

[numthreads(8, 8, 1)]
void cs_main(uint3 dispatch_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID, uint3 thread_id : SV_GroupThreadID)
{
  const uint3 pixel_id = { dispatch_id.x, dispatch_id.y, dispatch_id.z };
  
  const uint4 pixel_value = lightmap_input[pixel_id];
  const uint inst_idx = pixel_value.x;
  const uint prim_idx = pixel_value.y;
  
  if(inst_idx == -1 || prim_idx == -1) return;
  
  const uint seed = pixel_id.x | (pixel_id.y << 16);  
  const uint base = rnd_base;

  //const float2 rand = nrand(thread_id.xy / float2(extent_x, extent_y));

  //const uint sample_index = base; //seed * MAX_SAMPLES + base % MAX_SAMPLES;
  //const uint sample_scramble = seed; //rand * 2.3283064365386963e+10; // random_seed[pixel_index];
  
  uint state = RandomSampler_init(seed, base);
  const float s0 = RandomSampler_sample(state);
  const float s1 = RandomSampler_sample(state);
   
  const Instance instance = buffer_inst[inst_idx];
  const Primitive primitive = buffer_trng[instance.prim_offset + prim_idx];
  const Vertex vertex0 = buffer_vert[instance.vert_offset + primitive.idx0];
  const Vertex vertex1 = buffer_vert[instance.vert_offset + primitive.idx1];
  const Vertex vertex2 = buffer_vert[instance.vert_offset + primitive.idx2];
  
  const float u = asfloat(pixel_value.z);
  const float v = asfloat(pixel_value.w);
  const float w = 1.0 - u - v;
  
  const float3 pos = vertex0.pos * u + vertex1.pos * v + vertex2.pos * w;
  const float3 nrm = vertex0.nrm * u + vertex1.nrm * v + vertex2.nrm * w;
  const float2 tc0 = vertex0.tc0 * u + vertex1.tc0 * v + vertex2.tc0 * w;
  
  const float3 n = normalize(nrm);
  const float3 t = normalize(n.y * n.y > n.x * n.x ? float3(0.0, -n.z, n.y) : float3(-n.z, 0.0, n.x));
  const float3 b = normalize(cross(n, t));
  const float3x3 tbn = float3x3(t, b, n);
  
  const float cos_theta = sqrt(s0);
  const float sin_theta = sqrt(1.0 - s0);
  const float cos_phi = cos(2.0 * PI * s1);
  const float sin_phi = sin(2.0 * PI * s1);
  const float3 wi = float3(cos_phi * sin_theta, sin_phi * sin_theta, cos_theta);
  const float pdf = cos_theta * (1.0 / PI);
  
  RayHit rayhit = (RayHit)0;
  rayhit.org = pos;
  rayhit.tmin = 0.0001;
  rayhit.dir = mul(transpose(tbn), normalize(wi));
  rayhit.tmax = 0.25;
  
  OccludeScene(rayhit, buffer_tbox, buffer_bbox, buffer_inst, buffer_trng, buffer_vert);
  const float3 tput = rayhit.dist == rayhit.tmax ? float3(1.0, 1.0, 1.0) : float3(0.0, 0.0, 0.0);
  
  //const float2 duvdx = float2(0.0, 0.0);
  //const float2 duvdy = float2(0.0, 0.0);
  //const float3 color = instance.tex0_idx != -1 
  //  ? texture0_items.SampleGrad(sampler0, float3(tc0, instance.tex0_idx), duvdx, duvdy) 
  //  : float4(1.0, 1.0, 1.0, 1.0);
  
  //float3 wi = float3(0.0, 0.0, 0.0);
  //float pdf = 1.0;
  //float3 tput_d = SampleLambertBRDF(color, wo, s0, s1, wi, pdf);
  
  //const float2 tc1 = vertex0.tc1 * u + vertex1.tc1 * v + vertex2.tc1 * w;
  //const float3 col = vertex0.col * u + vertex1.col * v + vertex2.col * w;
  
  lightmap_accum[pixel_id] += float4(tput, 1.0); //pixel_value;
}
