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

  uint aaam_layer;
  uint snno_layer;
  uint eeet_layer;
  uint mask_layer;

  uint vert_offset;
  uint vert_count;
  uint trng_offset;
  uint trng_count;
  uint mlet_offset;
  uint mlet_count;
  uint bbox_offset;
  uint bbox_count;
  uint vidx_offset;
  uint vidx_count;
  uint tidx_offset;
  uint tidx_count;

  float3 aabb_min;
  uint index;
  float3 aabb_max;
  uint flags;

  float3 bs_center;
  float bs_raduis;

  float4 fparam_0;
  float4 fparam_1;
  float4 fparam_2;
  float4 fparam_3;

  uint4 uparam_0;
  uint4 uparam_1;
};