#ifndef SPARK_PACKING_INCLUDED
#define SPARK_PACKING_INCLUDED

  uint3 PackFloat2To888UInt(float2 f)
  {
    uint2 i = (uint2)(f * 4095.5);
    uint2 hi = i >> 8;
    uint2 lo = i & 255;
    uint3 cb = uint3(lo, hi.x | (hi.y << 4));
    return cb;
  }

  float2 PackNormalOctQuadEncode(float3 n)
  {
    n *= rcp(max(dot(abs(n), 1.0), 1e-6));
    float t = saturate(-n.z);
    return n.xy + (n.xy >= 0.0 ? t : -t);
  }

  float2 Unpack888UIntToFloat2(uint3 x)
  {
    uint hi = x.z >> 4;
    uint lo = x.z & 15;
    uint2 cb = x.xy | uint2(lo << 8, hi << 8);

    return cb / 4095.0;
  }

  float3 UnpackNormalOctQuadEncode(float2 f)
  {
    float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    float t = max(-n.z, 0.0);
    n.xy += n.xy >= 0.0 ? -t.xx : t.xx;

    return normalize(n);
  }

  float3 PackNormalOctQuad(float3 n)
  {
    float2 compressed_normal = 0.5 * PackNormalOctQuadEncode(n) + 0.5;
    return float3(PackFloat2To888UInt(compressed_normal)) / 255.0;
  }

  float3 UnpackNormalOctQuad(float3 packed_normal)
  {
    float2 compressed_normal = 2.0 * Unpack888UIntToFloat2(uint3(packed_normal * 255.0)) - 1.0;
    return UnpackNormalOctQuadEncode(compressed_normal);
  }

  float3 PackNormalPlain(float3 n)
  {
    return 0.5 * n + 0.5;
  }

  float3 UnpackNormalPlain(float3 packed_normal)
  {
    return 2.0 * packed_normal - 1.0;
  }

  float3 PackNormal(float3 normal)
  {
    #ifdef USE_NORMAL_OCT_QUAD_PACKING
      return PackNormalOctQuad(normal);
    #else
      return PackNormalPlain(normal);
    #endif
  }

  float3 UnpackNormal(float3 packed_normal)
  {
    #ifdef USE_NORMAL_OCT_QUAD_PACKING
      return UnpackNormalOctQuad(packed_normal);
    #else
      return UnpackNormalPlain(packed_normal);
    #endif
  }

#endif