#pragma once

#include <glad/glad.h>



GLuint shader_compile_from_src_string(const char *shader_source_str, int shader_type);

void program_link_shaders(GLuint program_id, GLuint vertex_shader_id, GLuint fragment_shader_id);

GLuint program_create_from_shader_files(const char *vertex_shader_filename, const char *shader_fragment_filename);
void program_set_uniform_mat4(GLuint program_id, const char *uniform_name, glm::mat4 mat4);
void program_set_uniform_vec3(GLuint program_id, const char *uniform_name, glm::vec3 vec3);
void program_set_uniform_float(GLuint program_id, const char *uniform_name, float f);
