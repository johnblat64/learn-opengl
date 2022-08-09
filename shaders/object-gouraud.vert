#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform vec3 lightPos;


out vec4 color;

void main()
{

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vec3 FragPos = vec3(view * model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(view * model))) * aNormal; // this is needed incase there is any non uniform scaling

    vec3 LightPos = vec3(view * vec4(lightPos, 1.0)); // transform world space light position to view space for use in the fragment shader

    // ambient
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * lightColor;

    // diffuese
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    // specular
    float specularStregnth = 0.5;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStregnth * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0);
}