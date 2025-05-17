#include "scene.h"
#include <iostream>

void application_update(Scene &scene)
{
  arcball_camera_update(
    scene.userCamera.arcballCamera,
    scene.userCamera.transform,
    engine::get_delta_time());
  // std::cout << "Update" << std::endl;
}
