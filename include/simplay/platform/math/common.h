#pragma once

#include <math.h>

#include "ray.h"
#include "vec3.h"

namespace sim {
  const f64 F64_INF = (f64)INFINITY;
  const f64 PI = 3.1415926535897932385;

  inline f64 radians(f64 degrees) {
    return degrees * PI / 180.0;
  }
}
