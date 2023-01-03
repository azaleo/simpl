#pragma once

#include <math.h>

#include "simplay/platform/core.h"

namespace sim {
  struct Vec3 {
    f64 x;
    f64 y;
    f64 z;
    
    Vec3() : x(0.0), y(0.0), z(0.0) {}
    Vec3(f64 x, f64 y, f64 z) : x(x), y(y), z(z) {}
    
    f64& operator[](usize i) { return (&x)[i]; }
    const f64& operator[](usize i) const { return (&x)[i]; }
    
    Vec3 operator-() const {
      return Vec3(-x, -y, -z);
    }
    
    Vec3& operator+=(const Vec3& v) {
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    }
    
    Vec3& operator-=(const Vec3& v) {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
    }
    
    Vec3& operator*=(f64 t) {
      x *= t;
      y *= t;
      z *= t;
      return *this;
    }
    
    Vec3& operator/=(f64 t) {
      return (*this *= 1.0/t);
    }
    
    f64 mag() const {
      return sqrt(sqmag());
    }
    
    f64 sqmag() const {
      return x*x + y*y + z*z;
    }

    bool is_near_zero() const {
      const f64 epsilon = 1e-8;
      return (fabs(x) < epsilon) && (fabs(y) < epsilon) && (fabs(y) < epsilon);
    }
  };
  
  typedef Vec3 Point3;
  typedef Vec3 Color3;
  
  inline Vec3 operator+(const Vec3& a, const Vec3& b) {
    f64 x = a.x + b.x;
    f64 y = a.y + b.y;
    f64 z = a.z + b.z;
    return Vec3(x, y, z);
  }
  
  inline Vec3 operator-(const Vec3& a, const Vec3& b) {
    f64 x = a.x - b.x;
    f64 y = a.y - b.y;
    f64 z = a.z - b.z;
    return Vec3(x, y, z);
  }

  inline Vec3 operator*(const Vec3& a, const Vec3& b) {
    f64 x = a.x * b.x;
    f64 y = a.y * b.y;
    f64 z = a.z * b.z;
    return Vec3(x, y, z);
  }
  
  inline Vec3 operator*(const Vec3& v, f64 t) {
    f64 x = v.x * t;
    f64 y = v.y * t;
    f64 z = v.z * t;
    return Vec3(x, y, z);
  }
  
  inline Vec3 operator*(f64 t, const Vec3& v) {
    return v * t;
  }
  
  inline Vec3 operator/(const Vec3& v, f64 t) {
    return v * (1.0/t);
  }
  
  inline f64 dot(const Vec3& a, const Vec3& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
  }
  
  inline Vec3 cross(const Vec3& a, const Vec3& b) {
    f64 x = a.y*b.z - a.z*b.y;
    f64 y = a.z*b.x - a.x*b.z;
    f64 z = a.x*b.y - a.y*b.x;
    return Vec3(x, y, z);
  }
  
  inline Vec3 normalize(const Vec3& v) {
    return v / v.mag();
  }

  inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2.0*dot(v, n)*n;
  }
}
