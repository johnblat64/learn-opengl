#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include "timestep.h"



SDL_Window *window;
SDL_GLContext openglContext;
int window_W = 1920;
int window_h = 1080;
int bpp;

void quit_program()
{
    exit(EXIT_SUCCESS);
}

char *file_rightead_to_str(std::string filename){
    FILE *shader_file = fopen(filename.c_str(), "r");

    fseek(shader_file, 0, SEEK_END);

    size_t shader_file_size = ftell(shader_file);

    fseek(shader_file, 0, SEEK_SET);

    char *shader_text = (char *) calloc(shader_file_size + 1, sizeof(char));

    fread(shader_text, sizeof(char), shader_file_size, shader_file);

    shader_text[shader_file_size] = '\0';

    return shader_text;
    
}

unsigned int compile_shader(const char *shader_source_str, int shader_type)
{
    unsigned int shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_source_str, NULL);
    glCompileShader(shader_id);

    int compilation_success;
    char info_log[512];
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compilation_success);

    if(!compilation_success)
    {
        glGetShaderInfoLog(shader_id, 512, NULL, info_log);
        printf("error::shader::vertex::compilation_failed: %s\n", info_log);
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


int main(){
    SDL_Init(SDL_INIT_VIDEO);    

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);




    window = SDL_CreateWindow(
        "windur",
        0,
        0,
        window_W,
        window_h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL |SDL_WINDOW_RESIZABLE
    );

    openglContext = SDL_GL_CreateContext(window);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    glViewport(0,0,window_W, window_h);

    const char *vertex_shader_source_str = file_rightead_to_str("shaders/whatever.vert");
    const char *fragment_shader_source_str_orange = file_rightead_to_str("shaders/orange.frag");
    const char *fragment_shader_source_str_yellow = file_rightead_to_str("shaders/yellow.frag");


    GLuint vertex_shader_id = compile_shader(vertex_shader_source_str, GL_VERTEX_SHADER);

    GLuint fragment_shader_id_orange = compile_shader(fragment_shader_source_str_orange, GL_FRAGMENT_SHADER);
    GLuint fragment_shader_id_yellow = compile_shader(fragment_shader_source_str_yellow, GL_FRAGMENT_SHADER);


    GLuint shader_program_id_orange = glCreateProgram();
    GLuint shader_program_id_yellow = glCreateProgram();
  
    program_link_shaders(shader_program_id_orange, vertex_shader_id, fragment_shader_id_orange);
    program_link_shaders(shader_program_id_yellow, vertex_shader_id, fragment_shader_id_yellow);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id_orange);
    glDeleteShader(fragment_shader_id_yellow);





    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,   // top left 
        -0.1f, 0.0f, 0.0f, // center left
        0.1f, 0.0f, 0.0f // center right
    };

    unsigned int indices_right[] = {
        0, 1, 5, // 1st triangle
    };

    unsigned int indices_left[] = {
        3, 2, 4   // 2nd triangle
    };

    // 
    // ELEMENT BUFFER OBJECT
    //
    // holds indices that index into a VERTEX BUFFER OBJECT
    //
    glUseProgram(shader_program_id_orange);

    GLuint EBO_id_right;
    glGenBuffers(1, &EBO_id_right);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id_right);
    // glBufferData is a function specifically targeted to copy user-defined data into the currently bound buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_right), indices_right, GL_STATIC_DRAW);

    glUseProgram(shader_program_id_yellow);

    GLuint EBO_id_left;
    glGenBuffers(1, &EBO_id_left);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id_left);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_left), indices_left, GL_STATIC_DRAW);

    //
    // VERTEX ARRAY OBJECT
    //
    // holds an array of vertex buffers. Kind of like an array of arrays.
    // Vertex buffer objects need to be in a vertex array object to be rendered
    // 
    GLuint VAO_id_right;
    glGenVertexArrays(1, &VAO_id_right);

    GLuint VAO_id_left;
    glGenVertexArrays(1, &VAO_id_left);



    

    //
    // VERTEX BUFFER OBJECT
    //
    // holds a list of vertices that something like element buffer object will reference
    //
    GLuint VBO_id_right;
    glGenBuffers(1, &VBO_id_right);

    glBindVertexArray(VAO_id_right); // need to bind VAO to add VBO  to

    glBindBuffer(GL_ARRAY_BUFFER, VBO_id_right);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);



    GLuint VBO_id_left;
    glGenBuffers(1, &VBO_id_left);

    glBindVertexArray(VAO_id_left); // need to bind VAO to add VBO  to

    glBindBuffer(GL_ARRAY_BUFFER, VBO_id_left);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    


    TimeStep timestep = ts_TimeStep_init(60.0f);

    SDL_Event event;

    while(true){
        
        ts_TimeStep_start_ticks_set_to_current_ticks(timestep);

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit_program();
            }
            else if(event.type == SDL_WINDOWEVENT){
                if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                    SDL_GetWindowSize(window, &window_W, &window_h);
                    glViewport(0, 0, window_W, window_h);
                }
            }
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    quit_program();
                }
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program_id_orange);
        // glBindVertexArray(VAO_id);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id_right
);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glUseProgram(shader_program_id_yellow);


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id_left
);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);


        SDL_GL_SwapWindow(window);

        ts_TimeStep_delay_remaining_time(timestep);

    }



}