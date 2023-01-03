#include <stdio.h>

#include <simplay/platform/core.h>
#include <simplay/platform/math/common.h>
#include <simplay/platform/math/hittable.h>

namespace sim {
  const f64 ASPECT_RATIO = 16.0 / 9.0;
  const i32 IMG_W = 400;
  const i32 IMG_H = (i32)(IMG_W / ASPECT_RATIO);
  
  Color3 ray_color(const Ray& r, const Hittable& scene) {
    HitRecord hr;
    if (scene.hit(r, 0.0, F64_INF, &hr))
      return 0.5 * (hr.normal + Vec3(1.0, 1.0, 1.0));
    
    Vec3 unit_dir = normalize(r.dir);
    f64 t = 0.5 * (unit_dir.y + 1.0);
    return (1.0-t)*Color3(1.0, 1.0, 1.0) + t*Color3(0.5, 0.7, 1.0);
  }
  
  void write_color3(FILE* out, const Color3& c) {
    i32 ir = (i32)(255.999 * c.x);
    i32 ig = (i32)(255.999 * c.y);
    i32 ib = (i32)(255.999 * c.z);
    fprintf(out, "%d %d %d\n", ir, ig, ib);
  }
}

int main() {
  using namespace sim;

  // Scene
  Hittable world = Hittable::make_scene();
  world.scene.push(Hittable::make_sphere(Point3(0.0, 0.0, -1.0), 0.5));
  world.scene.push(Hittable::make_sphere(Point3(0.0, -100.5, -1.0), 100.0));
  
  // Calculate viewport dims
  f64 viewport_h = 2.0;
  f64 viewport_w = viewport_h * ASPECT_RATIO;
  f64 focal_length = 1.0;
  
  Point3 origin;
  Vec3 horizontal(viewport_w, 0.0, 0.0);
  Vec3 vertical(0.0, viewport_h, 0.0);
  Point3 lower_left = origin - horizontal/2 - vertical/2 - Vec3(0.0, 0.0, focal_length);
  
  // Render
  printf("P3\n%d %d\n255\n", IMG_W, IMG_H);
  for (int y = IMG_H-1; y > 0; --y) {
    fprintf(stderr, "\rRendering %.2f%%", (f64)(IMG_H-y) / (IMG_H-1) * 100.0);
    fflush(stderr);
    
    for (int x = 0; x < IMG_W; ++x) {
      f64 u = (f64)x / (IMG_W-1);
      f64 v = (f64)y / (IMG_H-1);
      
      Ray r(origin, lower_left + u*horizontal + v*vertical - origin);
      write_color3(stdout, ray_color(r, world));
    }
  }
  fprintf(stderr, "\n");
}
