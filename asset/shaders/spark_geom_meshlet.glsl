#version 460
#extension GL_EXT_mesh_shader : require
#extension GL_EXT_shader_explicit_arithmetic_types_int8  : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

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
  uint col;
  vec3 nrm;
  uint msk;
  vec3 tgn;
  float sign;
  vec2 tc0;
  vec2 tc1;
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

    vec3 aabb_min;
    uint index;
    vec3 aabb_max;
    uint flags;

    vec3 bs_center;
    float bs_raduis;

    vec4 fparam_0;
    vec4 fparam_1;
    vec4 fparam_2;
    vec4 fparam_3;

    uvec4 uparam_0;
    uvec4 uparam_1;
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

//layout(location=0) out Payload
//{
//  flat uint vidx;
//  vec3 wpos;
//} payload[];

#define VERT_LIMIT 64
#define TRNG_LIMIT 128
#define GROUP_SIZE 32

const vec4[3] positions = {
  vec4( 00.0, -10.0, 10.0, 1.0),
  vec4(-10.0,  10.0, 10.0, 1.0),
  vec4( 10.0,  10.0, 10.0, 1.0)
};
//
//const vec4[3] colors = {
//  vec4(0.0, 1.0, 0.0, 1.0),
//  vec4(0.0, 0.0, 1.0, 1.0),
//  vec4(1.0, 0.0, 0.0, 1.0)
//};


const uint VERT_ITERATIONS = (VERT_LIMIT + GROUP_SIZE - 1) / GROUP_SIZE;
const uint TRNG_ITERATIONS = (TRNG_LIMIT + GROUP_SIZE - 1) / GROUP_SIZE;

layout(local_size_x = GROUP_SIZE, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = VERT_LIMIT, max_primitives = TRNG_LIMIT) out;

//layout(location = 0) out Payload
//{
//  vec4 color;
//} payload[];

//layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
//layout(triangles, max_vertices = 3, max_primitives = 1) out;
//
//void main()
//{
//  uint iid = gl_LocalInvocationID.x;
//
//  vec4 offset = vec4(0.0, 0.0, gl_GlobalInvocationID.x, 0.0);
//
//  SetMeshOutputsEXT(3, 1);
//  //mat4 mvp = ubo.projection * ubo.view * ubo.model;
//  gl_MeshVerticesEXT[0].gl_Position = (positions[0] + offset);
//  gl_MeshVerticesEXT[1].gl_Position = (positions[1] + offset);
//  gl_MeshVerticesEXT[2].gl_Position = (positions[2] + offset);
//  //payload[0].color = colors[0];
//  //payload[1].color = colors[1];
//  //payload[2].color = colors[2];
//  gl_PrimitiveTriangleIndicesEXT[gl_LocalInvocationIndex] =  uvec3(0, 1, 2);
//}

void main()
{
  uint groupID = gl_WorkGroupID.x;
  uint localID = gl_LocalInvocationID.x;

  Meshlet meshlet = meshlets[instance.mlet_offset + groupID];
  SetMeshOutputsEXT(meshlet.vidx_count, meshlet.tidx_count);

  mat4 mvp = camera.proj * camera.view;
  
  //gl_MeshVerticesEXT[0].gl_Position = (mvp * positions[0]);
  //gl_MeshVerticesEXT[1].gl_Position = (mvp * positions[1]);
  //gl_MeshVerticesEXT[2].gl_Position = (mvp * positions[2]);

  for (uint i = 0; i < VERT_ITERATIONS; ++i)
  {
    uint vidx = localID + i * GROUP_SIZE;
    if(vidx < meshlet.vidx_count)
    { 
      uint idx = v_indices[vidx + meshlet.vidx_offset + instance.vidx_offset];

      vec3 opos = vertices[idx + instance.vert_offset].pos;
      vec3 wpos = opos; //(instance.transform  * vec4(opos, 1.0)).xyz;

      //payload[vidx].vidx = vidx;
      //payload[vidx].wpos = wpos;

      vec4 hpos = mvp * vec4(wpos, 1.0);
      gl_MeshVerticesEXT[vidx].gl_Position = hpos;
    }
  }

  //gl_PrimitiveTriangleIndicesEXT[gl_LocalInvocationIndex] =  uvec3(0, 1, 2);


  for (uint i = 0; i < TRNG_ITERATIONS; ++i)
  {
    uint tidx = localID + i * GROUP_SIZE;
    if(tidx < meshlet.tidx_count)
    {
      uint idx0 = t_indices[tidx * 3 + 0 + meshlet.tidx_offset + instance.tidx_offset];
      uint idx1 = t_indices[tidx * 3 + 1 + meshlet.tidx_offset + instance.tidx_offset];
      uint idx2 = t_indices[tidx * 3 + 2 + meshlet.tidx_offset + instance.tidx_offset];
      
      gl_PrimitiveTriangleIndicesEXT[tidx] = uvec3(idx2, idx1, idx0);
    }
  }
}

#endif

#ifdef FRAG

//layout(location=0) in Payload
//{
//  flat uint vidx;
//  vec3 wpos;
//} payload;

//layout(location = 0) in Payload
//{
//  vec4 color;
//} payload;

layout(location = 0) out vec4 target_0;
layout(location = 1) out vec4 target_1;
layout(location = 2) out vec4 target_2;

void main()
{
  target_0 = vec4(0.0, 0.5, 0.0, 0.0); //payload.wpos;
  target_1 = vec4(0.0, 0.0, 0.0, 0.0);
  target_2 = vec4(0.0, 0.0, 0.0, 0.0);
}

#endif
