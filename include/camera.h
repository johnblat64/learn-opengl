#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "util.h"
#include "math.h"


struct Camera
{
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float yaw; // rotation around y axis
    float pitch; // rotation around x axis
};

glm::mat4 camera_view_mat4(Camera camera);

void camera_update_with_mouse_offset(Camera &camera, float mouse_offset_x, float mouse_offset_y);
