#define kDielectricSpec float4(0.04, 0.04, 0.04, 1.0 - 0.04)

float OneMinusReflectivityMetallic(float metallic)
{
    float oneMinusDielectricSpec = kDielectricSpec.a;
    return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
}

struct BRDF_Lambert
{
  float3 color;
  float dummy;
};

BRDF_Lambert Initialize_Lambert(Surface surface)
{
  BRDF_Lambert brdf = (BRDF_Lambert)0;

  brdf.color = surface.diffuse;

  return brdf;
}

float3 Evaluate_Lambert(BRDF_Lambert brdf, float3 lo, float3 wo)
{
  float res = 1.0;
  return float3(res, res, res); // *brdf.color;
}


struct BRDF_BlinnPhong
{
  float3 color;
  float shininess;
};

BRDF_BlinnPhong Initialize_BlinnPhong(Surface surface)
{
  BRDF_BlinnPhong brdf = (BRDF_BlinnPhong)0;

  brdf.color = surface.specular;
  brdf.shininess = surface.shininess;

  return brdf;
}

float3 Evaluate_BlinnPhong(BRDF_BlinnPhong brdf, float3 lo, float3 wo)
{
  float res = pow(max(0.0, normalize(wo + lo).z), brdf.shininess);
  return float3(res, res, res); // *brdf.color;
}


struct BRDF_CookTorrance
{
  float3 color;
  float roughness;
  float metallic;
};

BRDF_CookTorrance Initialize_CookTorrance(Surface surface)
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

  //const float NoL = max(0.0, wo.z);
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


//struct BRDFData
//{
//  float3 specular;
//  float smoothness;
//};
//
//#define HALF_MIN_SQRT 0.0078125
//#define HALF_MIN 6.103515625e-5
//#define FLT_MIN 1.175494351e-38
//
//
//inline float Pow4(float x)
//{
//  return x * x * x * x;
//}
//
//float3 SafeNormalize(float3 inVec)
//{
//  float dp3 = max(FLT_MIN, dot(inVec, inVec));
//  return inVec * rsqrt(dp3);
//}
//
//float4 SampleAlbedoAlpha(float2 uv)
//{
//  return texture0_items.Sample(sampler0, float3(uv, tex0_idx));
//}
//
//half4 SampleMetallicSpecGloss(float2 uv, half albedoAlpha)
//{
//  half4 specGloss;
//
//#ifdef _METALLICSPECGLOSSMAP
//  specGloss = half4(SAMPLE_METALLICSPECULAR(uv));
//#ifdef _SMOOTHNESS_TEXTURE_ALBEDO_CHANNEL_A
//  specGloss.a = albedoAlpha * _Smoothness;
//#else
//  specGloss.a *= _Smoothness;
//#endif
//#else // _METALLICSPECGLOSSMAP
//#if _SPECULAR_SETUP
//  specGloss.rgb = specular;// _SpecColor.rgb;
//#else
//  specGloss.rgb = float3(metallic, metallic, metallic);//_Metallic.rrr;
//#endif
//
//#ifdef _SMOOTHNESS_TEXTURE_ALBEDO_CHANNEL_A
//  specGloss.a = albedoAlpha * _Smoothness;
//#else
//  specGloss.a = roughness;//_Smoothness;
//#endif
//#endif
//
//  return specGloss;
//}
//
//inline void InitializeStandardLitSurfaceData(float2 uv, out SurfaceData outSurfaceData)
//{
//  half4 albedoAlpha = SampleAlbedoAlpha(uv);
//  outSurfaceData.alpha = 1.0f;// Alpha(albedoAlpha.a, _BaseColor, _Cutoff);
//
//  half4 specGloss = SampleMetallicSpecGloss(uv, albedoAlpha.a);
//  outSurfaceData.albedo = albedoAlpha.rgb * diffuse;
//  //outSurfaceData.albedo = AlphaModulate(outSurfaceData.albedo, outSurfaceData.alpha);
//
//#if _SPECULAR_SETUP
//  outSurfaceData.metallic = half(1.0);
//  outSurfaceData.specular = specGloss.rgb;
//#else
//  outSurfaceData.metallic = specGloss.r;
//  outSurfaceData.specular = half3(0.0, 0.0, 0.0);
//#endif
//
//  outSurfaceData.smoothness = specGloss.a;
//  //outSurfaceData.normalTS = SampleNormal(uv, TEXTURE2D_ARGS(_BumpMap, sampler_BumpMap), _BumpScale);
//  outSurfaceData.occlusion = 1.0;// SampleOcclusion(uv);
//  outSurfaceData.emission = 0.0;// SampleEmission(uv, _EmissionColor.rgb, TEXTURE2D_ARGS(_EmissionMap, sampler_EmissionMap));
//
//#if defined(_CLEARCOAT) || defined(_CLEARCOATMAP)
//  half2 clearCoat = SampleClearCoat(uv);
//  outSurfaceData.clearCoatMask = clearCoat.r;
//  outSurfaceData.clearCoatSmoothness = clearCoat.g;
//#else
//  outSurfaceData.clearCoatMask = half(0.0);
//  outSurfaceData.clearCoatSmoothness = half(0.0);
//#endif
//
//#if defined(_DETAIL)
//  half detailMask = SAMPLE_TEXTURE2D(_DetailMask, sampler_DetailMask, uv).a;
//  float2 detailUv = uv * _DetailAlbedoMap_ST.xy + _DetailAlbedoMap_ST.zw;
//  outSurfaceData.albedo = ApplyDetailAlbedo(detailUv, outSurfaceData.albedo, detailMask);
//  outSurfaceData.normalTS = ApplyDetailNormal(detailUv, outSurfaceData.normalTS, detailMask);
//#endif
//}
//
//
//float OneMinusReflectivityMetallic(float metallic)
//{
//  float4 kDielectricSpec = float4(0.04, 0.04, 0.04, 1.0 - 0.04);
//  // We'll need oneMinusReflectivity, so
//  //   1-reflectivity = 1-lerp(dielectricSpec, 1, metallic) = lerp(1-dielectricSpec, 0, metallic)
//  // store (1-dielectricSpec) in kDielectricSpec.a, then
//  //   1-reflectivity = lerp(alpha, 0, metallic) = alpha + metallic*(0 - alpha) =
//  //                  = alpha - metallic * alpha
//  float oneMinusDielectricSpec = kDielectricSpec.a;
//  return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
//}
//
//float PerceptualSmoothnessToPerceptualRoughness(float perceptualSmoothness)
//{
//  return (1.0 - perceptualSmoothness);
//}
//
//float PerceptualRoughnessToRoughness(float perceptualRoughness)
//{
//  return perceptualRoughness * perceptualRoughness;
//}
//
//inline void InitializeBRDFDataDirect(half3 albedo, half3 diffuse, half3 specular, half reflectivity, half oneMinusReflectivity, half smoothness, inout half alpha, out BRDFData outBRDFData)
//{
//  outBRDFData = (BRDFData)0;
//  outBRDFData.albedo = albedo;
//  outBRDFData.diffuse = diffuse;
//  outBRDFData.specular = specular;
//  outBRDFData.reflectivity = reflectivity;
//
//  outBRDFData.perceptualRoughness = PerceptualSmoothnessToPerceptualRoughness(smoothness);
//  outBRDFData.roughness = max(PerceptualRoughnessToRoughness(outBRDFData.perceptualRoughness), HALF_MIN_SQRT);
//  outBRDFData.roughness2 = max(outBRDFData.roughness * outBRDFData.roughness, HALF_MIN);
//  outBRDFData.grazingTerm = saturate(smoothness + reflectivity);
//  outBRDFData.normalizationTerm = outBRDFData.roughness * half(4.0) + half(2.0);
//  outBRDFData.roughness2MinusOne = outBRDFData.roughness2 - half(1.0);
//
//  // Input is expected to be non-alpha-premultiplied while ROP is set to pre-multiplied blend.
//  // We use input color for specular, but (pre-)multiply the diffuse with alpha to complete the standard alpha blend equation.
//  // In shader: Cs' = Cs * As, in ROP: Cs' + Cd(1-As);
//  // i.e. we only alpha blend the diffuse part to background (transmittance).
//#if defined(_ALPHAPREMULTIPLY_ON)
//    // TODO: would be clearer to multiply this once to accumulated diffuse lighting at end instead of the surface property.
//  outBRDFData.diffuse *= alpha;
//#endif
//}
//
//inline void InitializeBRDFData(half3 albedo, half metallic, half3 specular, half smoothness, inout half alpha, out BRDFData outBRDFData)
//{
//  half4 kDielectricSpec = half4(0.04, 0.04, 0.04, 1.0 - 0.04);
//#ifdef _SPECULAR_SETUP
//  half reflectivity = ReflectivitySpecular(specular);
//  half oneMinusReflectivity = half(1.0) - reflectivity;
//  half3 brdfDiffuse = albedo * oneMinusReflectivity;
//  half3 brdfSpecular = specular;
//#else
//  half oneMinusReflectivity = OneMinusReflectivityMetallic(metallic);
//  half reflectivity = half(1.0) - oneMinusReflectivity;
//  half3 brdfDiffuse = albedo * oneMinusReflectivity;
//  half3 brdfSpecular = lerp(kDielectricSpec.rgb, albedo, metallic);
//#endif
//
//  InitializeBRDFDataDirect(albedo, brdfDiffuse, brdfSpecular, reflectivity, oneMinusReflectivity, smoothness, alpha, outBRDFData);
//}
//
//
//
//inline void InitializeBRDFData(inout SurfaceData surfaceData, out BRDFData brdfData)
//{
//  InitializeBRDFData(surfaceData.albedo, surfaceData.metallic, surfaceData.specular, surfaceData.smoothness, surfaceData.alpha, brdfData);
//}
//
////half SampleAmbientOcclusion(float2 normalizedScreenSpaceUV)
////{
////  float2 uv = UnityStereoTransformScreenSpaceTex(normalizedScreenSpaceUV);
////  return half(SAMPLE_TEXTURE2D_X(_ScreenSpaceOcclusionTexture, sampler_ScreenSpaceOcclusionTexture, uv).x);
////}
////
////AmbientOcclusionFactor GetScreenSpaceAmbientOcclusion(float2 normalizedScreenSpaceUV)
////{
////  AmbientOcclusionFactor aoFactor;
////
////#if defined(_SCREEN_SPACE_OCCLUSION) && !defined(_SURFACE_TYPE_TRANSPARENT)
////  float ssao = SampleAmbientOcclusion(normalizedScreenSpaceUV);
////
////  aoFactor.indirectAmbientOcclusion = ssao;
////  aoFactor.directAmbientOcclusion = lerp(half(1.0), ssao, _AmbientOcclusionParam.w);
////#else
////  aoFactor.directAmbientOcclusion = 1;
////  aoFactor.indirectAmbientOcclusion = 1;
////#endif
////
////#if defined(DEBUG_DISPLAY)
////  switch (_DebugLightingMode)
////  {
////  case DEBUGLIGHTINGMODE_LIGHTING_WITHOUT_NORMAL_MAPS:
////    aoFactor.directAmbientOcclusion = 0.5;
////    aoFactor.indirectAmbientOcclusion = 0.5;
////    break;
////
////  case DEBUGLIGHTINGMODE_LIGHTING_WITH_NORMAL_MAPS:
////    aoFactor.directAmbientOcclusion *= 0.5;
////    aoFactor.indirectAmbientOcclusion *= 0.5;
////    break;
////  }
////#endif
////
////  return aoFactor;
////}
////
////AmbientOcclusionFactor CreateAmbientOcclusionFactor(float2 normalizedScreenSpaceUV, half occlusion)
////{
////  AmbientOcclusionFactor aoFactor = GetScreenSpaceAmbientOcclusion(normalizedScreenSpaceUV);
////
////  aoFactor.indirectAmbientOcclusion = min(aoFactor.indirectAmbientOcclusion, occlusion);
////  return aoFactor;
////}
////
////AmbientOcclusionFactor CreateAmbientOcclusionFactor(InputData inputData, SurfaceData surfaceData)
////{
////  return CreateAmbientOcclusionFactor(inputData.normalizedScreenSpaceUV, surfaceData.occlusion);
////}
//
//Light GetMainLight()
//{
//  Light light;
//  light.direction = normalize(half3(1.0, 1.0, 1.0));
//#if USE_FORWARD_PLUS
//  light.distanceAttenuation = 1.0;
//#else
//  light.distanceAttenuation = 1.0;//unity_LightData.z; // unity_LightData.z is 1 when not culled by the culling mask, otherwise 0.
//#endif
//  light.shadowAttenuation = 1.0;
//  light.color = half3(1.0, 1.0, 1.0);//_MainLightColor.rgb;
//
//  //light.layerMask = _MainLightLayerMask;
//
//  return light;
//}
//
//Light GetMainLight(float4 shadowCoord)
//{
//  Light light = GetMainLight();
//  light.shadowAttenuation = 1.0;// MainLightRealtimeShadow(shadowCoord);
//  return light;
//}
//
//Light GetMainLight(float4 shadowCoord, float3 positionWS, half4 shadowMask)
//{
//  Light light = GetMainLight();
//  //light.shadowAttenuation = MainLightShadow(shadowCoord, positionWS, shadowMask, _MainLightOcclusionProbes);
//
//#if defined(_LIGHT_COOKIES)
//  real3 cookieColor = SampleMainLightCookie(positionWS);
//  light.color *= cookieColor;
//#endif
//
//  return light;
//}
//
//Light GetMainLight(InputData inputData, half4 shadowMask, AmbientOcclusionFactor aoFactor)
//{
//  Light light = GetMainLight(inputData.shadowCoord, inputData.positionWS, shadowMask);
//
//#if defined(_SCREEN_SPACE_OCCLUSION) && !defined(_SURFACE_TYPE_TRANSPARENT)
//  if (IsLightingFeatureEnabled(DEBUGLIGHTINGFEATUREFLAGS_AMBIENT_OCCLUSION))
//  {
//    light.color *= aoFactor.directAmbientOcclusion;
//  }
//#endif
//
//  return light;
//}
//
//void MixRealtimeAndBakedGI(inout Light light, half3 normalWS, inout half3 bakedGI)
//{
//#if defined(LIGHTMAP_ON) && defined(_MIXED_LIGHTING_SUBTRACTIVE)
//  bakedGI = SubtractDirectMainLightFromLightmap(light, normalWS, bakedGI);
//#endif
//}
//
//LightingData CreateLightingData(InputData inputData, SurfaceData surfaceData)
//{
//  LightingData lightingData;
//
//  lightingData.giColor = inputData.bakedGI;
//  lightingData.emissionColor = surfaceData.emission;
//  lightingData.vertexLightingColor = 0;
//  lightingData.mainLightColor = 0;
//  lightingData.additionalLightsColor = 0;
//
//  return lightingData;
//}
//
//half3 GlossyEnvironmentReflection(half3 reflectVector, float3 positionWS, half perceptualRoughness, half occlusion)
//{
//#if !defined(_ENVIRONMENTREFLECTIONS_OFF)
//  half3 irradiance;
//
//#ifdef _REFLECTION_PROBE_BLENDING
//  irradiance = CalculateIrradianceFromReflectionProbes(reflectVector, positionWS, perceptualRoughness);
//#else
//#ifdef _REFLECTION_PROBE_BOX_PROJECTION
//  reflectVector = BoxProjectedCubemapDirection(reflectVector, positionWS, unity_SpecCube0_ProbePosition, unity_SpecCube0_BoxMin, unity_SpecCube0_BoxMax);
//#endif // _REFLECTION_PROBE_BOX_PROJECTION
//
//  //half mip = PerceptualRoughnessToMipmapLevel(perceptualRoughness);
//  //half4 encodedIrradiance = half4(SAMPLE_TEXTURECUBE_LOD(unity_SpecCube0, samplerunity_SpecCube0, reflectVector, mip));
//
//  //irradiance = DecodeHDREnvironment(encodedIrradiance, unity_SpecCube0_HDR);
//
//  irradiance = float3(1.0f, 1.0f, 1.0f);
//#endif // _REFLECTION_PROBE_BLENDING
//  return irradiance * occlusion;
//#else
//  return _GlossyEnvironmentColor.rgb * occlusion;
//#endif // _ENVIRONMENTREFLECTIONS_OFF
//}
//
//half3 EnvironmentBRDFSpecular(BRDFData brdfData, half fresnelTerm)
//{
//  float surfaceReduction = 1.0 / (brdfData.roughness2 + 1.0);
//  return half3(surfaceReduction * lerp(brdfData.specular, brdfData.grazingTerm, fresnelTerm));
//}
//
//half3 EnvironmentBRDF(BRDFData brdfData, half3 indirectDiffuse, half3 indirectSpecular, half fresnelTerm)
//{
//  half3 c = indirectDiffuse * brdfData.diffuse;
//  c += indirectSpecular * EnvironmentBRDFSpecular(brdfData, fresnelTerm);
//  return c;
//}
//
//half DirectBRDFSpecular(BRDFData brdfData, half3 normalWS, half3 lightDirectionWS, half3 viewDirectionWS)
//{
//  float3 lightDirectionWSFloat3 = float3(lightDirectionWS);
//  float3 halfDir = SafeNormalize(lightDirectionWSFloat3 + float3(viewDirectionWS));
//
//  float NoH = saturate(dot(float3(normalWS), halfDir));
//  half LoH = half(saturate(dot(lightDirectionWSFloat3, halfDir)));
//
//  // GGX Distribution multiplied by combined approximation of Visibility and Fresnel
//  // BRDFspec = (D * V * F) / 4.0
//  // D = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2
//  // V * F = 1.0 / ( LoH^2 * (roughness + 0.5) )
//  // See "Optimizing PBR for Mobile" from Siggraph 2015 moving mobile graphics course
//  // https://community.arm.com/events/1155
//
//  // Final BRDFspec = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2 * (LoH^2 * (roughness + 0.5) * 4.0)
//  // We further optimize a few light invariant terms
//  // brdfData.normalizationTerm = (roughness + 0.5) * 4.0 rewritten as roughness * 4.0 + 2.0 to a fit a MAD.
//  float d = NoH * NoH * brdfData.roughness2MinusOne + 1.00001f;
//
//  half LoH2 = LoH * LoH;
//  half specularTerm = brdfData.roughness2 / ((d * d) * max(0.1h, LoH2) * brdfData.normalizationTerm);
//
//  // On platforms where half actually means something, the denominator has a risk of overflow
//  // clamp below was added specifically to "fix" that, but dx compiler (we convert bytecode to metal/gles)
//  // sees that specularTerm have only non-negative terms, so it skips max(0,..) in clamp (leaving only min(100,...))
//#if REAL_IS_HALF
//  specularTerm = specularTerm - HALF_MIN;
//  specularTerm = clamp(specularTerm, 0.0, 100.0); // Prevent FP16 overflow on mobiles
//#endif
//
//  return specularTerm;
//}
//
//half3 GlobalIllumination(BRDFData brdfData, BRDFData brdfDataClearCoat, float clearCoatMask,
//  half3 bakedGI, half occlusion, float3 positionWS,
//  half3 normalWS, half3 viewDirectionWS)
//{
//  half3 reflectVector = reflect(-viewDirectionWS, normalWS);
//  half NoV = saturate(dot(normalWS, viewDirectionWS));
//  half fresnelTerm = Pow4(1.0 - NoV);
//
//  half3 indirectDiffuse = bakedGI;
//  half3 indirectSpecular = GlossyEnvironmentReflection(reflectVector, positionWS, brdfData.perceptualRoughness, 1.0h);
//
//  half3 color = EnvironmentBRDF(brdfData, indirectDiffuse, indirectSpecular, fresnelTerm);
//
//  //if (IsOnlyAOLightingFeatureEnabled())
//  //{
//  //  color = half3(1, 1, 1); // "Base white" for AO debug lighting mode
//  //}
//
//#if defined(_CLEARCOAT) || defined(_CLEARCOATMAP)
//  half3 coatIndirectSpecular = GlossyEnvironmentReflection(reflectVector, positionWS, brdfDataClearCoat.perceptualRoughness, 1.0h);
//  // TODO: "grazing term" causes problems on full roughness
//  half3 coatColor = EnvironmentBRDFClearCoat(brdfDataClearCoat, clearCoatMask, coatIndirectSpecular, fresnelTerm);
//
//  // Blend with base layer using khronos glTF recommended way using NoV
//  // Smooth surface & "ambiguous" lighting
//  // NOTE: fresnelTerm (above) is pow4 instead of pow5, but should be ok as blend weight.
//  half4 kDielectricSpec = half4(0.04, 0.04, 0.04, 1.0 - 0.04);
//  half coatFresnel = kDielectricSpec.x + kDielectricSpec.a * fresnelTerm;
//  return (color * (1.0 - coatFresnel * clearCoatMask) + coatColor) * occlusion;
//#else
//  return color * occlusion;
//#endif
//}
//
//// InitilazeBRDF()
//
//float3 EvaluateBRDF(SurfaceData data, float3 lo, float3 wo)
//{
//  float3 result = float3(0.0, 0.0, 0.0);
//
//  const float oneMinusReflectivity = OneMinusReflectivityMetallic(data.metallic);
//  const float reflectivity = 1.0 - oneMinusReflectivity;
//
//  const float3 diffuse = albedo * oneMinusReflectivity;
//  const float3 specular = lerp(kDielectricSpec.rgb, albedo, metallic);
//
//  const float diffuseTerm = 1.0;
//  result += diffuse * diffuseTerm;
//
//  const float perceptualRoughness = PerceptualSmoothnessToPerceptualRoughness(data.smoothness);
//  const float roughness = max(PerceptualRoughnessToRoughness(perceptualRoughness), HALF_MIN_SQRT);
//  const float roughness2 = max(roughness * roughness, HALF_MIN);
//  //const float grazingTerm = saturate(smoothness + reflectivity);
//  const float normalizationTerm = roughness * 4.0 + 2.0;
//  const float roughness2MinusOne = roughness2 - 1.0;
//
//  const float NoL = wo.z;
//  //half3 radiance = lightColor * (lightAttenuation * NdotL);
//
//  //float3 lightDirectionWSFloat3 = float3(lightDirectionWS);
//  const float3 H = normalize(wo + wi);
//
//  const float NoH = H.z;
//  const float LoH = dot(wo, H);
//
//  // GGX Distribution multiplied by combined approximation of Visibility and Fresnel
//  // BRDFspec = (D * V * F) / 4.0
//  // D = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2
//  // V * F = 1.0 / ( LoH^2 * (roughness + 0.5) )
//  // See "Optimizing PBR for Mobile" from Siggraph 2015 moving mobile graphics course
//  // https://community.arm.com/events/1155
//
//  // Final BRDFspec = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2 * (LoH^2 * (roughness + 0.5) * 4.0)
//  // We further optimize a few light invariant terms
//  // brdfData.normalizationTerm = (roughness + 0.5) * 4.0 rewritten as roughness * 4.0 + 2.0 to a fit a MAD.
//  const float d = NoH * NoH * roughness2MinusOne + 1.00001f;
//
//  const float LoH2 = LoH * LoH;
//  const float specularTerm = roughness2 / ((d * d) * max(0.1h, LoH2) * normalizationTerm);
//
//  // On platforms where half actually means something, the denominator has a risk of overflow
//  // clamp below was added specifically to "fix" that, but dx compiler (we convert bytecode to metal/gles)
//  // sees that specularTerm have only non-negative terms, so it skips max(0,..) in clamp (leaving only min(100,...))
////#if REAL_IS_HALF
////  specularTerm = specularTerm - HALF_MIN;
////  specularTerm = clamp(specularTerm, 0.0, 100.0); // Prevent FP16 overflow on mobiles
////#endif
//
//  result += data.color * specularTerm;
//
//  return result;
//}
//

