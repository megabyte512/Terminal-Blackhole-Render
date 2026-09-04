#include "headers/camera.h"
#include "headers/input.h"

int main() {
  Camera camera;
  Input input;

  camera.set_camera_pos({9.0, 0.0, 0.0});
  camera.set_camera_dir({-1.0, 0.0, 0.0});
  camera.new_frame(); // runs a new everything

  return 0;
}
