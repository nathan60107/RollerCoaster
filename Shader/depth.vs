#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;

out vec4 pos;

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

void main(){
	pos = ProjectionMatrix * ModelViewMatrix * vec4(vertexPosition_modelspace,1);
	gl_Position =  ProjectionMatrix * ModelViewMatrix * vec4(vertexPosition_modelspace,1);
}

/*
layout(location = 0) in vec3 vertexPosition_modelspace;
 
uniform mat4 depthMVP;
 
out vec4 pos;
 
void main( void )
{
    vec3 finalPosition = vertexPosition_modelspace;
    pos = depthMVP * vec4( finalPosition, 1.0 );
    gl_Position = pos;
}*/
