#ifndef SPARK_GEOMETRY_INCLUDED
#define SPARK_GEOMETRY_INCLUDED

struct GeometryData
{
  float3 position_ws;
  float3x3 tbn;
};

GeometryData InitializeGeometryData(float3 position_ws, float3 normal_ws, float4 tangent_ws)
{
  GeometryData geometry_data = (GeometryData)0;

  geometry_data.position_ws = position_ws;

  const float3 n = normalize(normal_ws);
  const float3 t = normalize(tangent_ws.xyz);
  const float3 b = tangent_ws.w * cross(n, t);

  geometry_data.tbn = float3x3(t, b, n);

  return geometry_data;
}
#endif