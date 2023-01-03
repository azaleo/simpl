#pragma once

#include "core.h"
#include "ray.h"
#include "vec3.h"
#include "vector.h"

namespace sim {
  struct Material;

  struct HitRecord {
    Point3 p;
    f64 t;
    Vec3 normal;
    bool front_face;
    Material* mat;

    HitRecord() : p(), t(0.0), normal(), front_face(false), mat(nullptr) {}

    void set_normal(const Ray& r, const Vec3& surface_normal) {
      front_face = (dot(r.dir, surface_normal) < 0);
      normal = front_face ? surface_normal : -surface_normal;
    }
  };
  
  struct Sphere {
    Point3 center;
    f64 radius;
    Material* mat;

    Sphere() : center(), radius(0.0), mat(nullptr) {}
    Sphere(const Point3& center, f64 radius, Material* mat = nullptr)
      : center(center), radius(radius), mat(mat) {}
    
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
    
    static Hittable make_sphere(const Point3& center, f64 radius, Material* mat = nullptr) {
      Hittable h;
      h.type = SPHERE;
      h.sphere = Sphere(center, radius, mat);
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
