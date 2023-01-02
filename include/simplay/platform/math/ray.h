#pragma once

#include "simplay/platform/core.h"

#include "vec3.h"

namespace sim {
  struct Ray {
    Point3 origin;
    Vec3 dir;
    
    static Ray make(const Point3& origin, const Vec3& dir) {
      Ray r;
      r.origin = origin;
      r.dir = dir;
      return r;
    }
    
    Point3 at(f64 t) const {
      return origin + t*dir;
    }
  };
}
