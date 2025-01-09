#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

//#define USE_NORMAL_MAP
//#define USE_ALPHA_CLIP
//#define USE_SPECULAR_SETUP

#include "common.hlsl"
#include "surface.hlsl"
#include "brdf.hlsl"
#include "packing.hlsl"


VK_BINDING(0) sampler sampler0 : register(s0);
VK_BINDING(1) sampler sampler1 : register(s1);

VK_BINDING(2) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}

VK_BINDING(3) cbuffer constant1 : register(b1)
{
  float4x4 camera_view     : packoffset(c0.x);
  float4x4 camera_proj     : packoffset(c4.x);
  float4x4 camera_view_inv : packoffset(c8.x);
  float4x4 camera_proj_inv : packoffset(c12.x);
}

VK_BINDING(4) cbuffer constant2 : register(b2)
{
  float4x4 shadow_view     : packoffset(c0.x);
  float4x4 shadow_proj     : packoffset(c4.x);
  float4x4 shadow_view_inv : packoffset(c8.x);
  float4x4 shadow_proj_inv : packoffset(c12.x);
}

VK_BINDING(5) cbuffer constant3 : register(b3)
{
  float4x3 transform  : packoffset(c0.x);

  uint prim_offset    : packoffset(c3.x);
  uint prim_count     : packoffset(c3.y);
  uint vert_offset    : packoffset(c3.z);
  uint vert_count     : packoffset(c3.w);

  float4 brdf_param0  : packoffset(c4.x);
  float4 brdf_param1  : packoffset(c5.x);
  float4 brdf_param2  : packoffset(c6.x);
  float4 brdf_param3  : packoffset(c7.x);

  int tex0_idx : packoffset(c8.x);
  int tex1_idx : packoffset(c8.y);
  int tex2_idx : packoffset(c8.z);
  int tex3_idx : packoffset(c8.w);
  int tex4_idx : packoffset(c9.x);
  int tex5_idx : packoffset(c9.y);
  int tex6_idx : packoffset(c9.z);
  int tex7_idx : packoffset(c9.w);
  
  float3 bb_min : packoffset(c10.x);
  uint geom_idx : packoffset(c10.w);
  float3 bb_max : packoffset(c11.x);
  uint brdf_idx : packoffset(c11.w);

  uint4 padding[4]    : packoffset(c12.x);
};

VK_BINDING(6) Texture2DArray<float4> texture0_items : register(t0);
VK_BINDING(7) Texture2DArray<float4> texture1_items : register(t1);
VK_BINDING(8) Texture2DArray<float4> texture2_items : register(t2);
VK_BINDING(9) Texture2DArray<float4> texture3_items : register(t3);
VK_BINDING(10) Texture2DArray<float4> texture4_items : register(t4);
VK_BINDING(11) Texture2DArray<float4> texture5_items : register(t5);
VK_BINDING(12) Texture2DArray<float4> texture6_items : register(t6);
VK_BINDING(13) Texture2DArray<float4> texture7_items : register(t7);

VK_BINDING(14) Texture2DArray<float4> lightmap_items : register(t8);
VK_BINDING(15) TextureCube<float4> reflection_map : register(t9);



struct VSInput
{
  VK_LOCATION(0) float3 pos : register0;
  VK_LOCATION(1) float4 col : register1;
  VK_LOCATION(2) float3 nrm : register2;
  VK_LOCATION(3) uint msk 	: register3;
  VK_LOCATION(4) float3 tgn : register4;
  VK_LOCATION(5) float sgn : register5;
  VK_LOCATION(6) float2 tc0 : register6;
  VK_LOCATION(7) float2 tc1 : register7;
};

struct VSOutput
{
  VK_LOCATION(0) float4 w_pos_d : register0;
  VK_LOCATION(1) float4 w_nrm_u : register1;
  VK_LOCATION(2) float4 w_tng_v : register2;
  VK_LOCATION(3) float2 tc1		: register3;
  VK_LOCATION(4) uint mask 		: register4;
  VK_LOCATION(5) float4 color   : register5;
  VK_LOCATION(6) float4 pos     : SV_Position;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output = (VSOutput)0;

  output.pos = mul(camera_proj, mul(camera_view, float4(input.pos, 1.0)));
  output.w_pos_d = float4(input.pos, input.sgn);
  output.w_nrm_u = float4(input.nrm, input.tc0.x);
  output.w_tng_v = float4(input.tgn, input.tc0.y);
  output.tc1 = input.tc1;
  output.mask = input.msk;
  output.color = input.col;
  return output;
}

//float3 IndirectBRDF
//    (Surface
//    surface, BRDF
//    brdf,
//    float3 diffuse, float3 specular)
//{
//    float fresnelStrength = surface.fresnelStrength * Pow4(1.0 - saturate(dot(surface.normal, surface.view)));
//    float3 reflection = specular * lerp(brdf.specular, brdf.fresnel, fresnelStrength);
//    reflection /= brdf.roughness * brdf.roughness + 1.0;
  
  
//    return (diffuse * brdf.diffuse + reflection) * surface.occlusion;
//}

float PerceptualRoughnessToMipmapLevel(float perceptual_roughness, uint mipmap_levels_counter)
{
    perceptual_roughness = perceptual_roughness * (1.7 - 0.7 * perceptual_roughness);

    return perceptual_roughness * mipmap_levels_counter;
}

float PerceptualRoughnessToRoughness(float perceptual_roughness)
{
    return perceptual_roughness * perceptual_roughness;
}

float RoughnessToPerceptualRoughness(float roughness)
{
    return sqrt(roughness);
}

float RoughnessToPerceptualSmoothness(float roughness)
{
    return 1.0 - sqrt(roughness);
}

float PerceptualSmoothnessToRoughness(float perceptualSmoothness)
{
    return (1.0 - perceptualSmoothness) * (1.0 - perceptualSmoothness);
}

float PerceptualSmoothnessToPerceptualRoughness(float perceptualSmoothness)
{
    return (1.0 - perceptualSmoothness);
}

struct PSInput
{
  VK_LOCATION(0) float4 w_pos_d : register0;
  VK_LOCATION(1) float4 w_nrm_u : register1;
  VK_LOCATION(2) float4 w_tng_v : register2;
  VK_LOCATION(3) float2 tc1		: register3;
  VK_LOCATION(4) uint mask 		: register4;
  VK_LOCATION(5) float4 color   : register5;
};

struct PSOutput
{
  float4 target_0 : SV_Target0;
  float4 target_1 : SV_Target1;
  float4 target_2 : SV_Target2;
};

PSOutput ps_main(PSInput input)
{
  PSOutput output = (PSOutput)0;

  float3 n = normalize(input.w_nrm_u.xyz);
  float3 t = normalize(input.w_tng_v.xyz);
  float3 b = input.w_pos_d.w * cross(t, n);

//#ifdef USE_SPECULAR_SETUP
//  const float4 tex0_value = tex0_idx != -1 ? texture0_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex0_idx)) : float4(1.0, 1.0, 1.0, 1.0);
//  const float4 tex1_value = tex1_idx != -1 ? texture1_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex1_idx)) : float4(1.0, 1.0, 1.0, 1.0);
//  const float4 tex2_value = tex2_idx != -1 ? texture2_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex2_idx)) : float4(1.0, 1.0, 1.0, 1.0);
//  const float4 tex3_value = tex3_idx != -1 ? texture3_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex3_idx)) : float4(0.5, 0.5, 0.0, 0.0);
//
//  const Surface surface = Initialize_OBJ(emission, intensity, diffuse, shininess, specular, alpha, tex0_value, tex1_value, tex2_value, tex3_value);
//#else
//  const float4 tex0_value = tex0_idx != -1 ? texture0_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex0_idx)) : float4(1.0, 1.0, 1.0, 1.0);
//  const float4 tex1_value = tex1_idx != -1 ? texture1_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex1_idx)) : float4(0.0, 0.0, 0.0, 0.0);
//  const float4 tex2_value = tex2_idx != -1 ? texture2_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex2_idx)) : float4(1.0, 1.0, 1.0, 1.0);
//  const float4 tex3_value = tex3_idx != -1 ? texture3_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex3_idx)) : float4(0.0, 0.0, 1.0, 0.0);
//  const float4 tex4_value = tex4_idx != -1 ? texture4_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex4_idx)) : float4(1.0, 1.0, 1.0, 1.0);
//  
//  const Surface surface = Initialize_GLTF(emission, intensity, diffuse, shininess, specular, alpha, tex0_value, tex1_value, tex2_value, tex3_value);
//#endif

  //#ifdef USE_ALPHA_CLIP
  //  if (surface.alpha < 0.1) discard;
  //#endif
  //
//#ifdef USE_NORMAL_MAP
//  n = normalize(surface.normal.x * t + surface.normal.y * b + surface.normal.z * n);
//#endif
    
  const float3 surface_pos = input.w_pos_d.xyz;

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - surface_pos);
  const float3 camera_dir = (camera_pos - surface_pos) / camera_dst;
    
  const float3 v = -camera_dir;

  // temp mapping - only PBR materials!
  const float4 tex0_value = tex0_idx == -1 ? float4(1.0, 1.0, 1.0, 1.0) 
    : texture0_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex0_idx));
  const float4 tex1_value = tex1_idx == -1 ? float4(0.0, 0.0, 0.0, 0.0) 
    : texture1_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex1_idx));
  const float4 tex2_value = tex2_idx == -1 ? float4(1.0, 1.0, 1.0, 1.0) 
    : texture2_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex2_idx));
  const float4 tex3_value = tex3_idx == -1 ? float4(0.5, 0.5, 1.0, 0.0) 
    : texture3_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex3_idx));
  //const float4 tex3_value = float4(0.5, 0.5, 1.0, 0.0);
  const float4 tex4_value = tex4_idx == -1 ? float4(1.0, 1.0, 1.0, 1.0) 
    : texture4_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex4_idx));
    
  const float3 albedo = tex0_value.xyz;
  const float3 emission = tex1_value.xyz;
  const float metallic = tex2_value.z;
  const float roughness = tex2_value.y;
  const float3 normal = normalize(
    (2.0 * tex3_value.x - 1.0) * t +
    (2.0 * tex3_value.y - 1.0) * b +
    (2.0 * tex3_value.z - 1.0) * n);
  const float occlusion = tex4_value.x;
  
  
       
  const float one_minus_reflectivity = OneMinusReflectivityMetallic(metallic);
  const float reflectivity = 1.0 - one_minus_reflectivity;
  const float3 diffuse = albedo * one_minus_reflectivity;
  const float3 specular = lerp(kDielectricSpec.rgb, albedo, metallic);
    
  const float perceptual_roughness = RoughnessToPerceptualRoughness(roughness);
  const float perceptual_smoothness = 1.0 - perceptual_roughness;
  const float grazing_term = saturate(perceptual_smoothness + reflectivity);
  const float fresnel_term = pow(1.0 - max(dot(n,-v), 0.0), 4.0);
  float surface_reduction = 1.0 / (roughness * roughness + 1.0);
  float3 factor = surface_reduction * lerp(specular, grazing_term, fresnel_term);
    
  const float3 r = reflect(v, normal);
  const float mip = PerceptualRoughnessToMipmapLevel(perceptual_roughness, 6);
  const float3 gi_specular = reflection_map.SampleLevel(sampler1, r, mip).xyz;
    
  const float3 gi_diffuse = 0 * lightmap_items.Sample(sampler1, float3(input.tc1, input.mask)).xyz;
    


  


  const float3 global_illumination = (gi_diffuse + gi_specular * factor) * occlusion;

  output.target_0 = float4(emission + global_illumination, 1.0);
  output.target_1 = float4(albedo, metallic);
  output.target_2 = float4(0.5 * normal + 0.5, roughness);

#ifdef TEST
#endif

  return output;
};