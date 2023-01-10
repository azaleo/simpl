#include <stdio.h>
#include <stdlib.h>

#include <simplay/platform/camera.h>
#include <simplay/platform/common.h>
#include <simplay/platform/core.h>
#include <simplay/platform/hittable.h>
#include <simplay/platform/image.h>
#include <simplay/platform/material.h>
#include <simplay/platform/random.h>
#include <simplay/platform/ray.h>
#include <simplay/platform/vec3.h>

namespace sim {
  struct Scene {
    Hittable objects;

    Material ground_mat;
    Material big1_mat;
    Material big2_mat;
    Material big3_mat;
    Vector<Material> sphere_mats;

    void release() {
      objects.release();
      sphere_mats.release();
    }
  };

  const f64 ASPECT_RATIO = 3.0 / 2.0;
  const i32 IMG_W = 600;
  const i32 IMG_H = (i32)(IMG_W / ASPECT_RATIO);
  const i32 PIXEL_SAMPLES = 1;
  const i32 MAX_DEPTH = 2;

  void build_scene(Scene* world) {
    if (!world)
      return;

    world->release();
    world->objects = Hittable::make_scene();

    // Ground.
    world->ground_mat = Material::make_lambertian(Color3(0.5, 0.5, 0.5));
    world->objects.scene.push(Hittable::make_sphere(Point3(0.0, -1000.0, 0.0), 1000.0, &world->ground_mat));

    // Small spheres.
    // We need to pre-allocate to prevent material pointer invalidation.
    world->sphere_mats.reserve(22 * 22);
    for (i32 a = -11; a < 11; ++a) {
      for (i32 b = -11; b < 11; ++b) {
        Point3 center((f64)a + 0.9*random_f64(), 0.2, (f64)b + 0.9*random_f64());
        if ((center - Point3(4.0, 0.2, 0.0)).mag() > 0.9) {
          Material sphere_mat;

          f64 choose_mat = random_f64();
          if (choose_mat < 0.8) {
            Color3 albedo = random_vec3() * random_vec3();
            sphere_mat = Material::make_lambertian(albedo);
          } else if (choose_mat < 0.95) {
            Color3 albedo = random_vec3_in(0.5, 1.0);
            f64 fuzz = random_f64_in(0.0, 0.5);
            sphere_mat = Material::make_metal(albedo, fuzz);
          } else {
            sphere_mat = Material::make_dielectric(1.5);
          }

          world->sphere_mats.push(sphere_mat);
          world->objects.scene.push(Hittable::make_sphere(center, 0.2, &world->sphere_mats.back()));
        }
      }
    }

    // Big spheres.
    world->big1_mat = Material::make_dielectric(1.5);
    world->big2_mat = Material::make_lambertian(Color3(0.4, 0.2, 0.1));
    world->big3_mat = Material::make_metal(Color3(0.7, 0.6, 0.5), 0.0);
    world->objects.scene.push(Hittable::make_sphere(Point3(0.0, 1.0, 0.0), 1.0, &world->big1_mat));
    world->objects.scene.push(Hittable::make_sphere(Point3(-4.0, 1.0, 0.0), 1.0, &world->big2_mat));
    world->objects.scene.push(Hittable::make_sphere(Point3(4.0, 1.0, 0.0), 1.0, &world->big3_mat));
  }

  Camera make_camera() {
    Vec3 up(0.0, 1.0, 0.0);
    Vec3 lookfrom(13.0, 2.0, 3.0);
    Vec3 lookat(0.0, 0.0, 0.0);

    f64 focus_dist = 10.0;
    f64 aperture = 0.1;
    return Camera(lookfrom, lookat, up, 20.0, ASPECT_RATIO, aperture, focus_dist);
  }
  
  Color3 ray_color(const Ray& r, const Hittable& world, i32 depth) {
    if (depth <= 0)
      return Color3(0.0, 0.0, 0.0);
    
    HitRecord hr;
    if (!world.hit(r, 0.001, F64_INF, &hr)) {
      // If no hit, return a background sky gradient.
      Vec3 unit_dir = normalize(r.dir);
      f64 t = 0.5 * (unit_dir.y + 1.0);
      return (1.0-t)*Color3(1.0, 1.0, 1.0) + t*Color3(0.5, 0.7, 1.0);
    }

    Material* mat = hr.mat ? hr.mat : Material::get_default();
    Ray scattered;
    Color3 attenuation;
    if (!mat->scatter(r, hr, &attenuation, &scattered))
      return Color3(0.0, 0.0, 0.0);
    return attenuation * ray_color(scattered, world, depth-1);
  }
  
  void write_color3(FILE* out, const Color3& c) {
    // sqrt for gamma correction (gamma=2.0).
    i32 ir = (i32)(256.0 * clamp(sqrt(c.x), 0.0, 0.999));
    i32 ig = (i32)(256.0 * clamp(sqrt(c.y), 0.0, 0.999));
    i32 ib = (i32)(256.0 * clamp(sqrt(c.z), 0.0, 0.999));
    fprintf(out, "%d %d %d\n", ir, ig, ib);
  }

  FloatImage render(const Camera& cam, const Hittable& world) {
    FloatImage result;
    result.init(IMG_W, IMG_H);

    printf("P3\n%d %d\n255\n", IMG_W, IMG_H);
    for (i32 y = IMG_H-1; y > 0; --y) {
      fprintf(stderr, "\rRendering %.2f%%", (f64)(IMG_H-y) / (IMG_H-1) * 100.0);
      fflush(stderr);

      for (i32 x = 0; x < IMG_W; ++x) {
        Color3 pixel(0.0, 0.0, 0.0);
        for (i32 i = 0; i < PIXEL_SAMPLES; ++i) {
          f64 u = ((f64)x + random_f64()) / (IMG_W-1);
          f64 v = ((f64)y + random_f64()) / (IMG_H-1);
          pixel += ray_color(cam.cast_ray(u, v), world, MAX_DEPTH);
        }
        pixel /= PIXEL_SAMPLES;

        result.get(x, y) = pixel;
        write_color3(stdout, pixel);
      }
    }
    fprintf(stderr, "\n");
    return result;
  }
}

int main() {
  using namespace sim;

  Scene world;
  build_scene(&world);

  FloatImage result = render(make_camera(), world.objects);
  world.release();

  result.save_png_to("out/result.png");
  result.release();
}
