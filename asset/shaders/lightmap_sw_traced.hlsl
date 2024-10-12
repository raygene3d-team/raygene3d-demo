
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

VK_BINDING(0)  SamplerComparisonState sampler0 : register(s0);

VK_BINDING(1) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}

VK_BINDING(2) StructuredBuffer<Box> 		inst_boxes : register(t0);
VK_BINDING(3) StructuredBuffer<Box> 		prim_boxes : register(t1);
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


Hit IntersectScene(in Ray ray, out float dist)
{
  Hit hit;
  hit.geom = uint2(-1, -1);
  hit.bary = float2(0.0, 0.0);


  //StructuredBuffer<Box> mesh_nodes = mesh_boxes;
  dist = ray.tmax;

  const uint inst_offset = 0;
  const uint inst_count = inst_boxes[inst_offset].count;

  uint inst_stride = 0;
  while (inst_stride < inst_count)
  {
    const Box inst_node = inst_boxes[inst_stride + inst_offset];
    if(inst_node.count == 1) //Instance leaf
    {
      const int inst_idx = inst_node.offset;
      const Instance instance = inst_items[inst_idx];

      const uint prim_offset = 2 * instance.prim_offset - inst_idx;
      const uint prim_count = prim_boxes[prim_offset].count;

      uint prim_stride = 0;
      while (prim_stride < prim_count)
      {
        const Box prim_node = prim_boxes[prim_stride + prim_offset];
        if (prim_node.count == 1) //Primitive leaf
        {
          const int prim_idx = prim_node.offset;
          const Primitive primitive = prim_items[instance.prim_offset + prim_idx];

#ifdef USE_CONSISTENT_BVH
          const float3 center = f16tof32(asuint(prim_node.min));
          const float3 delta0 = f16tof32(asuint(prim_node.min) >> 16);
          const float3 delta1 = f16tof32(asuint(prim_node.max));
          const float3 delta2 = f16tof32(asuint(prim_node.max) >> 16);

          const float3 pos0 = center + delta0;
          const float3 pos1 = center + delta1;
          const float3 pos2 = center + delta2;
#else
          const float3 pos0 = vert_items[instance.vert_offset + primitive.idx0].pos;
          const float3 pos1 = vert_items[instance.vert_offset + primitive.idx1].pos;
          const float3 pos2 = vert_items[instance.vert_offset + primitive.idx2].pos;
#endif

          float t, u, v;
          if (CheckTriangle(pos0, pos1, pos2, ray.org, ray.tmin, ray.dir, dist, t, u, v))
          {
            if (dist >= t)
            {
              bool alpha_clip = false;
#ifdef USE_ALPHA_CLIP
              if (instance.tex1_idx != -1)
              {
                const Vertex vertex0 = vertex_items[instance.vert_offset + primitive.idx0];
                const Vertex vertex1 = vertex_items[instance.vert_offset + primitive.idx1];
                const Vertex vertex2 = vertex_items[instance.vert_offset + primitive.idx2];

                const float3 weights = float3(1.0 - u - v, u, v);
                const float u = dot(float3(vertex0.u, vertex1.u, vertex2.u), weights);
                const float v = dot(float3(vertex0.v, vertex1.v, vertex2.v), weights);

                uint tex_w = 0;
                uint tex_h = 0;
                uint tex_n = 0;
                texture1_items.GetDimensions(tex_w, tex_h, tex_n);
                const float4 tex_value = texture1_items.Load(int4(abs(frac(u)) * tex_w, abs(frac(v)) * tex_h, instance.tex1_idx, 0));
                
                alpha_clip = tex_value.r > 0.1 ? false : true;
              }
#endif
              if (!alpha_clip)
              {
                dist = t;
                hit.bary = float2(u, v);
                hit.geom = uint2(inst_idx, prim_idx);
              }
            }
          }
          prim_stride += 1;
          continue;
        }
        const float3 prim_bmin = prim_node.min;
        const float3 prim_bmax = prim_node.max;

        float prim_dmin, prim_dmax;
        if (CheckBox(prim_bmin, prim_dmin, prim_bmax, prim_dmax, ray.org, ray.tmin, ray.dir, dist))
        {
          prim_stride += 1;
          continue;
        }
        prim_stride += prim_node.count;
      }
      inst_stride += 1;
      continue;
    }
    const float3 inst_bmin = inst_node.min;
    const float3 inst_bmax = inst_node.max;

    float inst_dmin, inst_dmax;
    if (CheckBox(inst_bmin, inst_dmin, inst_bmax, inst_dmax, ray.org, ray.tmin, ray.dir, dist))
    {
      inst_stride += 1;
      continue;
    }
    inst_stride += inst_node.count;
  }
  return hit;
}



bool OccludeScene(in Ray ray)
{
  //StructuredBuffer<Box> mesh_nodes = mesh_boxes;

  const uint inst_offset = 0;
  const uint inst_count = inst_boxes[inst_offset].count;

  uint inst_stride = 0;
  while (inst_stride < inst_count)
  {
    const Box inst_node = inst_boxes[inst_stride + inst_offset];
    if (inst_node.count == 1) //Instance leaf
    {
      const int inst_idx = inst_node.offset;
      const Instance instance = inst_items[inst_idx];

      const uint prim_offset = 2 * instance.prim_offset - inst_idx;
      const uint prim_count = prim_boxes[prim_offset].count;

      uint prim_stride = 0;
      while (prim_stride < prim_count)
      {
        const Box prim_node = prim_boxes[prim_stride + prim_offset];
        if (prim_node.count == 1) //Primitive leaf
        {
          const int prim_idx = prim_node.offset;

#ifdef USE_CONSISTENT_BVH
          const float3 center = f16tof32(asuint(prim_node.min));
          const float3 delta0 = f16tof32(asuint(prim_node.min) >> 16);
          const float3 delta1 = f16tof32(asuint(prim_node.max));
          const float3 delta2 = f16tof32(asuint(prim_node.max) >> 16);

          const float3 pos0 = center + delta0;
          const float3 pos1 = center + delta1;
          const float3 pos2 = center + delta2;
#else
          const Primitive primitive = prim_items[instance.prim_offset + prim_idx];
          const float3 pos0 = vert_items[instance.vert_offset + primitive.idx0].pos;
          const float3 pos1 = vert_items[instance.vert_offset + primitive.idx1].pos;
          const float3 pos2 = vert_items[instance.vert_offset + primitive.idx2].pos;
#endif

          float t, u, v;
          if (CheckTriangle(pos0, pos1, pos2, ray.org, ray.tmin, ray.dir, ray.tmax, t, u, v))
          {
            if (ray.tmax > t)
            {
              return true;
            }
          }
          prim_stride += 1;
          continue;
        }
        const float3 prim_bmin = prim_node.min;
        const float3 prim_bmax = prim_node.max;

        float prim_dmin, prim_dmax;
        if (CheckBox(prim_bmin, prim_dmin, prim_bmax, prim_dmax, ray.org, ray.tmin, ray.dir, ray.tmax))
        {
          prim_stride += 1;
          continue;
        }
        prim_stride += prim_node.count;
      }
      inst_stride += 1;
      continue;
    }
    const float3 inst_bmin = inst_node.min;
    const float3 inst_bmax = inst_node.max;

    float inst_dmin, inst_dmax;
    if (CheckBox(inst_bmin, inst_dmin, inst_bmax, inst_dmax, ray.org, ray.tmin, ray.dir, ray.tmax))
    {
      inst_stride += 1;
      continue;
    }
    inst_stride += inst_node.count;
  }
  return false;
}

float nrand(float2 uv)
{
  return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

inline uint MurmurHash3_mix(in uint hash, in uint k)
{
  k *= 0xcc9e2d51;
  k = (k << 15) | (k >> (32 - 15));
  k *= 0x1b873593;

  hash ^= k;
  hash = ((hash << 13) | (hash >> (32 - 13))) * 5 + 0xe6546b64;

  return hash;
}

inline uint MurmurHash3_finalize(uint hash)
{
  hash ^= hash >> 16;
  hash *= 0x85ebca6b;
  hash ^= hash >> 13;
  hash *= 0xc2b2ae35;
  hash ^= hash >> 16;

  return hash;
}

inline uint LCG_next(uint value)
{
  return value * 1664525 + 1013904223;
}

inline uint RandomSampler_init(int pixelId, int sampleId)
{
  uint hash = 0;
  hash = MurmurHash3_mix(hash, pixelId);
  hash = MurmurHash3_mix(hash, sampleId);
  hash = MurmurHash3_finalize(hash);

  return hash;
}

//inline uint MurmurHash3_initialize(in uint index, in uint scramble)
//{
//  uint hash = 0;
//  hash = MurmurHash3_mix(hash, index);
//  hash = MurmurHash3_mix(hash, scramble);
//  hash ^= hash >> 16;
//  hash *= 0x85ebca6b;
//  hash ^= hash >> 13;
//  hash *= 0xc2b2ae35;
//  hash ^= hash >> 16;
//
//  return hash;
//}

inline float RandomSampler_sample(inout uint state)
{
  state = LCG_next(state); return state * 2.3283064365386963e-10;
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

//Hit FetchHit(in uint ix, in uint iy)
//{
//  Hit hit;

//  const uint4 value = lightmap_input[uint2(ix, iy)];
//  hit.geom = value.xy;
//  hit.bary = asfloat(value.zw);

//  return hit;
//}

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
  
  Ray ray;
  ray.org = pos;
  ray.tmin = 0.0001;
  ray.dir = mul(transpose(tbn), normalize(wi));
  ray.tmax = 0.25;
  
  const float3 tput = nrm; //OccludeScene(ray) ? float3(0.0, 0.0, 0.0) : float3(1.0, 1.0, 1.0);
  
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
