#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 nor;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 depthP;
uniform mat4 depthMV;

out vec2 texCord;
out vec4 ShadowCoord;
out vec3 norm;
out vec3 pos;
out vec3 eye_direction;

void main(void)
{
	pos = (ProjectionMatrix * ModelViewMatrix * vec4(vertex, 1.0)).xyz;
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(vertex, 1.0);
	ShadowCoord = depthP * depthMV * vec4(vertex,1);
    texCord = tex;
	norm = mat3(ModelViewMatrix) * nor;
	eye_direction = (ModelViewMatrix * vec4(0)).xyz;
}
