#pragma once

#include "common.h"
#include "random.h"
#include "ray.h"
#include "vec3.h"

namespace sim {
  struct Camera {
    Point3 origin;
    Point3 lower_left;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;
    f64 lens_radius;

    Camera(
        const Point3& lookfrom,
        const Point3& lookat,
        const Vec3& up,
        f64 fovy,
        f64 aspect_ratio,
        f64 aperture,
        f64 focus_dist) {
      f64 theta = radians(fovy);
      f64 h = tan(theta/2);
      f64 viewport_h = 2.0*h;
      f64 viewport_w = viewport_h * aspect_ratio;

      w = normalize(lookfrom - lookat);
      u = normalize(cross(up, w));
      v = cross(w, u);
      
      origin = lookfrom;
      horizontal = focus_dist * viewport_w * u;
      vertical = focus_dist * viewport_h * v;
      lower_left = origin - horizontal/2 - vertical/2 - focus_dist*w;
      
      lens_radius = aperture/2;
    }

    Ray cast_ray(f64 s, f64 t) const {
      Vec3 o = lens_radius * random_vec3_in_unit_disk();
      Vec3 offset = u*o.x + v*o.y;
      Vec3 dir = lower_left + s*horizontal + t*vertical - origin - offset;
      return Ray(origin + offset, dir);
    }
  };
}
