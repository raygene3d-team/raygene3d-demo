#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

VK_BINDING(0) cbuffer constant0 : register(b0)
{
  float4x4 shadow_view     : packoffset(c0.x);
  float4x4 shadow_proj     : packoffset(c4.x);
  float4x4 shadow_view_inv : packoffset(c8.x);
  float4x4 shadow_proj_inv : packoffset(c12.x);
}

struct VSInput
{
  VK_LOCATION(0) float3 pos : register0;
};

struct VSOutput
{
  VK_LOCATION(0) float4 pos : SV_Position;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;
  output.pos = mul(shadow_proj, mul(shadow_view, float4(input.pos, 1.0)));
  return output;
}
