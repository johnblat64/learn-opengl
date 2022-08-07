#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "camera.h"

const glm::vec3 VECTOR_UP =  glm::vec3(0.0f, 1.0f, 0.0f); 

glm::mat4 my_lookAt(Camera &camera)
{
    glm::vec3 camera_target = camera.position + camera.front;
    // camera direction
    glm::vec3 camera_zaxis = glm::normalize(camera.position - camera_target);
    // positive right axis vectort
    glm::vec3 camera_xaxis = glm::normalize
    (
        glm::cross
        (
            glm::normalize(VECTOR_UP),
            camera_zaxis
        )
    );
    // camera up vector
    glm::vec3 camera_yaxis = glm::cross(camera_zaxis, camera_xaxis);


    glm::mat4 rotation_mat4 = glm::mat4(1.0f);
    rotation_mat4[0][0] = camera_xaxis.x; // First column, first row
    rotation_mat4[1][0] = camera_xaxis.y;
    rotation_mat4[2][0] = camera_xaxis.z;
    rotation_mat4[0][1] = camera_yaxis.x; // First column, second row
    rotation_mat4[1][1] = camera_yaxis.y;
    rotation_mat4[2][1] = camera_yaxis.z;
    rotation_mat4[0][2] = camera_zaxis.x; // First column, third row
    rotation_mat4[1][2] = camera_zaxis.y;
    rotation_mat4[2][2] = camera_zaxis.z; 


    glm::mat4 translation_mat4 = glm::mat4(1.0f);
    translation_mat4[3][0] = -camera.position.x; // Third column, first row
    translation_mat4[3][1] = -camera.position.y;
    translation_mat4[3][2] = -camera.position.z;    

    glm::mat4 look_at = rotation_mat4 * translation_mat4;

    return look_at;

}

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

    camera.right = glm::normalize
    (
        glm::cross(camera.front, camera.up)
    );

    camera.up = glm::normalize
    (
        glm::cross(camera.right, camera.front)
    );
}

void camera_update_zoom(Camera &camera, float zoom_amount)
{
    camera.fov += zoom_amount;
    camera.fov = MAX(1.0f, camera.fov);
    camera.fov = MIN(80.0f, camera.fov);
}

