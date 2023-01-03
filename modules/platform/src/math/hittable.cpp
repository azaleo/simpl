#include "simplay/platform/math/hittable.h"

namespace sim {
  namespace {
    bool hit_scene(const Vector<Hittable>& scene, const Ray& r, f64 tmin, f64 tmax, HitRecord* hr) {
      bool hit_anything = false;
      f64 closest = tmax;
      for (usize i = 0; i < scene.length; ++i) {
        HitRecord rec;
        if (scene[i].hit(r, tmin, closest, &rec)) {
          hit_anything = true;
          closest = rec.t;
          *hr = rec;
        }
      }
      return hit_anything;
    }
  }

  bool Hittable::hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr) const {
    switch (type) {
      case NONE:
      default:
        return false;
      case SPHERE:
        return sphere.hit(r, tmin, tmax, hr);
      case SCENE:
        return hit_scene(scene, r, tmin, tmax, hr);
    }
  }
  
  bool Sphere::hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr) const {
    Vec3 oc = r.origin - center;
    f64 a = dot(r.dir, r.dir);
    f64 half_b = dot(r.dir, oc);
    f64 c = oc.sqmag() - radius*radius;
    
    f64 delta = half_b*half_b - a*c;
    if (delta < 0.0)
      return false;
    
    // Find root within [tmin, tmax]
    f64 sqrtd = ::sqrt(delta);
    f64 root = (-half_b - sqrtd) / a;
    if (root < tmin || tmax < root) {
      root = (-half_b + sqrtd) / a;
      if (root < tmin || tmax < root)
        return false;
    }
    
    if (hr) {
      hr->t = root;
      hr->p = r.at(hr->t);
      hr->set_normal(r, (hr->p - center) / radius);
    }
    return true;
  }
}
