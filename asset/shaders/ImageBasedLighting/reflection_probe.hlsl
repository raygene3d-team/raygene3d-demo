#include "ImageBasedLighting/common.hlsl"
#include "ImageBasedLighting/utils.hlsl"
#include "ImageBasedLighting/image_based_lighting.hlsl"

//#define USE_KARIS_APPROXIMATION

float4 IntegrateReflectionProbe(float3 V, float3 N, float roughness, float inv_omega_p, uint sample_count)
{
  float3x3 local_to_world = GetLocalFrame(N);

#ifndef USE_KARIS_APPROXIMATION
  float NdotV = 1; // N == V
  float part_lambda_v = GetSmithJointGGXPartLambdaV(NdotV, roughness);
#endif

  float3 light_int = float3(0.0, 0.0, 0.0);
  float  cbsdf_int = 0.0;

  if (sample_count == 1) return skybox_texture.SampleLevel(sampler0, N, 0);

  for (uint i = 0; i < sample_count; ++i)
  {
    float3 L;
    float  NdotL, NdotH, LdotH;

    {
      float2 uv = Hammersley2d(i, sample_count);
      SampleGGXDir(uv, V, local_to_world, roughness, L, NdotL, NdotH, LdotH);

      if (NdotL <= 0) continue;
    }

    float pdf = 0.25 * D_GGX(NdotH, roughness);
    float omega_s = rcp(sample_count) * rcp(pdf);

    const float mip_bias = roughness;
    float mip_level = 0.5 * log2(omega_s * inv_omega_p) + mip_bias;

    float3 val = skybox_texture.SampleLevel(sampler0, L, mip_level).rgb;

#ifndef USE_KARIS_APPROXIMATION
    float F = 1;
    float G = V_SmithJointGGX(NdotL, NdotV, roughness, part_lambda_v) * NdotL * NdotV;

    light_int += F * G * val;
    cbsdf_int += F * G;
#else
    light_int += NdotL * val;
    cbsdf_int += NdotL;
#endif
  }

  return float4(light_int / cbsdf_int, 1.0);
}

float PerceptualRoughnessToRoughness(float perceptual_roughness)
{
  return perceptual_roughness * perceptual_roughness;
}

float RoughnessToPerceptualRoughness(float roughness)
{
  return sqrt(roughness);
}


float PerceptualRoughnessToMipmapLevel(float perceptual_roughness, uint mipmap_levels_counter)
{
  perceptual_roughness = perceptual_roughness * (1.7 - 0.7 * perceptual_roughness);

  return perceptual_roughness * mipmap_levels_counter;
}

float MipmapLevelToPerceptualRoughness(float mipmap_level, float mipmap_levels_counter)
{
  float perceptual_roughness = saturate(mipmap_level / mipmap_levels_counter);

  return saturate(1.7 / 1.4 - sqrt(2.89 / 1.96 - (2.8 / 1.96) * perceptual_roughness));
}

uint GetSampleCount(uint mip_level)
{
  uint sample_count = 1;

  switch (mip_level)
  {
    case 0: sample_count = 1; break;
    case 1: sample_count = 20; break;
    case 2: sample_count = 40; break;
    case 3: sample_count = 60; break;
    case 4: sample_count = 80; break;
    case 5: sample_count = 100; break;
    case 6: sample_count = 120; break;
  }

  return sample_count;
}