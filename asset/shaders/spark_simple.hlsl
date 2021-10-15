#ifdef USE_SPIRV
#define VK_BINDING(x) [[vk::binding(x)]]
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_BINDING(x)
#define VK_LOCATION(x)
#endif

struct Surface
{
  float3 Ke;
  float p;
  float3 Kd;
  float m;
  float3 Ks;
  float r;
  float3 Kt;
  float i;
};

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

VK_BINDING(2) cbuffer constant1 : register(b1)
{
  uint light_size_x       : packoffset(c0.x);
  uint light_size_y       : packoffset(c0.y);
  uint light_base         : packoffset(c0.z);
  uint light_seed         : packoffset(c0.w);
  float4x3 light_view     : packoffset(c1.x);
  float4x4 light_proj     : packoffset(c4.x);
  float4x3 light_view_inv : packoffset(c8.x);
  float4x4 light_proj_inv : packoffset(c11.x);
  uint4 light_padding     : packoffset(c15.x);
}

VK_BINDING(3) cbuffer constant2 : register(b2)
{
  float4x3 transform  : packoffset(c0.x);

  float3 emission     : packoffset(c3.x);
  float intensity     : packoffset(c3.w);
  float3 diffuse      : packoffset(c4.x);
  float metallic      : packoffset(c4.w);
  float3 specular     : packoffset(c5.x);
  float roughness     : packoffset(c5.w);
  float3 transmit     : packoffset(c6.x);
  float ior           : packoffset(c6.w);

  int tex0_idx        : packoffset(c7.x);
  int tex1_idx        : packoffset(c7.y);
  int tex2_idx        : packoffset(c7.z);
  int tex3_idx        : packoffset(c7.w);

  float debug_color   : packoffset(c8.x);
  uint geometry_idx   : packoffset(c8.w);  

  uint prim_offset    : packoffset(c9.x);
  uint prim_count     : packoffset(c9.y);
  uint vert_offset    : packoffset(c9.z);
  uint vert_count     : packoffset(c9.w);

  uint4 padding[6]    : packoffset(c10.x);
};

VK_BINDING(4) Texture2DArray<float4> texture0_items : register(t0);
VK_BINDING(5) Texture2DArray<float4> texture1_items : register(t1);
VK_BINDING(6) Texture2DArray<float4> texture2_items : register(t2);
VK_BINDING(7) Texture2DArray<float4> texture3_items : register(t3);

#define USE_NORMAL_MAP
#define USE_ALPHA_CLIP

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
  VK_LOCATION(0) float4 w_pos_d : register0;
  VK_LOCATION(1) float4 w_nrm_u : register1;
  VK_LOCATION(2) float4 w_tng_v : register2;
  VK_LOCATION(3) float4 pos   : SV_Position;
};

VSOutput vs_main(VSInput input)
{
  VSOutput output;

  float4x4 view = float4x4(transpose(cam_view), float4(0.0, 0.0, 0.0, 1.0));
  output.pos = mul(cam_proj, mul(view, float4(input.pos, 1.0)));
  output.w_pos_d = float4(input.pos, input.sign);
  output.w_nrm_u = float4(input.nrm, input.u);
  output.w_tng_v = float4(input.tgn, input.v);
  return output;
}

struct PSInput
{
  VK_LOCATION(0) float4 w_pos_d : register0;
  VK_LOCATION(1) float4 w_nrm_u : register1;
  VK_LOCATION(2) float4 w_tng_v : register2;
};

struct PSOutput
{
  float4 target_0 : SV_Target0;
};

PSOutput ps_main(PSInput input)
{
  const float3 n = normalize(input.w_nrm_u.xyz);
  const float3 t = normalize(input.w_tng_v.xyz);
  const float3 b = input.w_pos_d.w * cross(n, t);
  float3x3 tbn = float3x3(t, b, n);

#ifdef USE_ALPHA_CLIP
  if (tex1_idx != -1)
  {
    const float4 tex_value = texture1_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex1_idx));
    if (tex_value.r < 0.1)
    {
      discard;
    }
  }
#endif

#ifdef USE_NORMAL_MAP
  if (tex3_idx != -1)
  {
    float3 tangent_n = float3(0.0, 0.0, 1.0);
    float3 tangent_t = float3(1.0, 0.0, 0.0);
    float3 tangent_b = float3(0.0, 1.0, 0.0);

    float4 tex_value = texture3_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex3_idx));

    tangent_n = normalize(float3(1.0 - tex_value.xy * 2.0, 0.25));
    tangent_t = normalize(tangent_t - tangent_n * dot(tangent_t, tangent_n));
    tangent_b = cross(tangent_n, tangent_b);
    tbn = mul((float3x3(tangent_t, tangent_b, tangent_n)), tbn);
  }
#endif

  Surface surface = (Surface)0;

  surface.Ke = emission;
  surface.p = intensity;
  
  surface.Kd = diffuse;
  if (tex0_idx != -1)
  {
    const float4 tex_value = texture0_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex0_idx));
    surface.Kd *= tex_value.xyz;
    //surface.d = surface.d * tex_value.a;
  }
  surface.m = metallic;

  surface.Ks = specular;
  if (tex2_idx != -1)
  {
    const float4 tex_value = texture2_items.Sample(sampler0, float3(input.w_nrm_u.w, input.w_tng_v.w, tex2_idx));
    surface.Ks *= tex_value.xyz;
    //dissolve_value = dissolve_value * tex_value.a;
  }
  surface.r = roughness;

  surface.Kt = transmit;
  surface.i = ior;

  const float3 surface_pos = input.w_pos_d.xyz;

  const float3 view_pos = float3(cam_view_inv[3][0], cam_view_inv[3][1], cam_view_inv[3][2]);
  const float view_dst = length(view_pos - surface_pos);
  const float3 view_dir = (view_pos - surface_pos) / view_dst;

  const float3 light_pos = float3(light_view_inv[3][0], light_view_inv[3][1], light_view_inv[3][2]);
  const float light_dst = length(light_pos - surface_pos);
  const float3 light_dir = (light_pos - surface_pos) / light_dst;

  const float3 lo = mul(tbn, light_dir);
  const float3 wo = mul(tbn, view_dir);

  const float ambient = 0.025;
  const float diffuse = max(0.0, lo.z) / (light_dst * light_dst) * 10.0;
  const float specular = pow(max(0.0, normalize(lo + wo).z), surface.r) / (light_dst * light_dst) * 10.0;

  const float3 color = (ambient + diffuse) * surface.Kd + surface.Ks * specular;

  PSOutput output;
  output.target_0 = float4(color, 0.0);

  return output;
};
