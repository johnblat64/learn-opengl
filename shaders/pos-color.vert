#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float horizontalOffset;

out vec3 ourColor; // output to give to fragment shader

void main()
{
    gl_Position = vec4(aPos.x + horizontalOffset, aPos.y, aPos.z, 1.0);
    ourColor = vec3(aPos.x, aPos.y, aPos.z);
}