struct BRDF_Lambert
{
  float3 color;
  float dummy;
};

BRDF_Lambert Initialize_Lambert(SurfaceData surface)
{
  BRDF_Lambert brdf = (BRDF_Lambert)0;

  brdf.color = surface.diffuse;

  return brdf;
}

float3 Evaluate_Lambert(BRDF_Lambert brdf, float3 lo)
{
  float res = 1.0;
  return float3(res, res, res);
}


struct BRDF_BlinnPhong
{
  float3 color;
  float shininess;
};

BRDF_BlinnPhong Initialize_BlinnPhong(SurfaceData surface)
{
  BRDF_BlinnPhong brdf = (BRDF_BlinnPhong)0;

  brdf.color = surface.specular;
  brdf.shininess = surface.shininess;

  return brdf;
}

float3 Evaluate_BlinnPhong(BRDF_BlinnPhong brdf, float3 lo, float3 wo)
{
  float res = pow(max(0.0, normalize(wo + lo).z), brdf.shininess);
  return float3(res, res, res);
}


struct BRDF_CookTorrance
{
  float3 color;
  float roughness;
  float metallic;
};

BRDF_CookTorrance Initialize_CookTorrance(SurfaceData surface)
{
  BRDF_CookTorrance brdf = (BRDF_CookTorrance)0;

  brdf.color = surface.diffuse;
  brdf.roughness = sqrt(2.0 / (surface.shininess + 2.0));
  brdf.metallic = surface.metallic;

  return brdf;
}


float3 Evaluate_CookTorrance(BRDF_CookTorrance brdf, float3 lo, float3 wo)
{
  ////float3 lightDirectionWSFloat3 = float3(lightDirectionWS);
  //float3 halfDir = normalize(lightDirectionWS + float3(viewDirectionWS));
  //
  //float NoH = saturate(dot(float3(normalWS), halfDir));
  //float LoH = float(saturate(dot(lightDirectionWS, halfDir)));

  //  float3 result = float3(0.0, 0.0, 0.0);

  //const float oneMinusReflectivity = OneMinusReflectivityMetallic(data.metallic);
  //const float reflectivity = 1.0 - oneMinusReflectivity;

  //const float3 diffuse = albedo * oneMinusReflectivity;
  //const float3 specular = lerp(kDielectricSpec.rgb, albedo, metallic);

  //const float diffuseTerm = 1.0;
  //result += diffuse * diffuseTerm;

  //const float perceptualRoughness = PerceptualSmoothnessToPerceptualRoughness(data.smoothness);
  //const float roughness = max(PerceptualRoughnessToRoughness(perceptualRoughness), HALF_MIN_SQRT);
  const float roughness2 = brdf.roughness * brdf.roughness; // , 6.103515625e-5);
  ////const float grazingTerm = saturate(smoothness + reflectivity);
  const float normalizationTerm = brdf.roughness * 4.0 + 2.0;
  const float roughness2MinusOne = roughness2 - 1.0;

  const float NoL = lo.z;
  //half3 radiance = lightColor * (lightAttenuation * NdotL);

  //float3 lightDirectionWSFloat3 = float3(lightDirectionWS);
  const float3 H = normalize(wo + lo);

  const float NoH = H.z;
  const float LoH = max(0.0, dot(lo, H));

  // GGX Distribution multiplied by combined approximation of Visibility and Fresnel
  // BRDFspec = (D * V * F) / 4.0
  // D = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2
  // V * F = 1.0 / ( LoH^2 * (roughness + 0.5) )
  // See "Optimizing PBR for Mobile" from Siggraph 2015 moving mobile graphics course
  // https://community.arm.com/events/1155

  // Final BRDFspec = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2 * (LoH^2 * (roughness + 0.5) * 4.0)
  // We further optimize a few light invariant terms
  // brdfData.normalizationTerm = (roughness + 0.5) * 4.0 rewritten as roughness * 4.0 + 2.0 to a fit a MAD.
  const float d = NoH * NoH * roughness2MinusOne + 1.0; // 0001f;

  const float LoH2 = LoH * LoH;
  const float res = roughness2 / ((d * d) * max(0.0, LoH2) * normalizationTerm);

  return float3(res, res, res); // *brdf.color;
}
