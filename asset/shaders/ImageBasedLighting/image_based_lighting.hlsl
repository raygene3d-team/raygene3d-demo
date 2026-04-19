#ifndef RAYGENE3D_IMAGE_BASED_LIGHTING
#define RAYGENE3D_IMAGE_BASED_LIGHTING

  void SampleGGXDir(float2 u, float3 V, float3x3 local_to_world, float roughness, out float3 L, out float NdotL, out float NdotH, out float VdotH)
  {
    float cos_theta = sqrt((1.0 - u.x) / (1.0 - (1.0 - roughness * roughness) * u.x));
    float sin_theta = sqrt(saturate(1.0 - cos_theta * cos_theta));
    
    float sin_phi, cos_phi;
    sincos(TWO_PI * u.y, sin_phi, cos_phi);

    float3 local_h = float3(float2(cos_phi, sin_phi) * sin_theta, cos_theta);

      NdotH = cos_theta;

      float3 local_v = float3(0.0, 0.0, 1.0);
      VdotH  = NdotH;

      float3 local_l = -local_v + 2.0 * VdotH * local_h;
      NdotL = local_l.z;

      L = mul(local_l, local_to_world);
  }

  float D_GGX(float NdotH, float roughness)
  {
      float a2 = roughness * roughness;
      float s = (NdotH * a2 - NdotH) * NdotH + 1.0;

    return INV_PI * (a2 / (s * s));
}

  float GetSmithJointGGXPartLambdaV(float NdotV, float roughness)
  {
    float a2 = roughness * roughness;
    return sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
  }

  float V_SmithJointGGX(float NdotL, float NdotV, float roughness, float part_lambda_v)
  {
    float a2 = roughness * roughness;
    float lambda_v = NdotL * part_lambda_v;
    float lambda_l = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

    return 0.5 / max(lambda_v + lambda_l, FLT_MIN);
  }

#endif // RAYGENE3D_IMAGE_BASED_LIGHTING