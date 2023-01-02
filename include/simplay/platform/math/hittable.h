#pragma once

#include "simplay/platform/core.h"

#include "ray.h"
#include "vec3.h"

namespace sim {
  struct HitRecord {
    f64 t = 0.0;
    Point3 p;
    Vec3 normal;
  };
  
  struct Sphere {
    Point3 center;
    f64 radius = 0.0;
    
    bool hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr);
  };
  
  struct Hittable {
    enum Type {
      NONE,
      SPHERE,
    };
    
    Type type = NONE;
    union {
      Sphere sphere;
    };
    
    Hittable() {}
    
    static Hittable make_sphere(const Point3& center, f64 radius) {
      Hittable h;
      h.type = SPHERE;
      h.sphere.center = center;
      h.sphere.radius = radius;
      return h;
    }
    
    bool hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr);
  };
}
