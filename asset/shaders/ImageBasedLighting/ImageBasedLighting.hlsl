#ifndef RAYGENE3D_IMAGE_BASED_LIGHTING
#define RAYGENE3D_IMAGE_BASED_LIGHTING

  void SampleGGXDir(real2   u,
                    real3   V,
                    real3x3 localToWorld,
                    real    roughness,
                out real3   L,
                out real    NdotL,
                out real    NdotH,
                out real    VdotH,
                    bool    VeqN = false)
  {
      // GGX NDF sampling
      real cosTheta = sqrt(SafeDiv(1.0 - u.x, 1.0 + (roughness * roughness - 1.0) * u.x));
      real phi      = TWO_PI * u.y;

      real3 localH = SphericalToCartesian(phi, cosTheta);

      NdotH = cosTheta;

      real3 localV;

      if (VeqN)
      {
          // localV == localN
          localV = real3(0.0, 0.0, 1.0);
          VdotH  = NdotH;
      }
      else
      {
          localV = mul(V, transpose(localToWorld));
          VdotH  = saturate(dot(localV, localH));
      }

      // Compute { localL = reflect(-localV, localH) }
      real3 localL = -localV + 2.0 * VdotH * localH;
      NdotL = localL.z;

      L = mul(localL, localToWorld);
  }

  real D_GGXNoPI(real NdotH, real roughness)
  {
      real a2 = Sq(roughness);
      real s = (NdotH * a2 - NdotH) * NdotH + 1.0;

      // If roughness is 0, returns (NdotH == 1 ? 1 : 0).
      // That is, it returns 1 for perfect mirror reflection, and 0 otherwise.
      return SafeDiv(a2, s * s);
  }

  real D_GGX(real NdotH, real roughness)
  {
      return INV_PI * D_GGXNoPI(NdotH, roughness);
  }

  // Precompute part of lambdaV
  real GetSmithJointGGXPartLambdaV(real NdotV, real roughness)
  {
    real a2 = Sq(roughness);
    return sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
  }

  // Note: V = G / (4 * NdotL * NdotV)
  // Ref: http://jcgt.org/published/0003/02/03/paper.pdf
    real V_SmithJointGGX(real NdotL, real NdotV, real roughness, real partLambdaV)
    {
      real a2 = Sq(roughness);

      // Original formulation:
      // lambda_v = (-1 + sqrt(a2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5
      // lambda_l = (-1 + sqrt(a2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5
      // G        = 1 / (1 + lambda_v + lambda_l);

      // Reorder code to be more optimal:
      real lambdaV = NdotL * partLambdaV;
      real lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

      // Simplify visibility term: (2.0 * NdotL * NdotV) /  ((4.0 * NdotL * NdotV) * (lambda_v + lambda_l))
      return 0.5 / max(lambdaV + lambdaL, REAL_MIN);
    }

#endif // RAYGENE3D_IMAGE_BASED_LIGHTING