#pragma once

#include <math.h>
#include <stdlib.h>

#include "ray.h"
#include "vec3.h"

#undef min
#undef max

namespace sim {
  const f64 F64_INF = (f64)INFINITY;
  const f64 PI = 3.1415926535897932385;

  inline f64 radians(f64 degrees) {
    return degrees * PI / 180.0;
  }

  inline f64 random_f64() {
    return (f64)rand() / (RAND_MAX + 1);
  }

  inline f64 random_f64_in(f64 min, f64 max) {
    return min + (max-min)*random_f64();
  }

  inline Vec3 random_vec3() {
    f64 x = random_f64();
    f64 y = random_f64();
    f64 z = random_f64();
    return Vec3(x, y, z);
  }

  inline Vec3 random_vec3_in(f64 min, f64 max) {
    f64 x = random_f64_in(min, max);
    f64 y = random_f64_in(min, max);
    f64 z = random_f64_in(min, max);
    return Vec3(x, y, z);
  }

  inline Vec3 random_vec3_in_unit_sphere() {
    while (true) {
      Vec3 p = random_vec3_in(-1.0, 1.0);
      if (p.sqmag() < 1.0)
        return p;
    }
    // Unreachable.
    return Vec3(0.0, 0.0, 0.0);
  }

  inline Vec3 random_vec3_in_hemisphere(const Vec3& normal) {
    Vec3 in_sphere = random_vec3_in_unit_sphere();
    return (dot(in_sphere, normal) > 0.0) ? in_sphere : -in_sphere;
  }

  inline Vec3 random_dir() {
    return normalize(random_vec3_in_unit_sphere());
  }

  inline f64 min(f64 a, f64 b) {
    return a < b ? a : b;
  }

  inline f64 max(f64 a, f64 b) {
    return a > b ? a : b;
  }

  inline f64 clamp(f64 x, f64 a, f64 b) {
    return min(max(a, x), b);
  }
}
