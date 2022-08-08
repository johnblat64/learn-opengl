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
#include "shader.h"



SDL_Window *window;
SDL_GLContext gl_context;
int window_w = 1600;
int window_h = 900;
int bpp;



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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    GLuint texture_1 = texture_create_from_image_file("awesomeface.png", GL_RGBA);    
    
    glBindTexture(GL_TEXTURE_2D, texture_1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    
    // SETUP SHADERS


    GLuint object_shader_program_id = program_create_from_shader_files
    (
        "shaders/object.vert",
        "shaders/object.frag"
    );

    GLuint light_source_shader_program_id = program_create_from_shader_files
    (
        "shaders/light-source.vert",
        "shaders/light-source.frag"
    );



    // VERTICES

    glm::vec3 light_position(1.2f, 1.0f, 2.0f);
    float light_speed = 0.04f;
    float light_radius = 3.0f;



    float vertices[] = {
        // pos                 // normal
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };




    GLuint light_VAO_id;
    glGenVertexArrays(1, &light_VAO_id);
    glBindVertexArray(light_VAO_id);

    GLuint light_VBO_id;
    glGenBuffers(1, &light_VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, light_VBO_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glUseProgram(object_shader_program_id);
    program_set_uniform_vec3(object_shader_program_id, "objectColor", glm::vec3(1.0f, 0.5f, 0.31f));









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
    camera.pitch = -13.0f;
    camera.position = glm::vec3(1.5f, 1.5f, 4.5f);
    camera.up = VECTOR_UP;
    camera.front = glm::vec3(-0.1f, -0.2f, -0.9f);
    camera.fov = 45.0f;

    const float camera_speed = 0.09f;



    //
    // MAIN LOOP
    //

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowMouseGrab(window, SDL_TRUE);
    
    SDL_Event event;
    bool camera_move_toggle = true;

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
            else if(event.type == SDL_KEYUP)
            {
                if(event.key.keysym.sym == SDLK_x)
                {
                    camera_move_toggle = !camera_move_toggle;
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



        if(camera_move_toggle)
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            SDL_SetWindowMouseGrab(window, SDL_TRUE);
        }
        else 
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_SetWindowMouseGrab(window, SDL_FALSE);
        }

        if(camera_move_toggle)
        {
            // camera 
            glm::vec3 move_direction = glm::vec3(camera.front.x, camera.front.y, camera.front.z);
            
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
            if(keyboard_state[SDL_SCANCODE_E])
            {
                camera.position += camera_speed * glm::normalize(VECTOR_UP);
            }
            if(keyboard_state[SDL_SCANCODE_Q])
            {
                camera.position -= camera_speed * glm::normalize(VECTOR_UP);
            }

            // light source

            

            move_direction = glm::vec3(camera.front.x, camera.front.y, camera.front.z);
            
            if(keyboard_state[SDL_SCANCODE_UP])
            {  
                light_position += light_speed * glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
            }
            if(keyboard_state[SDL_SCANCODE_DOWN])
            {
                light_position +=  light_speed * glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
            }
            if(keyboard_state[SDL_SCANCODE_LEFT])
            {
                light_position += light_speed * glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
            }
            if(keyboard_state[SDL_SCANCODE_RIGHT])
            {
                light_position += light_speed * glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
            }
            if(keyboard_state[SDL_SCANCODE_PAGEUP])
            {
                light_position += light_speed * glm::normalize(VECTOR_UP);
            }
            if(keyboard_state[SDL_SCANCODE_PAGEDOWN])
            {
                light_position -= light_speed * glm::normalize(VECTOR_UP);
            }
            // if(keyboard_state[SDL_SCANCODE_EQUALS])
            // {
            //     light_radius += 0.1;
            // }
            // if(keyboard_state[SDL_SCANCODE_MINUS])
            // {
            //     light_radius -= 0.1;
            // }
            // if(keyboard_state[SDL_SCANCODE_J])
            // {
            //     light_speed -= 0.01;
            // }
            // if(keyboard_state[SDL_SCANCODE_K])
            // {
            //     light_speed += 0.01;
            // }

            // light_position.x = sin((float)SDL_GetTicks()/100 * light_speed ) * light_radius;
            // light_position.z = cos((float)SDL_GetTicks()/100 * light_speed ) * light_radius;



            float sensitivity = 0.1f;

            float scaled_total_xrel = (float)total_xrel * sensitivity;
            float scaled_total_yrel = (float)total_yrel * sensitivity; 


            camera_update_with_mouse_offset(camera, scaled_total_xrel, scaled_total_yrel);
            camera_update_zoom(camera, -total_precise_scroll_y);
        }


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
        ImGui::InputFloat3("Camera position", glm::value_ptr(camera.position));
        ImGui::InputFloat3("Camera front", glm::value_ptr(camera.front));
        ImGui::InputFloat("Camera Yaw", &camera.yaw);
        ImGui::InputFloat("Camera Pitch", &camera.pitch);
        ImGui::InputFloat3("Light Position", glm::value_ptr(light_position));
        if(ImGui::Button("click me to close"))
        {
            show_imgui_window = false;
        }


        ImGui::End();
        ImGui::Render();

        // UPDATE CAMERA


        glm::mat4 view_mat4 = camera_view_mat4(camera);
        

        // RENDER
   
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



        glBindVertexArray(light_VAO_id);   

        glUseProgram(object_shader_program_id);

        // MATRIX TRANSFORMATION
        

        glm::mat4 projection_mat4 = glm::mat4(1.0f);
        projection_mat4 = glm::perspective
        (
            glm::radians(camera.fov),
            aspect_ratio_x/aspect_ratio_y,
            0.1f,
            100.0f
        );

        glm::vec3 light_color = glm::vec3(1.0, 1.0, 1.0);

        // object
        {
            glm::mat4 model_mat4 = glm::mat4(1.0f);

            program_set_uniform_vec3(object_shader_program_id, "lightPos", light_position);
            program_set_uniform_vec3(object_shader_program_id, "lightColor", light_color);

            program_set_uniform_mat4(object_shader_program_id, "view", view_mat4);
            program_set_uniform_mat4(object_shader_program_id, "projection", projection_mat4);
            program_set_uniform_mat4(object_shader_program_id, "model", model_mat4);
            program_set_uniform_vec3(object_shader_program_id, "viewPos", camera.position);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // light source
        {
            glm::mat4 model_mat4 = glm::mat4(1.0f);
            model_mat4 = glm::translate(model_mat4, light_position);
            model_mat4 = glm::scale(model_mat4, glm::vec3(0.2f));
            glUseProgram(light_source_shader_program_id);

            program_set_uniform_mat4(light_source_shader_program_id, "view", view_mat4);
            program_set_uniform_mat4(light_source_shader_program_id, "projection", projection_mat4);
            program_set_uniform_mat4(light_source_shader_program_id, "model", model_mat4);

            glBindVertexArray(light_VAO_id);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }



        SDL_GL_SwapWindow(window);

        ts_TimeStep_delay_remaining_time(timestep);

    }
}