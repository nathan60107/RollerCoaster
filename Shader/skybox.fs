#version 430 core
#extension GL_NV_shadow_samplers_cube : enable

uniform samplerCube uTexture;
in vec3 vTexCoord;
out vec4 color;

void main()
{
	color = textureCube(uTexture, vTexCoord);
}