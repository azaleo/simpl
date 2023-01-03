#include "simplay/platform/material.h"

#include "simplay/platform/common.h"

namespace sim {
  bool Lambertian::scatter(const Ray& in, const HitRecord& hr, Color3* attenuation, Ray* scattered) const {
    (void)in;
    if (scattered) {
      Vec3 scatter_dir = hr.normal + random_dir();
      *scattered = Ray(hr.p, scatter_dir.is_near_zero() ? hr.normal : scatter_dir);
    }
    if (attenuation)
      *attenuation = albedo;
    return true;
  }

  bool Metal::scatter(const Ray& in, const HitRecord& hr, Color3* attenuation, Ray* scattered) const {
    Vec3 reflected = reflect(normalize(in.dir), hr.normal) + fuzz*random_vec3_in_unit_sphere();
    if (scattered)
      *scattered = Ray(hr.p, reflected);
    if (attenuation)
      *attenuation = albedo;
    return (dot(reflected, hr.normal) > 0.0);
  }

  bool Material::scatter(const Ray& in, const HitRecord& hr, Color3* attenuation, Ray* scattered) const {
    switch (type) {
      case NONE:
      default:
        return false;
      case LAMBERTIAN:
        return lambertian.scatter(in, hr, attenuation, scattered);
      case METAL:
        return metal.scatter(in, hr, attenuation, scattered);
    }
  }
}
