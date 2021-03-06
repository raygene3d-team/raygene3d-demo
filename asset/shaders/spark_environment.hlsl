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

VK_BINDING(2) Texture2D<float4> environment_texture : register(t0);

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
  float4x4 view = float4x4(transpose(cam_view), float4(0.0, 0.0, 0.0, 1.0));
  output.pos = float4(input.pos, 1.0, 1.0); //mul(cam_proj, mul(view, float4(input.pos, 0.5, 1.0)));
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
};

PSOutput ps_main(PSInput input)
{
  PSOutput output;
 
  const float rx = 2.0 * (input.pos.x / cam_size_x) - 1.0; 
  const float ry = 2.0 * (input.pos.y / cam_size_y) - 1.0;
 
  const float3 camera_dir = normalize(mul(cam_proj_inv, float4(rx, -ry, 1.0, 1.0)).xyz);
  const float3 view_dir = mul((float3x3)cam_view, camera_dir);
  

  const float env_s = 0.5 * atan2(view_dir.x, view_dir.z) / PI + 0.5;
  const float env_t = -asin(view_dir.y) / PI + 0.5;
  const float4 env_value = environment_texture.Sample(sampler0, float2(env_s, env_t));

  //uint env_tex_w = 0;
  //uint env_tex_h = 0;
  //environment_texture.GetDimensions(env_tex_w, env_tex_h);
  //const float4 env_value = environment_texture.Load(int3(int(env_tex_w * env_s), int(env_tex_h * env_t), 0));

  output.target_0 = env_value;
  return output;
}
