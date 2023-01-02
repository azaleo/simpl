#pragma once

#include <math.h>

#include "simplay/platform/core.h"

namespace sim {
  struct vec3 {
    f64 e[3] = {};
    
    static vec3 zero() { return {}; }
    static vec3 make(f64 e0, f64 e1, f64 e2) {
      vec3 v;
      v.e[0] = e0;
      v.e[1] = e1;
      v.e[2] = e2;
      return v;
    }
    
    f64 x() const { return e[0]; }
    f64 y() const { return e[1]; }
    f64 z() const { return e[2]; }
    
    f64& operator[](usize i) { return e[i]; }
    const f64& operator[](usize i) const { return e[i]; }
    
    vec3 operator-() const {
      return {-x(), -y(), -z()};
    }
    
    vec3& operator+=(const vec3& v) {
      e[0] += v[0];
      e[1] += v[1];
      e[2] += v[2];
      return *this;
    }
    
    vec3& operator-=(const vec3& v) {
      e[0] -= v[0];
      e[1] -= v[1];
      e[2] -= v[2];
      return *this;
    }
    
    vec3& operator*=(f64 t) {
      e[0] *= t;
      e[1] *= t;
      e[2] *= t;
      return *this;
    }
    
    vec3& operator/=(f64 t) {
      return (*this *= 1.0/t);
    }
    
    f64 mag() const {
      return ::sqrt(sqmag());
    }
    
    f64 sqmag() const {
      return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }
  };
  
  typedef vec3 point3;
  typedef vec3 rgb;
  
  inline vec3 operator+(const vec3& a, const vec3& b) {
    f64 x = a.x() + b.x();
    f64 y = a.y() + b.y();
    f64 z = a.z() + b.z();
    return {x, y, z};
  }
  
  inline vec3 operator-(const vec3& a, const vec3& b) {
    f64 x = a.x() - b.x();
    f64 y = a.y() - b.y();
    f64 z = a.z() - b.z();
    return {x, y, z};
  }
  
  inline vec3 operator*(const vec3& v, f64 t) {
    f64 x = v.x() * t;
    f64 y = v.y() * t;
    f64 z = v.z() * t;
    return {x, y, z};
  }
  
  inline vec3 operator*(f64 t, const vec3& v) {
    return v * t;
  }
  
  inline vec3 operator/(const vec3& v, f64 t) {
    return v * (1.0/t);
  }
  
  inline f64 dot(const vec3& a, const vec3& b) {
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
  }
  
  inline vec3 cross(const vec3& a, const vec3& b) {
    f64 x = a.y()*b.z() - a.z()*b.y();
    f64 y = a.z()*b.x() - a.x()*b.z();
    f64 z = a.x()*b.y() - a.y()*b.x();
    return {x, y, z};
  }
  
  inline vec3 normalize(const vec3& v) {
    return v / v.mag();
  }
}
