#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

VK_BINDING(0) cbuffer constant0 : register(b0)
{
  uint cam_size_x       : packoffset(c0.x);
  uint cam_size_y       : packoffset(c0.y);
  uint cam_base         : packoffset(c0.z);
  uint cam_seed         : packoffset(c0.w);
  float4x3 cam_view     : packoffset(c1.x);
  float4x4 cam_proj     : packoffset(c4.x);
  float4x3 cam_view_inv : packoffset(c8.x);
  float4x4 cam_proj_inv : packoffset(c11.x);
  uint4 cam_padding     : packoffset(c15.x);
}

struct VSInput
{
  VK_LOCATION(0) float3 pos : register0;
  VK_LOCATION(1) float  u : register1;
  VK_LOCATION(2) float3 nrm : register2;
  VK_LOCATION(3) float  v : register3;
  VK_LOCATION(4) float3 tgn : register4;
  VK_LOCATION(5) float  sign : register5;
};

struct VSOutput
{
  VK_LOCATION(0) float4 pos : SV_Position;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;
  float4x4 view = float4x4(transpose(cam_view), float4(0.0, 0.0, 0.0, 1.0));
  output.pos = mul(cam_proj, mul(view, float4(input.pos, 1.0)));
  return output;
}
