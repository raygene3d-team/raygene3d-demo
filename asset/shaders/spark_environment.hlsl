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

VK_BINDING(3) TextureCube<float4> skybox_texture : register(t0);

struct VSInput
{
  VK_LOCATION(0) float2 pos : register0;
};

struct VSOutput
{
  VK_LOCATION(0) float4 pos : SV_Position;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;
  output.pos = float4(input.pos, 1.0, 1.0);
  return output;
}

struct PSInput
{
  VK_LOCATION(0) float4 pos : SV_Position;
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
    
  const float4 ndc_coord = float4(rx, -ry, 1.0, 1.0);
  const float4 view_pos = mul(camera_proj_inv, ndc_coord);
  const float3 view_dir = mul(camera_view_inv, view_pos / view_pos.w).xyz;

  const float4 skybox = skybox_texture.Sample(sampler0, view_dir);
    
  output.target_0 = skybox;
  output.target_1 = float4(0.0, 0.0, 0.0, 0.0);
  output.target_2 = float4(0.0, 0.0, 0.0, 0.0);
  return output;
}
