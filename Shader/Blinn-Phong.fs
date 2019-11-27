#version 430 core

out vec4 color;

in vec3 vs_worldpos;
in vec3 vs_normal;

uniform vec4 color_ambient = vec4(0.1, 0.2, 0.5, 1.0);
uniform vec4 color_diffuse = vec4(0.2, 0.3, 0.6, 1.0);
uniform vec4 color_specular = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 Color = vec4(0.1, 0.1, 0.5, 1.0);
uniform float shininess = 7777.0f;
uniform vec3 light_position = vec3(50.0, 30.0, 560.0);
uniform vec3 eye_direction = vec3(0.0, 0.0, 1.0);

void main()
{
	vec3 light_dir = normalize(light_position - vs_worldpos);
	vec3 normal = normalize(vs_normal);
	vec3 half_vector = normalize(normalize(light_dir) + normalize(eye_direction));
	float diffuse = max(0.0, dot(normal, light_dir));
	float specular = pow(max(0.0, dot(vs_normal, half_vector)), shininess);
	color = min(Color * color_ambient, vec4(1.0)) 
			+ diffuse * color_diffuse
			+ specular * color_specular;
}
