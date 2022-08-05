#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "camera.h"

const glm::vec3 VECTOR_UP =  glm::vec3(0.0f, 1.0f, 0.0f); 


glm::mat4 camera_view_mat4(Camera camera)
{
    return glm::lookAt(camera.position, camera.position + camera.front, VECTOR_UP);
}



void camera_update_with_mouse_offset(Camera &camera, float mouse_offset_x, float mouse_offset_y)
{
    camera.yaw += mouse_offset_x;
    camera.pitch += mouse_offset_y;

    camera.pitch = MIN(camera.pitch,  89.0f);
    camera.pitch = MAX(camera.pitch, -89.0f);

    camera.front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front.y = sin(glm::radians(camera.pitch));
    camera.front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));

    camera.front = glm::normalize(camera.front);

    // camera.right = glm::normalize
    // (
    //     glm::cross(camera.front, camera.up)
    // );

    // camera.up = glm::normalize
    // (
    //     glm::cross(camera.right, camera.front)
    // );


}