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
  float color;
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

  uint prim_offset;
  uint prim_count;
  uint vert_offset;
  uint vert_count;

  float3 emission;
  float intensity;
  float3 diffuse;
  float shininess;
  float3 specular;
  float alpha;

  int tex0_idx;
  int tex1_idx;
  int tex2_idx;
  int tex3_idx;

  float3 debug_color;
  uint geometry_idx;

  uint4 padding[7];
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
  in float3 org, in float tmin, const in float3 dir, in float tmax, 
  out float t, out float u, out float v)
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