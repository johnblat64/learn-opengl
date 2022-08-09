#version 330 core

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};


struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;
  

uniform Material material;
uniform Light light;


in vec3 FragPos;
in vec3 Normal;
in vec3 LightPosViewSpace;


void main()
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuese
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPosViewSpace - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);


    // specular
    float specularStregnth = 0.5;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}