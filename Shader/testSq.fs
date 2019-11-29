#version 430 core

in vec3 position;
in vec3 worldNormal;
in vec3 eyeNormal;

out vec4 color;

uniform vec3 eyePos;
//uniform samplerCube envMap;

uniform sampler2D Texture;

void main() {
     vec3 eye = normalize(eyePos - position);
     vec3 r = reflect(eye, worldNormal);
     //color = textureCube(envMap, r);
	 color = vec4(0, 0, 0.5, 0.5);
	 
	 /*color = texture2D(Texture,vec2(0,0));     
	 color.a = 1;*/
}
