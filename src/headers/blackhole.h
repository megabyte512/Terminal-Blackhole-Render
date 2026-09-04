#pragma once
#include "common.h"

class Blackhole {
  private:
    double mass;
    double radius_s;
    double α;

  public:
    Blackhole();
    Blackhole(double mass, double radius_s, double α);
    ~Blackhole();

    /// @brief bends the rays
    /// @returns true/false whether collided with black hole or not
    bool calculate(Ray& r);
};
