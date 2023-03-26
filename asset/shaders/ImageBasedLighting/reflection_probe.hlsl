#include "ImageBasedLighting/common.hlsl"
#include "ImageBasedLighting/utils.hlsl"
#include "ImageBasedLighting/fibonacci.hlsl"
#include "ImageBasedLighting/image_based_lighting.hlsl"

#define REFLECTION_SPECCUBE_LOD_STEPS 6
//#define USE_KARIS_APPROXIMATION

float radicalInverse_VdC(uint bits) {
  bits = (bits << 16u) | (bits >> 16u);
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 hammersley2d(uint i, uint N)
{
  return float2(float(i) / float(N), radicalInverse_VdC(i));
}

float4 IntegrateLD(
  float3 V,
  float3 N,
  float roughness,
  float invOmegaP,
  uint sampleCount, // Must be a Fibonacci number
  bool prefilter)
{
  float3x3 localToWorld = GetLocalFrame(N);

#ifndef USE_KARIS_APPROXIMATION
  float NdotV = 1; // N == V
  float partLambdaV = GetSmithJointGGXPartLambdaV(NdotV, roughness);
#endif

  float3 lightInt = float3(0.0, 0.0, 0.0);
  float  cbsdfInt = 0.0;

  for (uint i = 0; i < sampleCount; ++i)
  {
    float3 L;
    float  NdotL, NdotH, LdotH;

    {
      float2 u = Fibonacci2d(i, sampleCount);
      //float2 u = hammersley2d(i, sampleCount);

      SampleGGXDir(u, V, localToWorld, roughness, L, NdotL, NdotH, LdotH, true);

      if (NdotL <= 0) continue;
    }

    float mipLevel;

    if (!prefilter)
    {
      mipLevel = 0;
    }
    else
    {
      float omegaS;

      {
        float pdf = 0.25 * D_GGX(NdotH, roughness);
        omegaS = rcp(sampleCount) * rcp(pdf);
      }

      const float mipBias = roughness;
      mipLevel = 0.5 * log2(omegaS * invOmegaP) + mipBias;
    }

    float3 val = skybox_texture.SampleLevel(sampler0, L, mipLevel).rgb;

#ifndef USE_KARIS_APPROXIMATION
    float F = 1; // F_Schlick(F0, LdotH);
    float G = V_SmithJointGGX(NdotL, NdotV, roughness, partLambdaV) * NdotL * NdotV;

    lightInt += F * G * val;
    cbsdfInt += F * G;
#else
    lightInt += NdotL * val;
    cbsdfInt += NdotL;
#endif
  }

  return float4(lightInt / cbsdfInt, 1.0);
}

float PerceptualRoughnessToRoughness(float perceptualRoughness)
{
  return perceptualRoughness * perceptualRoughness;
}

float RoughnessToPerceptualRoughness(float roughness)
{
  return sqrt(roughness);
}


float PerceptualRoughnessToMipmapLevel(float perceptualRoughness, uint maxMipLevel)
{
  perceptualRoughness = perceptualRoughness * (1.7 - 0.7 * perceptualRoughness);

  return perceptualRoughness * maxMipLevel;
}

float PerceptualRoughnessToMipmapLevel(float perceptualRoughness)
{
  return PerceptualRoughnessToMipmapLevel(perceptualRoughness, REFLECTION_SPECCUBE_LOD_STEPS);
}

float MipmapLevelToPerceptualRoughness(float mipmapLevel)
{
  float perceptualRoughness = saturate(mipmapLevel / REFLECTION_SPECCUBE_LOD_STEPS);

  return saturate(1.7 / 1.4 - sqrt(2.89 / 1.96 - (2.8 / 1.96) * perceptualRoughness));
}

uint GetIBLRuntimeFilterSampleCount(uint mipLevel)
{
  uint sampleCount = 1;

  switch (mipLevel)
  {
  case 1: sampleCount = 21; break;
  case 2: sampleCount = 34; break;
  case 3: sampleCount = 55; break;
  case 4: sampleCount = 89; break;
  case 5: sampleCount = 89; break;
  case 6: sampleCount = 89; break;
  }

  return sampleCount;
}