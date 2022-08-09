#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPosWorldSpace;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPosViewSpace; // needed for 

void main()
{

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(view * model))) * aNormal; // this is needed incase there is any non uniform scaling

    LightPosViewSpace = vec3(view * vec4(lightPosWorldSpace, 1.0)); // transform world space light position to view space for use in the fragment shader
}