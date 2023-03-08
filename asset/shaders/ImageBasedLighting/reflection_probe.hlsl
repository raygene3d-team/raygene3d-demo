#include "ImageBasedLighting/common.hlsl"
#include "ImageBasedLighting/utils.hlsl"
#include "ImageBasedLighting/Fibonacci.hlsl"
#include "ImageBasedLighting/ImageBasedLighting.hlsl"

#define UNITY_SPECCUBE_LOD_STEPS 6
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

      // Note: if (N == V), all of the microsurface normals are visible.
      SampleGGXDir(u, V, localToWorld, roughness, L, NdotL, NdotH, LdotH, true);

      if (NdotL <= 0) continue; // Note that some samples will have 0 contribution
    }

    float mipLevel;

    if (!prefilter) // BRDF importance sampling
    {
      mipLevel = 0;
    }
    else // Prefiltered BRDF importance sampling
    {
      // Use lower MIP-map levels for fetching samples with low probabilities
      // in order to reduce the variance.
      // Ref: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch20.html
      //
      // - OmegaS: Solid angle associated with the sample
      // - OmegaP: Solid angle associated with the texel of the cubemap

      float omegaS;

      {
        // real PDF = D * NdotH * Jacobian, where Jacobian = 1 / (4 * LdotH).
        // Since (N == V), NdotH == LdotH.
        float pdf = 0.25 * D_GGX(NdotH, roughness);
        // TODO: improve the accuracy of the sample's solid angle fit for GGX.
        omegaS = rcp(sampleCount) * rcp(pdf);
      }

      // 'invOmegaP' is precomputed on CPU and provided as a parameter to the function.
      // real omegaP = FOUR_PI / (6.0 * cubemapWidth * cubemapWidth);
      const float mipBias = roughness;
      mipLevel = 0.5 * log2(omegaS * invOmegaP) + mipBias;

      //if (mipLevel > 8.0)
      //{
      //  return float4(1.0, 0.0, 0.0, 0.0);
      //}
    }

    // TODO: use a Gaussian-like filter to generate the MIP pyramid.
    //float3 val = SAMPLE_TEXTURECUBE_LOD(tex, sampl, L, mipLevel).rgb;
    float3 val = skybox_texture.SampleLevel(sampler0, L, mipLevel).rgb;

    // The goal of this function is to use Monte-Carlo integration to find
    // X = Integral{Radiance(L) * CBSDF(L, N, V) dL} / Integral{CBSDF(L, N, V) dL}.
    // Note: Integral{CBSDF(L, N, V) dL} is given by the FDG texture.
    // CBSDF  = F * D * G * NdotL / (4 * NdotL * NdotV) = F * D * G / (4 * NdotV).
    // PDF    = D * NdotH / (4 * LdotH).
    // Weight = CBSDF / PDF = F * G * LdotH / (NdotV * NdotH).
    // Since we perform filtering with the assumption that (V == N),
    // (LdotH == NdotH) && (NdotV == 1) && (Weight == F * G).
    // Therefore, after the Monte Carlo expansion of the integrals,
    // X = Sum(Radiance(L) * Weight) / Sum(Weight) = Sum(Radiance(L) * F * G) / Sum(F * G).

#ifndef USE_KARIS_APPROXIMATION
    // The choice of the Fresnel factor does not appear to affect the result.
    float F = 1; // F_Schlick(F0, LdotH);
    float G = V_SmithJointGGX(NdotL, NdotV, roughness, partLambdaV) * NdotL * NdotV; // 4 cancels out

    lightInt += F * G * val;
    cbsdfInt += F * G;
#else
    // Use the approximation from "Real Shading in Unreal Engine 4": Weight ~ NdotL.
    lightInt += NdotL * val;
    cbsdfInt += NdotL;
#endif
  }

  return float4(lightInt / cbsdfInt, 1.0);
}

real PerceptualRoughnessToRoughness(real perceptualRoughness)
{
  return perceptualRoughness * perceptualRoughness;
}

real RoughnessToPerceptualRoughness(real roughness)
{
  return sqrt(roughness);
}


real PerceptualRoughnessToMipmapLevel(real perceptualRoughness, uint maxMipLevel)
{
  perceptualRoughness = perceptualRoughness * (1.7 - 0.7 * perceptualRoughness);

  return perceptualRoughness * maxMipLevel;
}

real PerceptualRoughnessToMipmapLevel(real perceptualRoughness)
{
  return PerceptualRoughnessToMipmapLevel(perceptualRoughness, UNITY_SPECCUBE_LOD_STEPS);
}

real MipmapLevelToPerceptualRoughness(real mipmapLevel)
{
  real perceptualRoughness = saturate(mipmapLevel / UNITY_SPECCUBE_LOD_STEPS);

  return saturate(1.7 / 1.4 - sqrt(2.89 / 1.96 - (2.8 / 1.96) * perceptualRoughness));
}

uint GetIBLRuntimeFilterSampleCount(uint mipLevel)
{
  //return 400;
  uint sampleCount = 1;

  switch (mipLevel)
  {
  case 1: sampleCount = 21; break;
  case 2: sampleCount = 34; break;
  case 3: sampleCount = 55; break;
  case 4: sampleCount = 89; break;
  case 5: sampleCount = 89; break;
  case 6: sampleCount = 89; break; // UNITY_SPECCUBE_LOD_STEPS
  }

  return sampleCount;
}