#pragma once

#include "simplay/platform/core.h"

#include "ray.h"
#include "vec3.h"

namespace sim {
  struct HitRecord {
    f64 t;
    Point3 p;
    Vec3 normal;

    HitRecord() : t(0.0), p(), normal() {}
  };
  
  struct Sphere {
    Point3 center;
    f64 radius;

    Sphere() : center(), radius(0.0) {}
    Sphere(const Point3& center, f64 radius) : center(center), radius(radius) {}
    
    bool hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr);
  };
  
  struct Hittable {
    enum Type {
      NONE,
      SPHERE,
    };
    
    Type type;
    union {
      Sphere sphere;
    };
    
    Hittable() : type(NONE) {}
    
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
