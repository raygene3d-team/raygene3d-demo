#ifndef RAYGENE3D_IMAGE_BASED_LIGHTING
#define RAYGENE3D_IMAGE_BASED_LIGHTING

  void SampleGGXDir(float2   u,
                    float3   V,
                    float3x3 localToWorld,
                    float    roughness,
                out float3   L,
                out float    NdotL,
                out float    NdotH,
                out float    VdotH,
                    bool    VeqN = false)
  {
      float cosTheta = sqrt(SafeDiv(1.0 - u.x, 1.0 + (roughness * roughness - 1.0) * u.x));
      float phi      = TWO_PI * u.y;

      float3 localH = SphericalToCartesian(phi, cosTheta);

      NdotH = cosTheta;

      float3 localV;

      if (VeqN)
      {
          localV = float3(0.0, 0.0, 1.0);
          VdotH  = NdotH;
      }
      else
      {
          localV = mul(V, transpose(localToWorld));
          VdotH  = saturate(dot(localV, localH));
      }

      float3 localL = -localV + 2.0 * VdotH * localH;
      NdotL = localL.z;

      L = mul(localL, localToWorld);
  }

  float D_GGXNoPI(float NdotH, float roughness)
  {
      float a2 = Sq(roughness);
      float s = (NdotH * a2 - NdotH) * NdotH + 1.0;

      return SafeDiv(a2, s * s);
  }

  float D_GGX(float NdotH, float roughness)
  {
      return INV_PI * D_GGXNoPI(NdotH, roughness);
  }

  float GetSmithJointGGXPartLambdaV(float NdotV, float roughness)
  {
    float a2 = Sq(roughness);
    return sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
  }

  float V_SmithJointGGX(float NdotL, float NdotV, float roughness, float partLambdaV)
  {
    float a2 = Sq(roughness);
    float lambdaV = NdotL * partLambdaV;
    float lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

    return 0.5 / max(lambdaV + lambdaL, REAL_MIN);
  }

#endif // RAYGENE3D_IMAGE_BASED_LIGHTING