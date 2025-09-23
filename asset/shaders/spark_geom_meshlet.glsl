#version 460
#extension GL_EXT_mesh_shader : require
#extension GL_EXT_shader_explicit_arithmetic_types_int8  : require

struct Box
{
  vec3 min;
  uint offset;
  vec3 max;
  uint count;
};

struct Ray
{
  vec3 org;
  float tmin;
  vec3 dir;
  float tmax;
};

struct Hit
{
  uvec2 geom;
  vec2 bary;
};

struct Surface
{
  vec3 Ke;
  float p;
  vec3 Kd;
  float m;
  vec3 Ks;
  float r;
  vec3 Kt;
  float i;
};

struct Vertex
{
  vec3 pos;
  float u;
  vec3 nrm;
  float v;
  vec3 tgn;
  float sign;
};

struct Triangle
{
  uint idx0;
  uint idx1;
  uint idx2;
};

struct Meshlet
{
  uint vidx_offset;
  uint vidx_count;
  uint tidx_offset;
  uint tidx_count;
};

struct Frustum
{
  mat4x4 view;
  mat4x4 proj;
  mat4x4 view_inv;
  mat4x4 proj_inv;
};

struct Screen
{
  uint extent_x;
  uint extent_y;
  uint rnd_base;
  uint rnd_seed;
};

struct Instance
{
    mat3x4 transform;

    uint aaam_layer; // AM
    uint snno_layer; // SNAO
    uint eeet_layer; // ET
    uint mask_layer;

    uint vert_offset; // vert_offset
    uint vert_count;  // vert_count
    uint trng_offset; // prim_offset
    uint trng_count;  // prim_count
    uint mlet_offset; // mlet_offset
    uint mlet_count;  // mlet_count
    uint bone_offset;
    uint bone_count;

    vec3 aabb_min;
    uint index;
    vec3 aabb_max;
    uint flags;

    vec4 fparam_0;
    vec4 fparam_1;
    vec4 fparam_2;
    vec4 fparam_3;

    uvec4 uparam_0;
    uvec4 uparam_1;
    uvec4 uparam_2;
    uvec4 uparam_3;
};


const float RAY_TMIN = 0.001;
const float RAY_TMAX = 100.0;


struct Attribute
{
  vec2 bary;
};


layout(set = 0, binding = 0) uniform sampler sampler0;
layout(set = 0, binding = 1) uniform sampler sampler1;

layout(std140, set = 0, binding = 2) uniform screen_data {Screen screen;};
layout(std140, set = 0, binding = 3) uniform camera_data {Frustum camera;};
layout(std140, set = 0, binding = 4) uniform shadow_data {Frustum shadow;};
layout(std140, set = 0, binding = 5) uniform instance_data {Instance instance;};

layout(std430, set = 0, binding = 6) buffer readonly meshlet_buffer {Meshlet meshlets[];};
layout(std430, set = 0, binding = 7) buffer readonly v_index_buffer {uint v_indices[];};
layout(std430, set = 0, binding = 8) buffer readonly vertex_buffer {Vertex vertices[];};
layout(std430, set = 0, binding = 9) buffer readonly t_index_buffer {uint8_t t_indices[];};

layout(set = 0, binding = 10) uniform texture2DArray aaam_array;
layout(set = 0, binding = 11) uniform texture2DArray snno_array;
layout(set = 0, binding = 12) uniform texture2DArray eeet_array;




#ifdef MESH

layout(location=0) out Payload
{
  flat uint vidx;
  vec3 wpos;
} payload[];

#define VERT_LIMIT 64
#define TRNG_LIMIT 128
#define GROUP_SIZE 32


const uint VERT_ITERATIONS = (VERT_LIMIT + GROUP_SIZE - 1) / GROUP_SIZE;
const uint TRNG_ITERATIONS = (TRNG_LIMIT + GROUP_SIZE - 1) / GROUP_SIZE;

layout(local_size_x = GROUP_SIZE, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = VERT_LIMIT, max_primitives = TRNG_LIMIT) out;

void main()
{
  uint groupID = gl_WorkGroupID.x;
  uint localID = gl_LocalInvocationID.x;

  Meshlet meshlet = meshlets[instance.mlet_offset + groupID];
  SetMeshOutputsEXT(meshlet.vidx_count, meshlet.tidx_count);

  for (uint i = 0; i < VERT_ITERATIONS; ++i)
  {
    uint vidx = localID + i * GROUP_SIZE;
    if(vidx < meshlet.vidx_count)
    { 
      vec3 opos = vertices[v_indices[vidx + meshlet.vidx_offset] + instance.vert_offset].pos;
      vec3 wpos = opos; //(instance.transform  * vec4(opos, 1.0)).xyz;

      payload[vidx].vidx = vidx;
      payload[vidx].wpos = wpos;

      vec4 hpos = (camera.view * vec4(wpos, 1.0));
      gl_MeshVerticesEXT[vidx].gl_Position = hpos;
    }
  }


  for (uint i = 0; i < TRNG_ITERATIONS; ++i)
  {
    uint tidx = localID + i * GROUP_SIZE;
    if(tidx < meshlet.tidx_count)
    {
      uint idx0 = t_indices[tidx * 3 + 0 + meshlet.tidx_offset];
      uint idx1 = t_indices[tidx * 3 + 1 + meshlet.tidx_offset];
      uint idx2 = t_indices[tidx * 3 + 2 + meshlet.tidx_offset];
      
      gl_PrimitiveTriangleIndicesEXT[tidx] = uvec3(idx0, idx1, idx2);
    }
  }
}

#endif

#ifdef FRAG

layout(location=0) in Payload
{
  flat uint vidx;
  vec3 wpos;
} payload;

layout(location = 0) out vec4 target_0;
layout(location = 1) out vec4 target_1;
layout(location = 2) out vec4 target_2;

void main()
{
  target_0 = payload.wpos;
  target_1 =  vec4(0.0, 0.0, 0.0, 0.0);
  target_2 =  vec4(0.0, 0.0, 0.0, 0.0);
}

#endif
