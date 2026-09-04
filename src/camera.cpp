#include "headers/camera.h"
#include <string>


void Camera::init_rays() {
  // this will need to change when we're looping
  Vec3 temp_up = {0.0, 0.0, 1.0};
  Vec3 right = look_dir.cross(temp_up);
  Vec3 up = right.cross(look_dir);
  
  // initializing rays coming from camera
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Ray upper_ray;
      Ray lower_ray;

      double upper_y = y * 2.0;
      double lower_y = y * 2.0 + 1.0;

      double u = (double(x) / width * 2.0 - 1.0) * aspect_ratio;
      double upper_v = upper_y / (height*2.0) * 2.0 - 1.0;
      double lower_v = lower_y / (height*2.0) * 2.0 - 1.0;

      upper_ray.position = camera_pos;
      upper_ray.direction = (look_dir + right * u + up * upper_v).normalize();
      upper_ray.λ = 0.0;
      upper_ray.pixel_color = {0, 0, 0};

      lower_ray.position = camera_pos;
      lower_ray.direction = (look_dir + right * u + up * lower_v).normalize();
      lower_ray.λ = 0.0;
      lower_ray.pixel_color = {0, 0, 0};

      rays.push_back(upper_ray);
      rays.push_back(lower_ray);
    }
  }
}


Camera::Camera() {
  std::cout << "\x1b[?25l";
  std::cout << "\x1b[2J\x1b[H";

  winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  width = w.ws_col;
  height = w.ws_row;
  aspect_ratio = double(width) / double(height*2) * 0.91; // aspect ratio

  blackhole = Blackhole(10000, 2, 0);
}
Camera::~Camera() {}


void Camera::new_frame() {
  std::string frame_buffer = "";

  init_rays();
  
  int idx = 0;
  for (int i = 0; i < rays.size(); i++) {
    if (blackhole.calculate(rays[i])) {                                // if collided with black hole
      if (i % 2 == 0) { frame_buffer += "\x1b[38;2;255;255;255m"; }    // upper ray
      else { frame_buffer += "\x1b[48;2;255;255;255m"; }               // lower ray
    } else {                                                           // not collided with black hole
      starfield.lookup_starfield(rays[i]);                             // changes RGB inside ray struct
      if (i % 2 == 0) {                                                // upper ray
        frame_buffer += "\x1b[38;2;" + std::to_string(rays[i].pixel_color.R) 
          + ";" + std::to_string(rays[i].pixel_color.G) + ";" 
          + std::to_string(rays[i].pixel_color.B) + "m"; 
      } else {                                                         // lower ray
        frame_buffer += "\x1b[48;2;" + std::to_string(rays[i].pixel_color.R)
          + ";" + std::to_string(rays[i].pixel_color.G) + ";"
          + std::to_string(rays[i].pixel_color.B) + "m";
      }
    }
    if (i % 2 != 0) {
      frame_buffer += "\u2580";
    }
    // if ((i+1) % width == 0) {
    //   frame_buffer += '\n';
    // }
  }
  std::cout << frame_buffer;
}

void Camera::set_camera_pos(Vec3 new_pos) {
  camera_pos = new_pos;
}
void Camera::set_camera_dir(Vec3 new_dir) {
  look_dir = new_dir;
}

