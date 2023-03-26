#ifndef RAYGENE3D_IBL_UTILS
#define RAYGENE3D_IBL_UTILS

  float FastSign(float x)
  {
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
  }

  float3x3 GetLocalFrame(float3 localZ)
  {
      float x  = localZ.x;
      float y  = localZ.y;
      float z  = localZ.z;
      float sz = FastSign(z);
      float a  = 1 / (sz + z);
      float ya = y * a;
      float b  = x * ya;
      float c  = x * sz;

      float3 localX = float3(c * x * a - 1, sz * b, c);
      float3 localY = float3(b, y * ya - sz, y);

      return float3x3(localX, localY, localZ);
  }

  float SinFromCos(float cosX)
  {
      return sqrt(saturate(1 - cosX * cosX));
  }

  float3 SphericalToCartesian(float cosPhi, float sinPhi, float cosTheta)
  {
      float sinTheta = SinFromCos(cosTheta);

      return float3(float2(cosPhi, sinPhi) * sinTheta, cosTheta);
  }

  float3 SphericalToCartesian(float phi, float cosTheta)
  {
      float sinPhi, cosPhi;
      sincos(phi, sinPhi, cosPhi);

      return SphericalToCartesian(cosPhi, sinPhi, cosTheta);
  }

  float SafeDiv(float numer, float denom)
  {
      return (numer != denom) ? numer / denom : 1;
  }

  float Sq(float x)
  {
    return x * x;
  }

#endif // RAYGENE3D_IBL_UTILS