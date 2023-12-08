#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

#include "packing.hlsl"
#include "traverse.hlsl"

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

Hit IntersectScene(in Ray ray, out float dist)
{
  Hit hit;
  hit.geom = uint2(-1, -1);
  hit.bary = float2(0.0, 0.0);


  //StructuredBuffer<Box> mesh_nodes = mesh_boxes;
  dist = ray.tmax;

  const uint inst_offset = 0;
  const uint inst_count = t_boxes[inst_offset].count;

  uint inst_stride = 0;
  while (inst_stride < inst_count)
  {
    const Box inst_node = t_boxes[inst_stride + inst_offset];
    if (inst_node.count == 1) //Instance leaf
    {
      const int inst_idx = inst_node.offset;
      const Instance instance = inst_items[inst_idx];

      const uint prim_offset = 2 * instance.prim_offset - inst_idx;
      const uint prim_count = b_boxes[prim_offset].count;

      uint prim_stride = 0;
      while (prim_stride < prim_count)
      {
        const Box prim_node = b_boxes[prim_stride + prim_offset];
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
  const uint inst_count = t_boxes[inst_offset].count;

  uint inst_stride = 0;
  while (inst_stride < inst_count)
  {
    const Box inst_node = t_boxes[inst_stride + inst_offset];
    if (inst_node.count == 1) //Instance leaf
    {
      const int inst_idx = inst_node.offset;
      const Instance instance = inst_items[inst_idx];

      const uint prim_offset = 2 * instance.prim_offset - inst_idx;
      const uint prim_count = b_boxes[prim_offset].count;

      uint prim_stride = 0;
      while (prim_stride < prim_count)
      {
        const Box prim_node = b_boxes[prim_stride + prim_offset];
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

//#ifdef USE_NORMAL_OCT_QUAD_ENCODING
//  const float3 normal = UnpackNormal(uint3(normal_smoothness.rgb * 255.0));
//#else
//#endif
  const float rx = 2.0 * input.pos.x / extent_x - 1.0;
  const float ry = 2.0 * input.pos.y / extent_y - 1.0;

  const float4 ndc_coord = float4(rx, -ry, depth, 1.0);
  const float4 view_pos = mul(camera_proj_inv, ndc_coord);
  const float3 surface_pos = mul(camera_view_inv, view_pos / view_pos.w).xyz;

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - surface_pos);
  const float3 camera_dir = (camera_pos - surface_pos) / camera_dst;

  const float3 shadow_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float shadow_dst = length(shadow_pos - surface_pos);
  const float3 shadow_dir = (shadow_pos - surface_pos) / shadow_dst;

  Ray ray;
  ray.org = surface_pos;
  ray.tmin = RAY_TMIN;
  ray.dir = shadow_dir;
  ray.tmax = shadow_dst;

  const float3 diffuse = max(0.0, dot(shadow_dir, normal)) * albedo_metallic.xyz;
  const float3 specular = pow(max(0.0, dot(normalize(camera_dir + shadow_dir), normal)), smoothness);
  const float attenuation = OccludeScene(ray) ? 0.0 : 1.0;

  const float3 color = diffuse * attenuation;
  output.target_0 = float4(color, 1.0);
  
  return output;
}