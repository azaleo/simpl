#pragma once

#include "hittable.h"
#include "ray.h"
#include "vec3.h"

namespace sim {
  struct Lambertian {
    Color3 albedo;

    explicit Lambertian(const Color3& albedo) : albedo(albedo) {}

    bool scatter(const Ray& in, const HitRecord& hr, Color3* attenuation, Ray* scattered) const;
  };

  struct Metal {
    Color3 albedo;
    f64 fuzz;

    explicit Metal(const Color3& albedo, f64 fuzz)
      : albedo(albedo), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

    bool scatter(const Ray& in, const HitRecord& hr, Color3* attenuation, Ray* scattered) const;
  };

  struct Material {
    enum Type {
      NONE,
      LAMBERTIAN,
      METAL,
    };

    Type type;
    union {
      Lambertian lambertian;
      Metal metal;
    };

    Material() : type(NONE) {}

    static Material* get_default() {
      static Material default_mat = make_lambertian(Color3(0.5, 0.5, 0.5));
      return &default_mat;
    }

    static Material make_lambertian(const Color3& albedo) {
      Material m;
      m.type = LAMBERTIAN;
      m.lambertian = Lambertian(albedo);
      return m;
    }

    static Material make_metal(const Color3& albedo, f64 fuzz) {
      Material m;
      m.type = METAL;
      m.metal = Metal(albedo, fuzz);
      return m;
    }

    bool scatter(const Ray& in, const HitRecord& hr, Color3* attenuation, Ray* scattered) const;
  };
}
