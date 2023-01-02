#include "simplay/platform/math/hittable.h"

namespace sim {
  bool Hittable::hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr) {
    switch (type) {
      case NONE:
      default:
        return false;
      case SPHERE:
        return sphere.hit(r, tmin, tmax, hr);
    }
  }
  
  bool Sphere::hit(const Ray& r, f64 tmin, f64 tmax, HitRecord* hr) {
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
    
    hr->t = root;
    hr->p = r.at(hr->t);
    hr->normal = (hr->p - center) / radius;
    return true;
  }
}
