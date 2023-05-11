#ifndef RAYGENE3D_IBL_UTILS
#define RAYGENE3D_IBL_UTILS

  float radicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
  }

  float2 Hammersley2d(uint i, uint N)
  {
    return float2(float(i) / float(N), radicalInverse_VdC(i));
  }

  float FastSign(float x)
  {
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
  }

  float3x3 GetLocalFrame(float3 local_z)
  {
      float x  = local_z.x;
      float y  = local_z.y;
      float z  = local_z.z;
      float sz = FastSign(z);
      float a  = 1.0 / (sz + z);
      float ya = y * a;
      float b  = x * ya;
      float c  = x * sz;

      float3 local_x = float3(c * x * a - 1.0, sz * b, c);
      float3 local_y = float3(b, y * ya - sz, y);

      return float3x3(local_x, local_y, local_z);
  }

  float3 SphericalToCartesian(float phi, float cos_theta)
  {
      float sin_phi, cos_phi;
      sincos(phi, sin_phi, cos_phi);

      float sin_theta = sqrt(saturate(1.0 - cos_theta * cos_theta));

      return float3(float2(cos_phi, sin_phi) * sin_theta, cos_theta);
  }

  float SafeDiv(float numer, float denom)
  {
      return (numer != denom) ? numer / denom : 1;
  }

#endif // RAYGENE3D_IBL_UTILS