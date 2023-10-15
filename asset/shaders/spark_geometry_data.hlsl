#ifndef SPARK_GEOMETRY_INCLUDED
#define SPARK_GEOMETRY_INCLUDED

struct GeometryData
{
  float3 position_ws;
  float3 normal_ws;
};

void InitializeGeometryData(inout GeometryData geometry_data, SurfaceData surface_data, float3 position_ws, float3 normal_ws, float4 tangent_ws)
{
  geometry_data.position_ws = position_ws;

#ifdef USE_NORMAL_MAP
  float3 bitangent_ws = tangent_ws.w * cross(normalize(normal_ws), normalize(tangent_ws));
  geometry_data.normal_ws = normalize(surface_data.normal.x * tangent_ws + surface_data.normal.y * bitangent_ws + surface_data.normal.z * normal_ws);
#endif
}
#endif