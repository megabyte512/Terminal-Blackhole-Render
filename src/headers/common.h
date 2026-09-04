#pragma once
#include <cmath>
#include <vector>

struct Vec3 {
  double x, y, z;

  Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
  Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
  Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }

  Vec3 cross(const Vec3& o) const { return {y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x}; }
  double dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }

  Vec3 normalize() const {
    double length = std::sqrt(dot(*this));
    if (length == 0) return {0, 0, 0};
    return {x/length, y/length, z/length};
  }
};

struct RGB {
  unsigned char R, G, B;
};

struct Ray {
  Vec3 position;
  Vec3 direction;
  double λ;
  RGB pixel_color;
};

