struct Surface
{
  float3 emission;
  float intensity;

  float3 diffuse;
  float alpha;

  float3 specular;
  float shininess;

  float3 normal;
  float metallic;
  float occlusion;
};

struct Frame
{
  float3 ng;
  float3x3 tbn;
  float2 duvdx;
  float2 duvdy;
};

Surface Initialize_OBJ(float3 emission, float intensity, float3 diffuse, float shininess, float3 specular, float alpha,
  float4 tex0_value, float4 tex1_value, float4 tex2_value, float4 tex3_value)
{
  Surface surface = (Surface)0;

  surface.emission = emission;
  surface.intensity = intensity;

  surface.diffuse = diffuse * tex0_value.xyz;
  surface.shininess = shininess;

  surface.specular = tex2_value.xyz;

#ifdef USE_ALPHA_CLIP
  surface.alpha = tex1_value.x;
#endif

#ifdef USE_NORMAL_MAP
  surface.normal = normalize(float3(2.0 * tex3_value.x - 1.0, 2.0 * tex3_value.y - 1.0, 0.25));
  surface.normal.x *= -1.0;
#endif

  surface.metallic = 0.0;
  surface.occlusion = 1.0;

  return surface;
}


Surface Initialize_GLTF(float3 emission, float intensity, float3 diffuse, float shininess, float3 specular, float alpha,
  float4 tex0_value, float4 tex1_value, float4 tex2_value, float4 tex3_value)
{
  Surface surface = (Surface)0;

  surface.emission = tex1_value.xyz;
  surface.intensity = intensity;

  surface.diffuse = tex0_value.xyz;


  surface.shininess = exp2(10 * tex2_value.y + 1); // 2.0 / (tex2_value.y * tex2_value.y) - 2.0;

  surface.specular = specular * tex2_value.xyz;

#ifdef USE_ALPHA_CLIP
  surface.alpha = tex1_value.x;
#endif

#ifdef USE_NORMAL_MAP
  surface.normal = float3(tex3_value.xyz * 2.0 - 1.0);
#endif

  surface.metallic = tex2_value.y;
  surface.occlusion = 1.0;

  return surface;
}
