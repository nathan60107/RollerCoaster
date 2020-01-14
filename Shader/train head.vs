#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 nor;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform vec3 trainPos;
uniform mat4 depthP;
uniform mat4 depthMV;
uniform float rX;
uniform float rY;
uniform float rZ;
uniform float time;
uniform vec3 center;
uniform bool isWheel;
uniform float scale;

out vec2 texCord;
out vec4 ShadowCoord;
out vec3 norm;
out vec3 pos;
out vec3 eye_direction;

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

void main(void)
{
	vec4 v = vec4(vertex * scale, 1);
	if(isWheel){
		v = ((v - vec4(center * scale, 1)) * rotationZ(time)) + vec4(center * scale, 1);
	}
	v = v  * rotationY(rY) * rotationZ(rZ)* rotationX(rX);
	
	v = v + vec4(trainPos, 0) / 10;
	
	vec4 p = ProjectionMatrix * ModelViewMatrix * v;
	pos = p.xyz;
    gl_Position = p;
	
	ShadowCoord = depthP * depthMV * vec4(vertex+trainPos,1);
    texCord = tex;
	norm = mat3(ModelViewMatrix) * nor;
	eye_direction = (ModelViewMatrix * vec4(0)).xyz;
}
