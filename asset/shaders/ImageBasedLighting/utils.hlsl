#ifndef RAYGENE3D_IBL_UTILS
#define RAYGENE3D_IBL_UTILS

  float FastSign(float x)
  {
    return saturate(x * FLT_MAX + 0.5) * 2.0 - 1.0;
  }
  // Generates an orthonormal (row-major) basis from a unit vector. TODO: make it column-major.
  // The resulting rotation matrix has the determinant of +1.
  // Ref: 'ortho_basis_pixar_r2' from http://marc-b-reynolds.github.io/quaternions/2016/07/06/Orthonormal.html
  real3x3 GetLocalFrame(real3 localZ)
  {
      real x  = localZ.x;
      real y  = localZ.y;
      real z  = localZ.z;
      real sz = FastSign(z);
      real a  = 1 / (sz + z);
      real ya = y * a;
      real b  = x * ya;
      real c  = x * sz;

      real3 localX = real3(c * x * a - 1, sz * b, c);
      real3 localY = real3(b, y * ya - sz, y);

      // Note: due to the quaternion formulation, the generated frame is rotated by 180 degrees,
      // s.t. if localZ = {0, 0, 1}, then localX = {-1, 0, 0} and localY = {0, -1, 0}.
      return real3x3(localX, localY, localZ);
  }

  // Assumes that (0 <= x <= Pi).
  real SinFromCos(real cosX)
  {
      return sqrt(saturate(1 - cosX * cosX));
  }

  // Transforms the unit vector from the spherical to the Cartesian (right-handed, Z up) coordinate.
  real3 SphericalToCartesian(real cosPhi, real sinPhi, real cosTheta)
  {
      real sinTheta = SinFromCos(cosTheta);

      return real3(real2(cosPhi, sinPhi) * sinTheta, cosTheta);
  }

  real3 SphericalToCartesian(real phi, real cosTheta)
  {
      real sinPhi, cosPhi;
      sincos(phi, sinPhi, cosPhi);

      return SphericalToCartesian(cosPhi, sinPhi, cosTheta);
  }

  // Division which returns 1 for (inf/inf) and (0/0).
  // If any of the input parameters are NaNs, the result is a NaN.
  real SafeDiv(real numer, real denom)
  {
      return (numer != denom) ? numer / denom : 1;
  }

  real Sq(real x)
  {
    return x * x;
  }

#endif // RAYGENE3D_IBL_UTILS