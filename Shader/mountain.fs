#version 430 core

in vec2 texCord;

out vec4 fColor;

uniform sampler2D Texture;

void main()
{
	fColor = texture(Texture, texCord);
}
