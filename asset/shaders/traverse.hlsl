struct Box
{
  float3 min; 
  uint offset;
  float3 max; 
  uint count; 
};

struct Vertex
{
  float3 pos;
  uint col;
  float3 nrm;
  float mask;
  float3 tgn;
  float sgn;
  float2 tc0;
  float2 tc1;
};

struct Primitive
{
  uint idx0;
  uint idx1;
  uint idx2;
};

struct Instance
{
  float4x3 transform;
  
  uint aaam_layer;
  uint snno_layer;
  uint eeet_layer;
  uint mask_layer;

  uint vert_offset;
  uint vert_count; 
  uint prim_offset;
  uint prim_count; 
  uint mlet_offset;
  uint mlet_count; 
  uint bone_offset;
  uint bone_count;
  
  float3 bb_min;
  uint index;
  float3 bb_max;
  uint flags;

  float4 fparam_0;
  float4 fparam_1;
  float4 fparam_2;
  float4 fparam_3;

  uint4 uparam_0;
  uint4 uparam_1;
  uint4 uparam_2;
  uint4 uparam_3;
};

struct Ray
{
  float3 org;
  float tmin;
  float3 dir;
  float tmax;
};

struct Hit
{
  uint2 geom;
  float2 bary;
};

struct RayHit
{
  float3 org;
  float tmin;
  float3 dir;
  float tmax;
  float3 pg;
  float dist;
  uint2 geom;
  float2 bary;
};

static const float RAY_TMIN = 0.001;
static const float RAY_TMAX = 100.0;

bool CheckBox(in float3 pmin, out float dmin, in float3 pmax, out float dmax,
  in float3 org, in float tmin, in float3 dir, in float tmax)
{
  const float3 f = (pmax - org) / dir;
  const float3 n = (pmin - org) / dir;

  const float3 bmax = max(f, n);
  const float3 bmin = min(f, n);

  dmax = min(min(bmax.x, min(bmax.y, bmax.z)), tmax);
  dmin = max(max(bmin.x, max(bmin.y, bmin.z)), tmin);

  return dmax >= dmin;
}

bool CheckTriangle(in float3 p0, in float3 p1, in float3 p2,
  in float3 org, in float tmin, const in float3 dir, in float tmax, out float t, out float u, out float v)
{
  const float3 e1 = p1 - p0;
  const float3 e2 = p2 - p0;
  const float3 s1 = cross(dir, e2);
  const float  invd = 1.0 / (dot(s1, e1));
  const float3 d = org - p0;
  u = dot(d, s1) * invd;
  const float3 s2 = cross(d, e1);
  v = dot(dir, s2) * invd;
  t = dot(e2, s2) * invd;

  if (u < 0.0 || u > 1.0 || v < 0.0 || u + v > 1.0 || t < tmin || t > tmax)
  {
    return false;
  }

  return true;
}



void IntersectScene(inout RayHit rayhit,
  in StructuredBuffer<Box> inst_boxes,
  in StructuredBuffer<Box> prim_boxes,
  in StructuredBuffer<Instance> inst_items,
  in StructuredBuffer<Primitive> prim_items,
  in StructuredBuffer<Vertex> vert_items)
{
  rayhit.dist = rayhit.tmax;

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
          if (CheckTriangle(pos0, pos1, pos2, rayhit.org, rayhit.tmin, rayhit.dir, rayhit.dist, t, u, v))
          {
            if (rayhit.dist >= t)
            {
              bool alpha_clip = false;
#ifdef USE_ALPHA_CLIP
              if (instance.tex1_idx != -1)
              {
                const Vertex vertex0 = vert_items[instance.vert_offset + primitive.idx0];
                const Vertex vertex1 = vert_items[instance.vert_offset + primitive.idx1];
                const Vertex vertex2 = vert_items[instance.vert_offset + primitive.idx2];

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
                rayhit.bary = float2(u, v);
                rayhit.geom = uint2(inst_idx, prim_idx);
                rayhit.dist = t;
                rayhit.pg = mul(float3(1.0 - u - v, u, v), float3x3(pos0, pos1, pos2)); 
              }
            }
          }
          prim_stride += 1;
          continue;
        }
        const float3 prim_bmin = prim_node.min;
        const float3 prim_bmax = prim_node.max;

        float prim_dmin, prim_dmax;
        if (CheckBox(prim_bmin, prim_dmin, prim_bmax, prim_dmax, rayhit.org, rayhit.tmin, rayhit.dir, rayhit.dist))
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
    if (CheckBox(inst_bmin, inst_dmin, inst_bmax, inst_dmax, rayhit.org, rayhit.tmin, rayhit.dir, rayhit.dist))
    {
      inst_stride += 1;
      continue;
    }
    inst_stride += inst_node.count;
  }
}



void OccludeScene(inout RayHit rayhit,
  in StructuredBuffer<Box> inst_boxes,
  in StructuredBuffer<Box> prim_boxes,
  in StructuredBuffer<Instance> inst_items,
  in StructuredBuffer<Primitive> prim_items,
  in StructuredBuffer<Vertex> vert_items)
{ 
  rayhit.dist = rayhit.tmax;

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
          if (CheckTriangle(pos0, pos1, pos2, rayhit.org, rayhit.tmin, rayhit.dir, rayhit.tmax, t, u, v))
          {
            if (rayhit.dist > t)
            {
              rayhit.dist = t;
              return;
            }
          }
          prim_stride += 1;
          continue;
        }
        const float3 prim_bmin = prim_node.min;
        const float3 prim_bmax = prim_node.max;

        float prim_dmin, prim_dmax;
        if (CheckBox(prim_bmin, prim_dmin, prim_bmax, prim_dmax, rayhit.org, rayhit.tmin, rayhit.dir, rayhit.tmax))
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
    if (CheckBox(inst_bmin, inst_dmin, inst_bmax, inst_dmax, rayhit.org, rayhit.tmin, rayhit.dir, rayhit.tmax))
    {
      inst_stride += 1;
      continue;
    }
    inst_stride += inst_node.count;
  }
  return;
}