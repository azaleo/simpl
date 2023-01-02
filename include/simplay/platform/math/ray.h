#pragma once

#include "vec3.h"
#include "simplay/platform/core.h"

namespace sim {
  struct Ray {
    point3 origin;
    vec3 dir;
    
    static Ray make(const point3& origin, const vec3& dir) {
      Ray r;
      r.origin = origin;
      r.dir = dir;
      return r;
    }
    
    point3 at(f64 t) {
      return origin + t*dir;
    }
  };
}
