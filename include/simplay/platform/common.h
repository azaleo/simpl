#pragma once

#include <math.h>

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
