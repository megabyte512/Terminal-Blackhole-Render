#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/ioctl.h>
#include <unistd.h>

int TERM_WIDTH;
int TERM_HEIGHT;
double ASPECT_RATIO;
void read_terminal_size() {
  winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  TERM_WIDTH = w.ws_col;
  TERM_HEIGHT = w.ws_row;

  ASPECT_RATIO = double(TERM_WIDTH) / double(TERM_HEIGHT*2) * 0.91;
}

struct Vec3 {
  double x, y, z;

  Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
  Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
  Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }

  Vec3 cross(const Vec3& v) const { return {y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x}; }
  double dot(const Vec3& v) const { return x*v.x + y*v.y + z*v.z; }

  Vec3 normalize() const {
    double length = std::sqrt(dot(*this));
    if (length == 0) return {0, 0, 0};
    return {x/length, y/length, z/length};
  }
};

struct Star {
  double theta, phi;
  double magnitude;
  double kelvin;
};

std::vector<Star> starfield;
std::vector<unsigned int> sector_offset(2593, 0);

int sector_index(double theta, double phi) {
  int theta_idx = int((theta*(180.0/M_PI))/5.0);
  int phi_idx = int(phi*(180.0/M_PI)/5.0);
  return theta_idx * 72 + phi_idx;
}

bool check_star_collision(double theta, double phi) {
  for (const Star& s : starfield) {
    if (std::abs(theta - s.theta) < 0.01 && std::abs(phi - s.phi) < 0.01) {
      return true;
    }
  }
  return false;
}







int main() {
  // creating starfield
  for (int i = 0; i < 500; i++) {
    Star new_star;
    new_star.theta = fmod(static_cast<double>(std::rand()), M_PI);
    new_star.phi = fmod(static_cast<double>(std::rand()), 2.0*M_PI);
    new_star.magnitude = std::rand() % 30 - 10; // -10 to +20
    new_star.kelvin = std::rand() % 36500 + 3500; // 3500 to 40000
    starfield.push_back(new_star);
  }

  // sorting the stars in starfield by sector
  std::sort(starfield.begin(), starfield.end(), [](const Star& a, const Star& b) {
      return sector_index(a.theta, a.phi) < sector_index(b.theta, b.phi);
  });
  for (const Star& s : starfield) {
    sector_offset[sector_index(s.theta, s.phi) + 1]++;
  }
  for (int s = 1; s < sector_offset.size(); s++) {
    sector_offset[s] += sector_offset[s - 1];
  }

  double radius = 6.0;
  double theta = M_PI / 2.0;
  double phi = 0.0;

  std::cout << "\x1b[?25l";
  std::cout << "\x1b[2J\x1b[H";

  Vec3 camera_pos;
  camera_pos.x = radius * std::sin(theta) * std::cos(phi);
  camera_pos.y = radius * std::sin(theta) * std::sin(phi);
  camera_pos.z = radius * std::cos(theta);

  Vec3 origin = {0.0, 0.0, 0.0};
  Vec3 forward = (origin - camera_pos).normalize();

  Vec3 temp_up = {0.0, 0.0, 1.0};

  Vec3 right = forward.cross(temp_up);
  // right = right.normalize();
  Vec3 up = right.cross(forward);

  std::string frame_buffer = "";

  read_terminal_size();
  for (int y = 0; y < TERM_HEIGHT; y++) {
    for (int x = 0; x < TERM_WIDTH; x++) {
      double top_y = y * 2;
      double bottom_y = y * 2 + 1;

      double u = (double(x) / TERM_WIDTH * 2.0 - 1.0) * ASPECT_RATIO;
      double top_v = (double(top_y) / (TERM_HEIGHT*2.0) * 2.0 - 1.0);
      double bottom_v = (double(bottom_y) / (TERM_HEIGHT*2.0) * 2.0 - 1.0);

      Vec3 ray_direction_top = (forward + right * u + up * top_v).normalize();
      Vec3 ray_direction_bottom = (forward + right * u + up * bottom_v).normalize();

      Vec3 ray_position_top = camera_pos;
      Vec3 ray_position_bottom = camera_pos;


      int max_steps = 500;
      // top
      for (int step = 0; step < max_steps; step++) {
        ray_position_top = ray_position_top + ray_direction_top; // * stepsize when you get to it
        if (ray_position_top.dot(ray_position_top) < 4) { // black hole radius squared
          std::cout << "\x1b[38;2;0;0;0m";
          break;
        }
        else if (ray_position_top.dot(ray_position_top) > 200) {
          // ray_position_top = ray_position_top.normalize();
          double theta = std::acos(ray_direction_top.z);
          double phi = std::atan2(ray_direction_top.y, ray_direction_top.x);
          if (phi < 0) {
            phi += 2.0 * M_PI;
          }
          if (check_star_collision(theta, phi)) {
            std::cout << "\x1b[38;2;0;0;0m";
          } else {
            std::cout << "\x1b[38;2;255;255;255m";
          }
          break;
        }
      }
      // bottom
      for (int step = 0; step < max_steps; step++) {
        ray_position_bottom = ray_position_bottom + ray_direction_bottom; // * stepsize when you get to it
        if (ray_position_bottom.dot(ray_position_bottom) < 4) { // black hole radius squared
          std::cout << "\x1b[48;2;0;0;0m";
          break;
        }
        else if (ray_position_bottom.dot(ray_position_bottom) > 200) {
          // ray_position_bottom = ray_position_bottom.normalize();
          double theta = std::acos(ray_direction_bottom.z);
          double phi = std::atan2(ray_direction_bottom.y, ray_direction_bottom.x);
          if (phi < 0) {
            phi += 2.0 * M_PI;
          }
          if (check_star_collision(theta, phi)) {
            std::cout << "\x1b[48;2;0;0;0m";
          } else {
            std::cout << "\x1b[48;2;255;255;255m";
          }
          break;
        }
      }
      std::cout << "\u2580";
    }
    std::cout << '\n';
  }
  std::cout << frame_buffer;
  return 0;
}
