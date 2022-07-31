#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include "timestep.h"
#include "math.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MIN(a, b)(a < b ? a : b)
#define MAX(a, b)(a > b ? a : b)


SDL_Window *window;
SDL_GLContext openglContext;
int window_W = 1080;
int window_h = 1080;
int bpp;

void quit_program()
{
    exit(EXIT_SUCCESS);
}


GLuint texture_create_from_image_file(const char *image_filename, GLenum format)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, num_channels;
    unsigned char *data = stbi_load(image_filename, &width, &height, &num_channels, 0);

    if(!data){
        fprintf(stderr, "Failed to load texture %s\n", image_filename);
        quit_program();
    }

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}


char *file_read_contents_into_char_ptr(std::string filename){
    FILE *shader_file = fopen(filename.c_str(), "r");

    fseek(shader_file, 0, SEEK_END);

    size_t shader_file_size = ftell(shader_file);

    fseek(shader_file, 0, SEEK_SET);

    char *shader_text = (char *) calloc(shader_file_size + 1, sizeof(char));

    fread(shader_text, sizeof(char), shader_file_size, shader_file);

    shader_text[shader_file_size] = '\0';

    return shader_text;
    
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


    // SETUP TEXTURE

    GLuint texture_0 = texture_create_from_image_file("container.jpg", GL_RGB);

    glBindTexture(GL_TEXTURE_2D, texture_0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLuint texture_1 = texture_create_from_image_file("awesomeface.png", GL_RGBA);    
    
    glBindTexture(GL_TEXTURE_2D, texture_1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    

    
    
    // SETUP SHADERS



    GLuint shader_program_id = program_create_from_shader_files(
        "shaders/textures.vert",
        "shaders/textures.frag"
    );


    float vertices[] = {
        // pos                 // colors           // texture coords
         0.5f,  0.5f,  0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top right
         0.5f, -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // bottom right
        -0.5f, -0.5f,  0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f  // topleft
    };

    unsigned int indices[] = {
        3, 2, 0, // 1st triangle
        0, 1, 2, // 2nd triangle
    };

    // 
    // ELEMENT BUFFER OBJECT
    //
    // holds indices that index into a VERTEX BUFFER OBJECT
    //
    glUseProgram(shader_program_id);

    GLuint EBO_id;
    glGenBuffers(1, &EBO_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id);
    // glBufferData is a function specifically targeted to copy user-defined data into the currently bound buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //
    // VERTEX ARRAY OBJECT
    //
    // holds an array of vertex buffers. Kind of like an array of arrays.
    // Vertex buffer objects need to be in a vertex array object to be rendered
    // 
    GLuint VAO_id;
    glGenVertexArrays(1, &VAO_id);



    //
    // VERTEX BUFFER OBJECT
    //
    // holds a list of vertices that something like element buffer object will reference
    //
    GLuint VBO_id;
    glGenBuffers(1, &VBO_id);

    glBindVertexArray(VAO_id); // need to bind VAO to add VBO  to

    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int number_of_vertices = 4;
    unsigned int number_of_all_floats = sizeof(vertices) / (number_of_vertices * sizeof(float));
    unsigned int size_of_all_attributes = number_of_all_floats * sizeof(float);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size_of_all_attributes, (void *)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, size_of_all_attributes, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, size_of_all_attributes, (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // set samples
    glUseProgram(shader_program_id);
    glUniform1i(glGetUniformLocation(shader_program_id, "ourTexture1"), 0);
    glUniform1i(glGetUniformLocation(shader_program_id, "ourTexture2"), 1);


    float visibility_amount = 0.5f;

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

        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        
        if(keyboardState[SDL_SCANCODE_UP])
        {
            visibility_amount += 0.02;
            visibility_amount = MIN(visibility_amount, 1.0f);
        }
        else if(keyboardState[SDL_SCANCODE_DOWN])
        {
            visibility_amount -= 0.02;
            visibility_amount = MAX(visibility_amount, 0.0f);

        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program_id);
        
        // float time_value = (float) SDL_GetTicks() / 1000.0f; // seconds
        // float green_value = (sin(time_value) / 2.0f) + 0.5;

        // int vertex_ourColor_location = glGetUniformLocation(shader_program_id, "ourColor");
        // int vertex_horizontalOffset_location = glGetUniformLocation(shader_program_id, "horizontalOffset");

        // glUseProgram(shader_program_id);
        // glUniform4f(vertex_ourColor_location, 0.0f, green_value, 0.0f, 1.0f);
        // glUniform1f(vertex_horizontalOffset_location, -0.3f);
        glUniform1f(glGetUniformLocation(shader_program_id, "visibilityAmount"), visibility_amount);    
        // glBindVertexArray(VAO_id);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_1);

        glBindVertexArray(VAO_id);        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id);
        // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // glBindVertexArray(VAO_id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    

        SDL_GL_SwapWindow(window);

        ts_TimeStep_delay_remaining_time(timestep);

    }
}