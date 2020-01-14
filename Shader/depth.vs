#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;

out vec4 pos;

// Values that stay constant for the whole mesh.
//uniform mat4 depthMVP;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform float rX;
uniform float rY;
uniform float rZ;
uniform vec3 trainPos;

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY( in float angle ) {
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

mat4 rotationZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

void main(){
	vec4 v = vec4(vertexPosition_modelspace, 1);
	v = v  * rotationY(rY) * rotationZ(rZ)* rotationX(rX);
	
	v = v + vec4(trainPos, 0) / 10;	
	
	pos = ProjectionMatrix * ModelViewMatrix * v;
	gl_Position =  ProjectionMatrix * ModelViewMatrix * v;
}
