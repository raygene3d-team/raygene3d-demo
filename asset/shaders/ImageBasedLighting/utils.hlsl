#ifndef RAYGENE3D_IBL_UTILS
#define RAYGENE3D_IBL_UTILS


float2 Hammersley2d(uint i, uint N)
{
  return float2(float(i) / float(N), reversebits(i) * 2.3283064365386963e-10);
}

float3x3 GetTBN(float3 n)
{
  //float sz = (n.z >= 0) ? 1.0 : -1.0;
  //float t0 = 1.0 / (sz + n.z);
  //float ya = n.y * t0;
  //float t1 = n.x * ya;
  //float t2 = n.x * sz;
  //const float3 t = float3(t2 * n.x * t0 - 1.0, sz * t1, t2);
  //const float3 b = float3(t1, n.y * ya - sz, n.y);
  
  const float3 t = normalize(n.y * n.y > n.x * n.x ? float3(0.0, -n.z, n.y) : float3(-n.z, 0.0, n.x));
  const float3 b = cross(n, t);
  
  return float3x3(t, b, n);
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