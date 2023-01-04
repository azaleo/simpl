#pragma once

#include "common.h"
#include "ray.h"
#include "vec3.h"

namespace sim {
  struct Camera {
    Point3 origin;
    Point3 lower_left;
    Vec3 horizontal;
    Vec3 vertical;

    Camera(const Point3& lookfrom, const Point3& lookat, const Vec3& up, f64 fovy, f64 aspect_ratio) {
      f64 theta = radians(fovy);
      f64 h = tan(theta/2);
      f64 viewport_h = 2.0*h;
      f64 viewport_w = viewport_h * aspect_ratio;

      Vec3 w = normalize(lookfrom - lookat);
      Vec3 u = normalize(cross(up, w));
      Vec3 v = cross(w, u);
      
      origin = lookfrom;
      horizontal = viewport_w * u;
      vertical = viewport_h * v;
      lower_left = origin - horizontal/2 - vertical/2 - w;
    }

    Ray cast_ray(f64 s, f64 t) const {
      return Ray(origin, lower_left + s*horizontal + t*vertical - origin);
    }
  };
}
