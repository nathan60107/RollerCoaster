#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

out vec3 vs_worldpos;
out vec3 vs_normal;

void main(void)
{
	vec3 v = vertex;
	v.y += 30;
	vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(v, 1.0);
    gl_Position = pos;
	vs_worldpos = pos.xyz;
    vs_normal = mat3(ModelViewMatrix)*normal;
}
