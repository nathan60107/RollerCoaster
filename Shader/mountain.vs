#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 tex;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

out vec2 texCord;

void main(void)
{
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(vertex, 1.0);
    texCord = tex;
}
