#include "headers/starfield.h"

void Starfield::random_direction(Star& star) {
  static std::mt19937_64 rng{ std::random_device{}() };
  static std::normal_distribution<double> gauss(0.0, 1.0);

  star.direction.x = gauss(rng);
  star.direction.y = gauss(rng);
  star.direction.z = gauss(rng);
  star.direction.normalize();
}

void Starfield::random_magnitude(Star& star) {
  static std::mt19937_64 rng{ std::random_device{}() };
  std::uniform_real_distribution<double> u(0.0, 1.0);

  double p = u(rng);
  double a = std::pow(10.0, 0.6 * -3.0);
  double b = std::pow(10.0, 0.6 * 9.0);
  double val = a + p * (b - a);
  star.magnitude = std::log10(val) / 0.6;
}

void Starfield::random_temperature(Star& star) {
  static std::mt19937_64 rng{ std::random_device{}() };
  static const double weights[] = { 0.76, 0.12, 0.076, 0.03, 0.006, 0.0013, 0.00003 };
  static const double temp_min[] = { 24, 37, 52, 60, 75, 100, 300 };  // x100 ... these are scaled down
  static const double temp_max[] = { 37, 52, 60, 75, 100, 300, 500 }; // so I don't have to divide by 100 later

  static std::discrete_distribution<int> class_dist(std::begin(weights), std::end(weights));
  int cls = class_dist(rng);

  std::uniform_real_distribution<double> temp_dist(temp_min[cls], temp_max[cls]);
  star.kelvin = temp_dist(rng);
}

Starfield::cbmap_idx Starfield::get_sector(Vec3& o) {
  double x = std::abs(o.x);
  double y = std::abs(o.y);
  double z = std::abs(o.z);
  double max;
  double u;
  double v;
  int f;

  if (x > y && x > z) {
    max = x; 
    if (o.x > 0) { f = 0; }
    else { f = 1; }
    u = o.y;
    v = o.z;
  } else if (y > x && y > z) {
    max = y;
    if (o.y > 0) { f = 2; }
    else { f = 3; }
    u = o.x;
    v = o.z;
  } else {
    max = z;
    if (o.z > 0) { f = 4; }
    else { f = 5; }
    u = o.x;
    v = o.y;
  }

  u /= max; // normalizing to cube face reference
  v /= max;
  u += 1.0; // shifting from [-1, 1] to [0, 2]
  v += 1.0;
  u *= 0.5; // scaling to values between [0, 1]
  v *= 0.5;
  u *= 15.0; // goes from continuous 0-1 to discrete idx 0-14
  v *= 15.0;

  return {f, static_cast<int>(u), static_cast<int>(v)};
}

std::vector<Starfield::cbmap_idx> Starfield::get_copies(Vec3& o) {
  return {};
}

RGB Starfield::get_color(const Star& s, double d) {
  double r, g, b;
  // Tanner Helland's algorithm
  if (s.kelvin <= 66.0) {
    r = 255.0;
  } else {
    r = s.kelvin - 60.0;
    r = 329.698727446 * std::pow(r, -0.1332047592);
    r = std::clamp(r, 0.0, 255.0);
  }
  if (s.kelvin <= 66.0) {
    g = s.kelvin;
    g = 99.4708025861 * std::log(g) - 161.1195681661;
  } else {
    g = s.kelvin - 60.0;
    g = 288.1221695283 * std::pow(g, -0.0755148492);
  }
  g = std::clamp(g, 0.0, 255.0);
  if (s.kelvin >= 66.0) {
    b = 255.0;
  } else if (s.kelvin <= 19.0) {
    b = 0.0;
  } else {
    b = s.kelvin - 10.0;
    b = 138.5177312231 * std::log(b) - 305.0447927307;
    b = std::clamp(b, 0.0, 255.0);
  }

  return {static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b)};
}


Starfield::Starfield() {
  for (int i = 0; i < 2000; i++) {
    Star star;

    random_direction(star);
    random_magnitude(star);
    random_temperature(star);

    cbmap_idx sector_idx = get_sector(star.direction);    // gets sector of star
    sector[sector_idx.f][sector_idx.u][sector_idx.v].push_back(star);     // stores star in its respective sector
    std::vector<cbmap_idx> sector_idxes;
    sector_idxes = get_copies(star.direction);             // gets a vector of sectors a star is close enough to to be copied to. usually one if any
    for (int i = 0; i < sector_idxes.size(); i++) {        // for every sector the star needs to be copied into, copy it
      sector[sector_idxes[i].f][sector_idxes[i].u][sector_idxes[i].v].push_back(star);
    }
    starfield.push_back(star);
  }
}
Starfield::~Starfield() {}

void Starfield::lookup_starfield(Ray& r) {
  // cbmap_idx si = get_sector(r.direction); // fast lookup of starfield by only checking stars in the vicinity
  // for (const Star& s : sector[si.f][si.u][si.v]) { // iterating over stars in sector to see if we hit any
  //   Vec3 dist = s.direction - r.direction;
  //   if (dist.dot(dist) < 0.005 /* some equation of magnitude */) { // if within star range
  //     r.pixel_color = get_color(s, dist.dot(dist));
  //     break;
  //   }
  // }
  for (Star& s : starfield) {
    Vec3 dist = s.direction - r.direction;
    if (dist.dot(dist) < 0.005 /* some equation of magnitude */) { // if within star range
      r.pixel_color = get_color(s, dist.dot(dist));
      break;
    }
  }
}
