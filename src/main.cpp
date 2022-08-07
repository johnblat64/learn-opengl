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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "util.h"
#include "camera.h"




SDL_Window *window;
SDL_GLContext gl_context;
int window_w = 1600;
int window_h = 900;
int bpp;

void quit_program()
{
    exit(EXIT_SUCCESS);
}



GLuint texture_create_from_image_file(const char *image_filename, GLenum format)
{
    stbi_set_flip_vertically_on_load(true); // opengl tex coordinates are opposite
    int width, height, num_channels;
    unsigned char *data = stbi_load(image_filename, &width, &height, &num_channels, 0);

    if(!data)
    {
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


int main()
{
    SDL_Init(SDL_INIT_VIDEO);    

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


    window = SDL_CreateWindow(
        "windur",
        0,
        0,
        window_w,
        window_h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL |SDL_WINDOW_RESIZABLE
    );


    gl_context = SDL_GL_CreateContext(window);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    glViewport(0,0,window_w, window_h);

    glEnable(GL_DEPTH_TEST);

    // IMGUI Setup
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");


    // SETUP TEXTURE

    GLuint texture_0 = texture_create_from_image_file("container.jpg", GL_RGB);

    glBindTexture(GL_TEXTURE_2D, texture_0);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLuint texture_1 = texture_create_from_image_file("awesomeface.png", GL_RGBA);    
    
    glBindTexture(GL_TEXTURE_2D, texture_1);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    
    // SETUP SHADERS


    GLuint shader_program_id = program_create_from_shader_files
    (
        "shaders/transform.vert",
        "shaders/transform.frag"
    );

    GLuint shader_program_id_color = program_create_from_shader_files
    (
        "shaders/color-example.vert",
        "shaders/color-example.frag"
    );

    // VERTICES

    glm::vec3 cube_positions[] = 
    {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)          
    };
    size_t num_cubes = 10;

    

    float vertices[] = {
        // pos                  // uv
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
      
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
      
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
      
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
      
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
    
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f
    };




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

    unsigned int number_of_vertices = 36;
    unsigned int number_of_all_floats = sizeof(vertices) / (number_of_vertices * sizeof(float));
    unsigned int size_of_all_attributes = number_of_all_floats * sizeof(float);

    // position
    glVertexAttribPointer
    (
        0, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        size_of_all_attributes, 
        (void *)0
    );
    glEnableVertexAttribArray(0);


    // texture coords
    glVertexAttribPointer
    (
        1, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        size_of_all_attributes, 
        (void *)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // set samples
    glUseProgram(shader_program_id);

    GLuint our_texture_1_location = glGetUniformLocation(shader_program_id, "ourTexture1"); 
    glUniform1i(our_texture_1_location, 0);

    GLuint our_texture_2_location = glGetUniformLocation(shader_program_id, "ourTexture2"); 
    glUniform1i(our_texture_2_location, 1);



    // SETUP FLOOR
    float vertices2[] = {
        // pos                // color
        -1.0f, 0.0f, -1.0f,   0.0f, 0.7f, 0.0f, // top left
         1.0f, 0.0f,  1.0f,   0.0f, 0.7f, 0.0f, // top right
        -1.0f, 0.0f,  1.0f,   0.0f, 0.7f, 0.0f, // bottom left
         1.0f, 0.0f, -1.0f,   0.0f, 0.7f, 0.0f  // bottom right
    };
    
    unsigned int indices2[] = {
        0, 1, 2, // triangle 1
        1, 0, 3  // triangle 2
    };



    GLuint VAO_id_2;
    glGenVertexArrays(1, &VAO_id_2);

    GLuint VBO_id_2;
    glGenBuffers(1, &VBO_id_2);

    glBindVertexArray(VAO_id_2); // need to bind VAO to add VBO  to

    glBindBuffer(GL_ARRAY_BUFFER, VBO_id_2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

        // position
    glVertexAttribPointer
    (
        0, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        6 * sizeof(float), 
        (void *)0
    );
    glEnableVertexAttribArray(0);


    // color 
    glVertexAttribPointer
    (
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        6 * sizeof(float), 
        (void *)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    GLuint EBO_id_2;
    glGenBuffers(1, &EBO_id_2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id_2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);




    // TIMESTEP

    TimeStep timestep = ts_TimeStep_init(60.0f);

    //
    // VIEW AND CAM VARS
    //

    float aspect_ratio_x = window_w;
    float aspect_ratio_y = window_h;

    const glm::vec3 VECTOR_UP =  glm::vec3(0.0f, 1.0f, 0.0f); 

    Camera camera;
    camera.yaw = -90.0f;
    camera.pitch = 0.f;
    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
    camera.up = VECTOR_UP;
    camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.fov = 45.0f;

    const float camera_speed = 0.09f;



    //
    // MAIN LOOP
    //

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowMouseGrab(window, SDL_TRUE);
    
    SDL_Event event;

    while(true)
    {

        int total_xrel = 0;
        int total_yrel = 0;

        float total_precise_scroll_y = 0;


        ts_TimeStep_start_ticks_set_to_current_ticks(timestep);

        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT)
            {
                quit_program();
            }
            else if(event.type == SDL_WINDOWEVENT)
            {
                if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    SDL_GetWindowSize(window, &window_w, &window_h);
                    glViewport(0, 0, window_w, window_h);
                }
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit_program();
                }
            }
            else if(event.type == SDL_MOUSEMOTION)
            {
                SDL_Window *focus_window = SDL_GetMouseFocus();
                if(focus_window) // only move the mouse position if the window is in focus
                {
                    total_xrel += event.motion.xrel;
                    total_yrel -= event.motion.yrel;
                }
            }
            else if(event.type == SDL_MOUSEWHEEL)
            {
                total_precise_scroll_y += event.wheel.preciseY;
            }
        }

        // INPUT
        const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);

        glm::vec3 move_direction = glm::vec3(camera.front.x, 0.0f, camera.front.z);
        
        if(keyboard_state[SDL_SCANCODE_W])
        {  
            camera.position += camera_speed * glm::normalize(move_direction);
        }
        if(keyboard_state[SDL_SCANCODE_S])
        {
            camera.position -=  camera_speed * glm::normalize(move_direction);
        }
        if(keyboard_state[SDL_SCANCODE_A])
        {
            camera.position -= camera_speed * glm::normalize
            (
                glm::cross(move_direction, VECTOR_UP)
            );
        }
        if(keyboard_state[SDL_SCANCODE_D])
        {
            camera.position += camera_speed * glm::normalize
            (
                glm::cross(move_direction, VECTOR_UP)
            );
        }
        float sensitivity = 0.1f;

        float scaled_total_xrel = (float)total_xrel * sensitivity;
        float scaled_total_yrel = (float)total_yrel * sensitivity; 


        camera_update_with_mouse_offset(camera, scaled_total_xrel, scaled_total_yrel);
        camera_update_zoom(camera, -total_precise_scroll_y);

        // GUI

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        bool show_imgui_window = true;
        ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
        ImGui::Begin("Modify Window", &show_imgui_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::InputFloat("FOV", &camera.fov, 0.5f, 0.0f, "%.2f", 0);
        ImGui::InputFloat("Aspect Ratio X", &aspect_ratio_x , 1.0f, 0.0f, "%.2f", 0);
        ImGui::InputFloat("Aspect Ratio Y", &aspect_ratio_y, 1.0f, 0.0f, "%.2f", 0);

        ImGui::End();
        ImGui::Render();

        // UPDATE CAMERA


        glm::mat4 view_mat4 = camera_view_mat4(camera);
        

        // RENDER
   
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_1);

        glBindVertexArray(VAO_id);   

        glUseProgram(shader_program_id);

        // UNIFORMS



        // MATRIX TRANSFORMATION
        

        glm::mat4 projection_mat4 = glm::mat4(1.0f);
        projection_mat4 = glm::perspective
        (
            glm::radians(camera.fov),
            aspect_ratio_x/aspect_ratio_y,
            0.1f,
            100.0f
        );


        for(int i = 0; i < num_cubes; i++)
        {
            glm::mat4 model_mat4 = glm::mat4(1.0f);
            model_mat4 = glm::translate                           
            (
                model_mat4,
                cube_positions[i]
            );


            GLuint model_uniform_location = glGetUniformLocation
            (
                shader_program_id, 
                "model"
            );
            glUniformMatrix4fv
            (
                model_uniform_location, 
                1, 
                GL_FALSE, 
                glm::value_ptr(model_mat4)
            );
            
            GLuint view_uniform_location = glGetUniformLocation
            (
                shader_program_id,
                "view"
            );
            glUniformMatrix4fv
            (
                view_uniform_location,
                1,
                GL_FALSE,
                glm::value_ptr(view_mat4)
            );

            GLuint projection_uniform_location = glGetUniformLocation
            (
                shader_program_id,
                "projection"
            );
            glUniformMatrix4fv
            (
                projection_uniform_location,
                1,
                GL_FALSE,
                glm::value_ptr(projection_mat4)
            );

            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        glUseProgram(shader_program_id_color);
        glBindVertexArray(VAO_id_2);


        glm::mat4 model_mat4 = glm::mat4(1.0f);
        model_mat4 = glm::translate                           
        (
            model_mat4,
            glm::vec3(-1.0f)
        );
        model_mat4 = glm::scale(model_mat4, glm::vec3(20.0f, 20.0f, 20.0f));


        GLuint model_uniform_location = glGetUniformLocation
        (
            shader_program_id_color, 
            "model"
        );
        glUniformMatrix4fv
        (
            model_uniform_location, 
            1, 
            GL_FALSE, 
            glm::value_ptr(model_mat4)
        );
        
        GLuint view_uniform_location = glGetUniformLocation
        (
            shader_program_id_color,
            "view"
        );
        glUniformMatrix4fv
        (
            view_uniform_location,
            1,
            GL_FALSE,
            glm::value_ptr(view_mat4)
        );

        GLuint projection_uniform_location = glGetUniformLocation
        (
            shader_program_id_color,
            "projection"
        );
        glUniformMatrix4fv
        (
            projection_uniform_location,
            1,
            GL_FALSE,
            glm::value_ptr(projection_mat4)
        );

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);

        ts_TimeStep_delay_remaining_time(timestep);

    }
}