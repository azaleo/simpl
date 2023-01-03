#pragma once

#include "simplay/platform/core.h"
#include "simplay/platform/vector.h"

#include "ray.h"
#include "vec3.h"

namespace sim {
  struct HitRecord {
    Point3 p;
    f64 t;
    Vec3 normal;
    bool front_face;

    HitRecord() : p(), t(0.0), normal(), front_face(false) {}

    void set_normal(const Ray& r, const Vec3& surface_normal) {
      front_face = (dot(r.dir, surface_normal) < 0);
      normal = front_face ? surface_normal : -surface_normal;
    }
  };
  
  struct Sphere {
    Point3 center;
    f64 radius;

    Sphere() : center(), radius(0.0) {}
    Sphere(const Point3& center, f64 radius) : center(center), radius(radius) {}
    
    bool hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr) const;
  };
  
  struct Hittable {
    enum Type {
      NONE,
      SPHERE,
      SCENE,
    };
    
    Type type;
    union {
      Sphere sphere;
      Vector<Hittable> scene;
    };
    
    Hittable() : type(NONE) {}
    
    static Hittable make_sphere(const Point3& center, f64 radius) {
      Hittable h;
      h.type = SPHERE;
      h.sphere = Sphere(center, radius);
      return h;
    }

    static Hittable make_scene() {
      Hittable h;
      h.type = SCENE;
      h.scene = Vector<Hittable>();
      return h;
    }
    
    bool hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr) const;
  };
}
