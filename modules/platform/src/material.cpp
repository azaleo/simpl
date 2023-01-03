#include "simplay/platform/material.h"

#include "simplay/platform/common.h"
#include "simplay/platform/random.h"

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

  namespace {
    f64 schlick_reflectance(f64 cos, f64 ref_idx) {
      f64 r0 = (1.0-ref_idx) / (1.0+ref_idx);
      r0 = r0*r0;
      return r0 + (1.0-r0)*pow((1.0-cos), 5.0);
    }
  }

  bool Dielectric::scatter(const Ray& in, const HitRecord& hr, Color3* attenuation, Ray* scattered) const {
    if (scattered) {
      f64 idx_ratio = hr.front_face ? (1.0/ior) : ior;
      
      Vec3 ray_dir = normalize(in.dir);
      f64 cos_theta = min(dot(-ray_dir, hr.normal), 1.0);
      f64 sin_theta = sqrt(1.0 - cos_theta*cos_theta);

      // Total internal reflection.
      bool should_reflect = (idx_ratio*sin_theta > 1.0);
      if (should_reflect || (schlick_reflectance(cos_theta, idx_ratio) > random_f64()))
        *scattered = Ray(hr.p, reflect(ray_dir, hr.normal));
      else
        *scattered = Ray(hr.p, refract(ray_dir, hr.normal, idx_ratio));
    }
    if (attenuation)
      *attenuation = Color3(1.0, 1.0, 1.0);
    return true;
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
      case DIELECTRIC:
        return dielectric.scatter(in, hr, attenuation, scattered);
    }
  }
}
