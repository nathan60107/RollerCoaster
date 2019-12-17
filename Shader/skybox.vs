#version 430 core

layout(location = 0) in vec3 aPosition;
out vec3 vTexCoord;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

void main()
{
	gl_Position = (ProjectionMatrix * ModelViewMatrix * vec4(aPosition, 1.0)).xyww;
	vTexCoord = aPosition;
}