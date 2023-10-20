#ifndef SPARK_FORWARD_INCLUDED
#define SPARK_FORWARD_INCLUDED

#include "brdf.hlsl"

#ifdef LIGHT_SHADOW
float Shadow(const float3 w_pos)
{
  const float3 light_pos = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float3 view_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float3 tc = w_pos - light_pos;

  const float m22 = shadow_proj[2][2];
  const float m23 = shadow_proj[2][3];

  const float near = -m23 / m22;
  const float far = near / (m22 - 1.0);

  const float3 atc = abs(tc);
  const float m = max(max(atc.x, atc.y), atc.z);
  const float3 tk = step(m, atc);

  const float3 dx = normalize(cross(tc, w_pos - view_pos));
  const float3 dy = normalize(cross(tc, dx));

  const float d = shadow_map.Sample(sampler1, normalize(tc));

  const float cd = far * (1.0 - near / (dot(tk, atc))) / (far - near);
  const float blur_radius = 0.005;

  float shadow = step(d, cd);
  for (uint x = 0; x < 8; ++x)
  {
    const float3 offset = (dx * poisson_disk[x / 2][2 * (x % 2)] + dy * poisson_disk[x / 2][2 * (x % 2) + 1]) * blur_radius;
    const float d = shadow_map.Sample(sampler1, normalize(tc) + offset);
    shadow += step(d, cd);
  }
  shadow /= 9.0;
  shadow *= sign(d);

  return 1.0 - shadow;
}
#endif

float4 EvaluateBlinnPhong(GeometryData geometry_data, SurfaceData surface_data)
{
  float3 n = geometry_data.normal_ws;
  float3 t = geometry_data.tanghent_ws.xyz;
  float3 b = geometry_data.tanghent_ws.w * cross(n, t);

  #ifdef USE_NORMAL_MAP
    n = normalize(surface_data.normal.x * t + surface_data.normal.y * b + surface_data.normal.z * n);
    t = normalize(t - n * dot(t, n));
    b = cross(t, n);
  #endif

  const float3x3 tbn = float3x3(t, b, n);
  const float3x3 inverse_tbn = InverseTBN(tbn);

  const float3 camera_pos = float3(camera_view_inv[0][3], camera_view_inv[1][3], camera_view_inv[2][3]);
  const float camera_dst = length(camera_pos - geometry_data.position_ws);
  const float3 view_dir_ws = (camera_pos - geometry_data.position_ws) / camera_dst;

  const float3 light_pos_ws = float3(shadow_view_inv[0][3], shadow_view_inv[1][3], shadow_view_inv[2][3]);
  const float light_dst = length(light_pos_ws - geometry_data.position_ws);
  const float3 light_dir_ws = (light_pos_ws - geometry_data.position_ws) / light_dst;

  const float3 wo = mul(tbn, view_dir_ws);
  const float3 lo = mul(tbn, light_dir_ws);

  float attenuation = 10.0 * 1.0 / (light_dst * light_dst) * max(0.0, lo.z);
#ifdef LIGHT_SHADOW
  attenuation *= Shadow(geometry_data.position_ws);
#endif
  const float3 diffuse = Evaluate_Lambert(Initialize_Lambert(surface_data), lo) * surface_data.diffuse;
  const float3 ambient = 0.025 * surface_data.diffuse + surface_data.emission;
  const float3 specular = Evaluate_BlinnPhong(Initialize_BlinnPhong(surface_data), lo, wo) * surface_data.specular;

  const float3 color = ambient + (diffuse + specular) * attenuation;

  return float4(color, 1.0);
}


#endif