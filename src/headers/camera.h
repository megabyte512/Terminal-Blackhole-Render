#pragma once

#include "common.h"
#include "blackhole.h"
#include "starfield.h"

#include <string>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

class Camera {
  private:
    int width;
    int height;
    double aspect_ratio;
    Vec3 camera_pos;
    Vec3 look_dir;
    std::vector<Ray> rays;

    Blackhole blackhole;
    Starfield starfield;

    /// @brief initializes all ray positions and directions
    void init_rays();

  public:
    Camera();
    ~Camera();

    /// @brief renders the frame
    void new_frame();
    /// @brief sets the camera position
    void set_camera_pos(Vec3 new_pos);
    /// @brief sets where camera is looking
    void set_camera_dir(Vec3 new_dir);

};
