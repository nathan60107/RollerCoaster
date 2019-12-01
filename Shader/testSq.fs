#version 430 core
#extension GL_NV_shadow_samplers_cube : enable

in vec3 position;
in vec3 worldNormal;
in vec3 eyeNormal;

out vec4 color;

uniform vec3 eyePos;
uniform samplerCube envMap;

void main() {
     vec3 eye = normalize(eyePos - position);
     vec3 r = reflect(eye, worldNormal);
     color = textureCube(envMap, r);
	 color.a = 0.5;
}
