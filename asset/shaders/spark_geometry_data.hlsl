#ifndef SPARK_GEOMETRY_INCLUDED
#define SPARK_GEOMETRY_INCLUDED

struct GeometryData
{
  float3 position_ws;
  float3 normal_ws;
  float4 tanghent_ws;
};

void InitializeGeometryData(inout GeometryData geometry_data, float3 position_ws, float3 normal_ws, float4 tangent_ws)
{
  geometry_data.position_ws = position_ws;
  geometry_data.normal_ws = normalize(normal_ws);
  geometry_data.tanghent_ws.xyz = normalize(tangent_ws.xyz);
  geometry_data.tanghent_ws.w = tangent_ws.w;
}
#endif