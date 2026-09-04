#pragma once
#include "common.h"

#include <algorithm>
#include <random>

class Starfield {
  private:
    struct Star {
      Vec3 direction;
      double magnitude, kelvin;
    };

    struct cbmap_idx {
      int f, u, v;
    };
    
    std::vector<Star> starfield; // you may need to sort and bucket stars into sectors for faster lookup
    std::vector<Star> sector[6][15][15];

    void random_direction(Star& star);
    void random_magnitude(Star& star);
    void random_temperature(Star& star);

    cbmap_idx get_sector(Vec3& v);

    std::vector<cbmap_idx> get_copies(Vec3& v);

    RGB get_color(const Star& s, double dist);

  public:
    Starfield();
    ~Starfield();

    /// @brief changes the input ray's color when called
    /// @param v the direction we're checking to see if it's pointing at a star
    void lookup_starfield(Ray& r);
};
