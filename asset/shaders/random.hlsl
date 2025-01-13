inline uint MurmurHash3_mix(in uint hash, in uint k)
{
  k *= 0xcc9e2d51;
  k = (k << 15) | (k >> (32 - 15));
  k *= 0x1b873593;

  hash ^= k;
  hash = ((hash << 13) | (hash >> (32 - 13))) * 5 + 0xe6546b64;

  return hash;
}

inline uint MurmurHash3_finalize(uint hash)
{
  hash ^= hash >> 16;
  hash *= 0x85ebca6b;
  hash ^= hash >> 13;
  hash *= 0xc2b2ae35;
  hash ^= hash >> 16;

  return hash;
}

inline uint LCG_next(uint value)
{
  return value * 1664525 + 1013904223;
}

inline uint RandomSampler_init(int pixelId, int sampleId)
{
  uint hash = 0;
  hash = MurmurHash3_mix(hash, pixelId);
  hash = MurmurHash3_mix(hash, sampleId);
  hash = MurmurHash3_finalize(hash);

  return hash;
}

//inline uint MurmurHash3_initialize(in uint index, in uint scramble)
//{
//  uint hash = 0;
//  hash = MurmurHash3_mix(hash, index);
//  hash = MurmurHash3_mix(hash, scramble);
//  hash ^= hash >> 16;
//  hash *= 0x85ebca6b;
//  hash ^= hash >> 13;
//  hash *= 0xc2b2ae35;
//  hash ^= hash >> 16;
//
//  return hash;
//}

inline float RandomSampler_sample(inout uint state)
{
  state = LCG_next(state); return state * 2.3283064365386963e-10;
}