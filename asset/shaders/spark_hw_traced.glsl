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

  uint prim_offset;
  uint prim_count;
  uint vert_offset;
  uint vert_count;

  vec4 brdf_param0;
  vec4 brdf_param1;
  vec4 brdf_param2;
  vec4 brdf_param3;

  int tex0_idx;
  int tex1_idx;
  int tex2_idx;
  int tex3_idx;
  int tex4_idx;
  int tex5_idx;
  int tex6_idx;
  int tex7_idx;

  vec3 bb_min;
  uint geom_idx;
  vec3 bb_max;
  uint brdf_idx;

  uvec4 padding[4];
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

struct BRDF_CookTorrance
{
  vec3 color;
  float roughness;
  float metallic;
};

#define kDielectricSpec vec4(0.04, 0.04, 0.04, 1.0 - 0.04)

float OneMinusReflectivityMetallic(float metallic)
{
    float oneMinusDielectricSpec = kDielectricSpec.a;
    return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
}

vec3 Evaluate_CookTorrance(BRDF_CookTorrance brdf, vec3 lo, vec3 wo)
{
  const float roughness2 = brdf.roughness * brdf.roughness; // , 6.103515625e-5);
  const float normalizationTerm = brdf.roughness * 4.0 + 2.0;
  const float roughness2MinusOne = roughness2 - 1.0;

  const vec3 H = normalize(wo + lo);

  const float NoH = H.z;
  const float LoH = max(0.0, dot(lo, H));

  // GGX Distribution multiplied by combined approximation of Visibility and Fresnel
  // BRDFspec = (D * V * F) / 4.0
  // D = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2
  // V * F = 1.0 / ( LoH^2 * (roughness + 0.5) )
  // See "Optimizing PBR for Mobile" from Siggraph 2015 moving mobile graphics course
  // https://community.arm.com/events/1155

  // Final BRDFspec = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2 * (LoH^2 * (roughness + 0.5) * 4.0)
  // We further optimize a few light invariant terms
  // brdfData.normalizationTerm = (roughness + 0.5) * 4.0 rewritten as roughness * 4.0 + 2.0 to a fit a MAD.
  const float d = NoH * NoH * roughness2MinusOne + 1.0; // 0001f;

  const float LoH2 = LoH * LoH;
  const float res = roughness2 / ((d * d) * max(0.0, LoH2) * normalizationTerm);

  return vec3(res, res, res); // *brdf.color;
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
//  const vec3 normal = UnpackNormal(uint3(normal_smoothness.rgb * 255.0));
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
  const vec3 shadow_dir =-vec3(shadow.shadow_view_inv[2][0], shadow.shadow_view_inv[2][1], shadow.shadow_view_inv[2][2]);

  Ray ray;
  ray.org = surface_pos;
  ray.tmin = RAY_TMIN;
  ray.dir = shadow_dir;
  ray.tmax = shadow_dst;

  const vec3 diffuse = max(0.0, dot(shadow_dir, normal)) * vec3(1, 1, 1) * albedo_metallic.xyz;
  const float specular = pow(max(0.0, dot(normalize(camera_dir + shadow_dir), normal)), smoothness);
  const float attenuation = OccludeScene(ray) ? 0.0 : 1.0;

  BRDF_CookTorrance brdf; // = Initialize_CookTorrance();
  brdf.color = albedo_metallic.xyz;
  brdf.roughness = clamp(normal_smoothness.w, 0.001, 0.999);
  brdf.metallic = albedo_metallic.w;

  const vec3 n = normalize(normal.xyz);
  const vec3 t = normalize(n.y * n.y > n.x * n.x ? vec3(0.0, -n.z, n.y) : vec3(-n.z, 0.0, n.x));
  const vec3 b = normalize(cross(n, t));
  const mat3x3 tbn = mat3x3(t, b, n);

  const vec3 wo = shadow_dir * tbn;
  const vec3 lo = camera_dir * tbn;

  const vec3 m = Evaluate_CookTorrance(brdf, lo, wo) * brdf.color;

  const float oneMinusReflectivity = OneMinusReflectivityMetallic(brdf.metallic);
  const float reflectivity = 1.0 - oneMinusReflectivity;

  const vec3 diff = brdf.color * oneMinusReflectivity;
  const vec3 spec = mix(kDielectricSpec.rgb, m, metallic);

  const vec3 color = (diff + spec) * max(0.0, wo.z) * attenuation;

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
