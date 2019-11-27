#version 430 core

/*out vec4 color;

in vec2 uv;

uniform sampler2D Texture;

void main()
{
	color = texture2D(Texture, uv);
}*/

in vec3 position;
in vec3 worldNormal;
in vec3 eyeNormal;

out vec4 color;

uniform vec3 eyePos;
//uniform samplerCube envMap;

void main() {
     vec3 eye = normalize(eyePos - position);
     vec3 r = reflect(eye, worldNormal);
     //color = textureCube(envMap, r);
     color = vec4(1);
	 color.a = 0.5;
}
