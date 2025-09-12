#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

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

  float3 emission;
  float intensity;
  float3 diffuse;
  float metallic;
  float3 specular;
  float roughness;

  int tex0_idx;
  int tex1_idx;
  int tex2_idx;
  int tex3_idx;
  int tex4_idx;
  int tex5_idx;
  int tex6_idx;
  int tex7_idx;

  float3 debug_color;
  uint geometry_idx;

  uint prim_offset;
  uint prim_count;
  uint vert_offset;
  uint vert_count;
  
  float3 bb_min;
  uint bb_min_padding;
  float3 bb_max;
  uint bb_max_padding;

  uint4 padding[4];
};