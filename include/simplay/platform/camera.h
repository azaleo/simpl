#pragma once

#include "ray.h"
#include "vec3.h"

namespace sim {
  struct Camera {
    Point3 origin;
    Point3 lower_left;
    Vec3 horizontal;
    Vec3 vertical;

    Camera() {
      f64 aspect_ratio = 16.0 / 9.0;
      f64 viewport_h = 2.0;
      f64 viewport_w = viewport_h * aspect_ratio;
      f64 focal_length = 1.0;
      
      origin = Vec3(0.0, 0.0, 0.0);
      horizontal = Vec3(viewport_w, 0.0, 0.0);
      vertical = Vec3(0.0, viewport_h, 0.0);
      lower_left = origin - horizontal/2 - vertical/2 - Vec3(0.0, 0.0, focal_length);
    }

    Ray cast_ray(f64 u, f64 v) const {
      return Ray(origin, lower_left + u*horizontal + v*vertical - origin);
    }
  };
}
