#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_samplerless_texture_functions : require

//#extension GL_EXT_nonuniform_qualifier : enable
//#extension GL_XT_scalar_block_layout : enable
//#extension GL_GOOGLE_include_directive : enable

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

struct Primitive
{
  uint idx0;
  uint idx1;
  uint idx2;
};

struct Instance
{
  mat3x4 transform;

  vec3 emission;
  float intensity;
  vec3 diffuse;
  float metallic;
  vec3 specular;
  float roughness;
  vec3 transmit;
  float ior;

  int tex0_idx;
  int tex1_idx;
  int tex2_idx;
  int tex3_idx;

  vec3 debug_color;
  uint geometry_idx;

  uint prim_offset;
  uint prim_count;
  uint vert_offset;
  uint vert_count;

  uvec4 padding[6];
};


const float RAY_TMIN = 0.001;
const float RAY_TMAX = 100.0;


struct Attribute
{
  vec2 bary;
};


layout(set = 0, binding = 0) uniform sampler sampler0;

layout(std140, set = 0, binding = 1) uniform Screen
{
  uint extent_x;
  uint extent_y;
  uint rnd_base;
  uint rnd_seed;
} screen;

layout(std140, set = 0, binding = 2) uniform Camera
{
  mat4 camera_view;
  mat4 camera_proj;
  mat4 camera_view_inv;
  mat4 camera_proj_inv;
} camera;

layout(std140, set = 0, binding = 3) uniform Shadow
{
  mat4 shadow_view;
  mat4 shadow_proj;
  mat4 shadow_view_inv;
  mat4 shadow_proj_inv;
} shadow;

layout(set = 0, binding = 4) uniform texture2D gbuffer_0_texture;
layout(set = 0, binding = 5) uniform texture2D gbuffer_1_texture;
layout(set = 0, binding = 6) uniform texture2D depth_texture;
//layout(set = 0, binding = 18) buffer AccumulationBuffer
//{
//  vec4 accumulation_buffer[];
//};

layout(set = 0, binding = 7, rgba32f) uniform image2D color_texture;
layout(set = 0, binding = 8) uniform accelerationStructureEXT tlas;


#ifdef RGEN

layout(location = 0) rayPayloadEXT bool occluded;


bool OccludeScene(in Ray ray)
{
	occluded = true;
    traceRayEXT(tlas, gl_RayFlagsOpaqueEXT | gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xff, 0, 0, 0, 
        ray.org, ray.tmin, ray.dir, ray.tmax, 0);
    return occluded;
}


void main()
{
  const uint ix = gl_LaunchIDEXT.x;
  const uint iy = gl_LaunchIDEXT.y;

  //sampler2DArray(texture_items, sampler0)

  const float depth = texelFetch(depth_texture, ivec2(ix, iy), 0).x;

  if (depth == 1.0)
  {
    return;
  }

  const vec4 albedo_metallic = texelFetch(gbuffer_0_texture, ivec2(ix, iy), 0); //gbuffer_0_texture.Load(int3(input.pos.xy, 0));
  const vec4 normal_smoothness = texelFetch(gbuffer_1_texture, ivec2(ix, iy), 0); //gbuffer_1_texture.Load(int3(input.pos.xy, 0));

  const float metallic = albedo_metallic.a;
  const float smoothness = normal_smoothness.a;

//#ifdef USE_NORMAL_OCT_QUAD_ENCODING
//  const float3 normal = UnpackNormal(uint3(normal_smoothness.rgb * 255.0));
//#else
  const vec3 normal = 2.0 * normal_smoothness.rgb - 1.0;
//#endif

  const float rx = 2.0 * (ix + 0.5) / screen.extent_x - 1.0;
  const float ry = 2.0 * (iy + 0.5) / screen.extent_y - 1.0;

  const vec4 ndc_coord = vec4(rx, -ry, depth, 1.0);
  const vec4 view_pos = camera.camera_proj_inv * ndc_coord;
  const vec3 surface_pos = (camera.camera_view_inv * view_pos / view_pos.w).xyz;

  const vec3 camera_pos = vec3(camera.camera_view_inv[3][0], camera.camera_view_inv[3][1], camera.camera_view_inv[3][2]);
  const float camera_dst = length(camera_pos - surface_pos);
  const vec3 camera_dir = (camera_pos - surface_pos) / camera_dst;

  const vec3 shadow_pos = vec3(shadow.shadow_view_inv[3][0], shadow.shadow_view_inv[3][1], shadow.shadow_view_inv[3][2]);
  const float shadow_dst = length(shadow_pos - surface_pos);
  const vec3 shadow_dir = (shadow_pos - surface_pos) / shadow_dst;

  Ray ray;
  ray.org = surface_pos;
  ray.tmin = RAY_TMIN;
  ray.dir = shadow_dir;
  ray.tmax = shadow_dst;

  const vec3 diffuse = max(0.0, dot(shadow_dir, normal)) * vec3(1, 1, 1) * albedo_metallic.xyz;
  const float specular = pow(max(0.0, dot(normalize(camera_dir + shadow_dir), normal)), smoothness);
  const float attenuation = OccludeScene(ray) ? 0.0 : 1.0;

  const vec3 color = diffuse * attenuation;

  const vec4 result = vec4(color, 0.0);
  imageStore(color_texture, ivec2(ix, iy), imageLoad(color_texture, ivec2(ix, iy)) + result); 
}

#endif



#ifdef MISS
layout(location = 0) rayPayloadInEXT bool occluded;

void main()
{
  occluded = false;
}
#endif