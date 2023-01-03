#include <stdio.h>

#include <simplay/platform/camera.h>
#include <simplay/platform/common.h>
#include <simplay/platform/core.h>
#include <simplay/platform/hittable.h>

namespace sim {
  const f64 ASPECT_RATIO = 16.0 / 9.0;
  const i32 IMG_W = 400;
  const i32 IMG_H = (i32)(IMG_W / ASPECT_RATIO);
  const i32 PIXEL_SAMPLES = 100;
  const i32 MAX_DEPTH = 10;
  
  Color3 ray_color(const Ray& r, const Hittable& world, i32 depth) {
    if (depth <= 0)
      return Color3(0.0, 0.0, 0.0);
    
    HitRecord hr;
    if (world.hit(r, 0.001, F64_INF, &hr)) {
      Point3 target = hr.p + random_vec3_in_hemisphere(hr.normal);
      return 0.5 * ray_color(Ray(hr.p, target - hr.p), world, depth-1);
    }
    
    Vec3 unit_dir = normalize(r.dir);
    f64 t = 0.5 * (unit_dir.y + 1.0);
    return (1.0-t)*Color3(1.0, 1.0, 1.0) + t*Color3(0.5, 0.7, 1.0);
  }
  
  void write_color3(FILE* out, const Color3& c) {
    // sqrt <=> gamma=2.0.
    i32 ir = (i32)(256.0 * clamp(sqrt(c.x), 0.0, 0.999));
    i32 ig = (i32)(256.0 * clamp(sqrt(c.y), 0.0, 0.999));
    i32 ib = (i32)(256.0 * clamp(sqrt(c.z), 0.0, 0.999));
    fprintf(out, "%d %d %d\n", ir, ig, ib);
  }

  void render(const Camera& camera, const Hittable& world) {
    printf("P3\n%d %d\n255\n", IMG_W, IMG_H);
    for (i32 y = IMG_H-1; y > 0; --y) {
      fprintf(stderr, "\rRendering %.2f%%", (f64)(IMG_H-y) / (IMG_H-1) * 100.0);
      fflush(stderr);
      for (i32 x = 0; x < IMG_W; ++x) {
        Color3 pixel(0.0, 0.0, 0.0);
        for (i32 i = 0; i < PIXEL_SAMPLES; ++i) {
          f64 u = ((f64)x + random_f64()) / (IMG_W-1);
          f64 v = ((f64)y + random_f64()) / (IMG_H-1);
          pixel += ray_color(camera.cast_ray(u, v), world, MAX_DEPTH);
        }
        pixel /= PIXEL_SAMPLES;
        write_color3(stdout, pixel);
      }
    }
    fprintf(stderr, "\n");
  }
}

int main() {
  using namespace sim;

  Camera camera;
  Hittable world = Hittable::make_scene();
  world.scene.push(Hittable::make_sphere(Point3(0.0, 0.0, -1.0), 0.5));
  world.scene.push(Hittable::make_sphere(Point3(0.0, -100.5, -1.0), 100.0));
  render(camera, world);
}
