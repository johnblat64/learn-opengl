
#include <glad/glad.h>
#include "../include/util.h"
#include <stdio.h>
#include <malloc.h>
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <string>

static char *file_read_contents_into_char_ptr(std::string filename){
    FILE *file = fopen(filename.c_str(), "r");

    fseek(file, 0, SEEK_END);

    size_t file_size = ftell(file);

    fseek(file, 0, SEEK_SET);

    char *text = (char *) calloc(file_size + 1, sizeof(char));

    fread(text, sizeof(char), file_size, file);

    text[file_size] = '\0';

    return text;
    
}

GLuint shader_compile_from_src_string(const char *shader_source_str, int shader_type)
{
    GLuint shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_source_str, NULL);
    glCompileShader(shader_id);

    int compilation_success;
    char info_log[512];
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compilation_success);

    if(!compilation_success)
    {
        glGetShaderInfoLog(shader_id, 512, NULL, info_log);
        if(shader_type == GL_FRAGMENT_SHADER){
            printf("error::shader::fragment::compilation_failed: %s\n", info_log);
        }
        else{
            printf("error::shader::vertex::compilation_failed: %s\n", info_log);
        }
        quit_program();
    }

    return shader_id;
}


void program_link_shaders(GLuint program_id, GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id); 

    int program_link_success;
    char info_log[512];
    glGetProgramiv(program_id, GL_LINK_STATUS, &program_link_success);

    if(!program_link_success)
    {
        glGetProgramInfoLog(program_id, 512, NULL, info_log);
        printf("program link error: %s\n", info_log);
        quit_program();
    }

}


GLuint program_create_from_shader_files(const char *vertex_shader_filename, const char *shader_fragment_filename)
{
    const char *vertex_shader_src_str = file_read_contents_into_char_ptr(vertex_shader_filename);
    const char *fragment_shader_src_str = file_read_contents_into_char_ptr(shader_fragment_filename);
    GLuint vertex_shader_id = shader_compile_from_src_string(vertex_shader_src_str, GL_VERTEX_SHADER);
    GLuint fragment_shader_id = shader_compile_from_src_string(fragment_shader_src_str, GL_FRAGMENT_SHADER);
    GLuint program_id = glCreateProgram();
    program_link_shaders(program_id, vertex_shader_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}


void program_set_uniform_mat4(GLuint program_id, const char *uniform_name, glm::mat4 mat4)
{
    GLuint uniform_location = glGetUniformLocation
    (
        program_id, 
        uniform_name
    );

    glUniformMatrix4fv
    (
        uniform_location, 
        1, 
        GL_FALSE, 
        glm::value_ptr(mat4)
    );
}

void program_set_uniform_vec3(GLuint program_id, const char *uniform_name, glm::vec3 vec3)
{
    GLuint uniform_location = glGetUniformLocation
    (
        program_id, 
        uniform_name
    );
    glUniform3f(uniform_location, vec3.x, vec3.y, vec3.z);
}