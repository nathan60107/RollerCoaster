#version 430 core
const float pi = 3.14159;
/*const float period = 100.0;
const float amplitude = 5.0;*/

layout(location = 0) in vec3 vertex;
//layout(location = 1) in vec3 normal;

out vec2 uv;
out vec3 position;
out vec3 worldNormal;
out vec3 eyeNormal;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

uniform float waterHeight;
uniform float time;
uniform int numWaves;
uniform float amplitude[8];
uniform float wavelength[8];
uniform float speed[8];
uniform vec2 direction[8];

float wave(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    return amplitude[i] * sin(theta * frequency + time * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
    for (int i = 0; i < numWaves; ++i)
        height += wave(i, x, y);
    return height;
}

float dWavedx(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].y * frequency;
    return A * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < numWaves; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, -dy, 1.0);
    return normalize(n);
}

void main(void)
{
	vec4 pos = vec4(vertex, 1.0);
    pos.z = waterHeight + waveHeight(pos.x, pos.y);
    position = pos.xyz / pos.w;
    worldNormal = waveNormal(pos.x, pos.y);
    eyeNormal = (inverse(transpose(ModelViewMatrix)) * vec4(worldNormal, 0)).xyz;
    gl_Position = ProjectionMatrix * ModelViewMatrix * pos;
	/*vec3 v = vertex;
	//v.y += 30;
	v.y *= height/25.5;
	//v.y += amplitude * sin( v.x*1000.0 + t/period*2*pi);
	vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(v, 1.0);
    gl_Position = pos;
	
	uv = textureCord;*/
}
