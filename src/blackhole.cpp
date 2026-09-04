#include "headers/blackhole.h"

Blackhole::Blackhole() {}
Blackhole::Blackhole(double mass, double radius_s, double α) {
  this->mass = mass;
  this->radius_s = radius_s;
  this->α = α;
}
Blackhole::~Blackhole() {}

bool Blackhole::calculate(Ray& r) {
  while (r.λ < 500) {
    r.λ++;
    r.position = r.position + r.direction;
    if (r.position.dot(r.position) < radius_s*radius_s) {
      return true;
    }
  }
  return false;
}
