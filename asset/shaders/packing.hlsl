#ifndef SPARK_PACKING_INCLUDED
#define SPARK_PACKING_INCLUDED

#define NORMAL_NONE_PACKING 0
#define NORMAL_OCT_QUAD_PACKING 1
#define NORMAL_SPHEREMAP_PACKING 2

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

  float2 PackNormalSphereMap(float3 n)
  {
    float f = sqrt(8.0 * n.z + 8.0);
    return n.xy / f + 0.5;
  }

  float3 UnpackNormalSphereMap(float2 packed_normal)
  {
    float2 fenc = packed_normal * 4.0 - 2.0;
    float f = dot(fenc, fenc);
    float g = sqrt(1.0 - f / 4.0);
    return float3(fenc * g, 1.0 - f / 2.0);
  }

  float3 PackNormalPlain(float3 n)
  {
    return 0.5 * n + 0.5;
  }

  float3 UnpackNormalPlain(float3 packed_normal)
  {
    return 2.0 * packed_normal - 1.0;
  }

  float4 PackNormalAndSmoothness(float3 normal, float smoothness)
  {
    #if NORMAL_ENCODING_ALGORITHM == NORMAL_OCT_QUAD_PACKING
      return float4(PackNormalOctQuad(normal), smoothness);
    #elif NORMAL_ENCODING_ALGORITHM == NORMAL_SPHEREMAP_PACKING
      return float4(PackNormalSphereMap(normal), smoothness, 0.0);
    #else
      return float4(PackNormalPlain(normal), smoothness);
    #endif
  }

  void UnpackNormalAndSmoothness(float4 packed, out float3 normal, out float smoothness)
  {
    #if NORMAL_ENCODING_ALGORITHM == NORMAL_OCT_QUAD_PACKING
      normal = UnpackNormalOctQuad(packed.xyz);
      smoothness = packed.a;
    #elif NORMAL_ENCODING_ALGORITHM == NORMAL_SPHEREMAP_PACKING
      normal = UnpackNormalSphereMap(packed.xy);
      smoothness = packed.z;
    #else
      normal = UnpackNormalPlain(packed.xyz);
      smoothness = packed.a;
    #endif
  }

#endif