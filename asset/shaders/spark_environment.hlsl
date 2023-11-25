#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

static const float PI = 3.14159265;

VK_BINDING(0) sampler sampler0 : register(s0);

VK_BINDING(1) cbuffer constant0 : register(b0)
{
  uint extent_x       : packoffset(c0.x);
  uint extent_y       : packoffset(c0.y);
  uint rnd_base       : packoffset(c0.z);
  uint rnd_seed       : packoffset(c0.w);
}

VK_BINDING(2) cbuffer constant1 : register(b1)
{
  float4x4 camera_view     : packoffset(c0.x);
  float4x4 camera_proj     : packoffset(c4.x);
  float4x4 camera_view_inv : packoffset(c8.x);
  float4x4 camera_proj_inv : packoffset(c12.x);
}

VK_BINDING(3) Texture2D<float4> environment_texture : register(t0);

struct VSInput
{
  VK_LOCATION(0) float2 pos : register0;
  VK_LOCATION(1) float2 uv : register1;
};

struct VSOutput
{
  VK_LOCATION(0) float4 pos : SV_Position;
  VK_LOCATION(1) float2 uv : register0;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;
  output.pos = float4(input.pos, 1.0, 1.0);
  output.uv = input.uv;
  return output;
}

struct PSInput
{
  VK_LOCATION(0) float4 pos : SV_Position;
  VK_LOCATION(1) float2 uv : register0;
};

struct PSOutput
{
  float4 target_0 : SV_Target0;
  float4 target_1 : SV_Target1;
  float4 target_2 : SV_Target2;
};

PSOutput ps_main(PSInput input)
{
  PSOutput output;
 
  const float rx = 2.0 * (input.pos.x / extent_x) - 1.0; 
  const float ry = 2.0 * (input.pos.y / extent_y) - 1.0;
 
  const float3 screen_dir = normalize(mul(camera_proj_inv, float4(rx, -ry, 1.0, 1.0)).xyz);
  const float3 camera_dir = mul(transpose((float3x3)camera_view), screen_dir);

  const float env_s = 0.5 * atan2(camera_dir.x, camera_dir.z) / PI + 0.5;
  const float env_t = -asin(camera_dir.y) / PI + 0.5;
  const float4 env_value = environment_texture.Sample(sampler0, float2(env_s, env_t));
  
  //const float4 env_value = environment_texture.Sample(sampler0, normalize(camera_dir));

  output.target_2 = env_value;
  return output;
}
