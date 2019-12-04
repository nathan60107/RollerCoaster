#version 430 core

in vec2 texCord;
in vec4 ShadowCoord;

out vec4 fColor;

uniform sampler2D Texture;
uniform sampler2DShadow shadowMap;


void main()
{
	float visibility = texture( shadowMap, vec3(ShadowCoord.xy, (ShadowCoord.z)/ShadowCoord.w) );

	fColor = visibility * texture(Texture, texCord);
}
