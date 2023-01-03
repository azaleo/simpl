#pragma once

#include "core.h"
#include "vec3.h"

namespace sim {
  struct Ray {
    Point3 origin;
    Vec3 dir;
    
    Ray() : origin(), dir() {}
    Ray(const Point3& origin, const Vec3& dir) : origin(origin), dir(dir) {}
    
    Point3 at(f64 t) const {
      return origin + t*dir;
    }
  };
}
