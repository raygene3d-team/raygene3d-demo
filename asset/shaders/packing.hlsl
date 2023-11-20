#ifndef SPARK_PACKING_INCLUDED
#define SPARK_PACKING_INCLUDED

uint3 PackFloat2To888UInt(float2 f)
{
  uint2 i = (uint2)(f * 4095.5);
  uint2 hi = i >> 8;
  uint2 lo = i & 255;
  // 8 bit in lo, 4 bit in hi
  uint3 cb = uint3(lo, hi.x | (hi.y << 4));
  return cb;
}

// return float between [-1, 1]
float2 PackNormalOctQuadEncode(float3 n)
{
  n *= rcp(max(dot(abs(n), 1.0), 1e-6));
  float t = saturate(-n.z);
  return n.xy + float2(n.x >= 0.0 ? t : -t, n.y >= 0.0 ? t : -t);
}

float2 Unpack888UIntToFloat2(uint3 x)
{
  // 8 bit in lo, 4 bit in hi
  uint hi = x.z >> 4;
  uint lo = x.z & 15;
  uint2 cb = x.xy | uint2(lo << 8, hi << 8);

  return cb / 4095.0;
}

float3 UnpackNormalOctQuadEncode(float2 f)
{
  float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
  // Optimized version of above code:
  float t = max(-n.z, 0.0);
  n.xy += float2(n.x >= 0.0 ? -t : t, n.y >= 0.0 ? -t : t);

  return normalize(n);
}

uint3 PackNormal(float3 n)
{
  float2 compressed_normal = 0.5 * PackNormalOctQuadEncode(n) + 0.5;
  return PackFloat2To888UInt(compressed_normal);
}

float3 UnpackNormal(uint3 packed_normal)
{
  float2 compressed_normal = 2.0 * Unpack888UIntToFloat2(packed_normal) - 1.0;
  return UnpackNormalOctQuadEncode(compressed_normal);
}

#endif