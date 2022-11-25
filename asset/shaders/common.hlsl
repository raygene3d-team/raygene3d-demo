
float3x3 InverseTBN(float3x3 tbn)
{
  float3 row0 = tbn[0];
  float3 row1 = tbn[1];
  float3 row2 = tbn[2];

  float3 col0 = cross(row1, row2);
  float3 col1 = cross(row2, row0);
  float3 col2 = cross(row0, row1);

  // Assume determinant is not 0
  float determinant = dot(row0, col0);

  return float3x3(col0, col1, col2) / determinant;
}