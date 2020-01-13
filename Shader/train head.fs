#version 330 core

in vec2 texCord;
in vec4 ShadowCoord;
in vec3 norm;
in vec3 pos;
in vec3 eye_direction;

out vec4 fColor;

uniform sampler2D Texture;
uniform sampler2D shadowMap;
//uniform vec3 eye_direction = vec3(-0, -50, -250);

uniform vec4 color_ambient = vec4(0.1, 0.2, 0.5, 1.0);
uniform vec4 color_diffuse = vec4(0.2, 0.3, 0.6, 1.0);
uniform vec4 color_specular = vec4(1.0, 1.0, 1.0, 1.0);
//uniform vec4 Color = vec4(0.1, 0.1, 0.5, 1.0);
uniform float shininess = 7777.0f;
uniform vec3 light_position = vec3(0, 50, 250);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 執行透視除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 變換到[0,1]的範圍
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近點的深度(使用[0,1]範圍下的fragPosLight當坐標)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得當前片元在光源視角下的深度
    float currentDepth = projCoords.z;
    // 檢查當前片元是否在陰影中
	float bias = 0.005;
    float shadow = currentDepth > closestDepth + bias ? 0.8 : 0.0;

    return shadow;
}

void main()
{           
    vec4 color = vec4(texture(Texture, texCord).rgb, 1);
	
	vec3 light_dir = normalize(light_position - pos);
	vec3 normal = normalize(norm);
	vec3 half_vector = normalize(normalize(light_dir) + normalize(eye_direction));
	float diffuse = max(0.0, dot(normal, light_dir));
	float specular = pow(max(0.0, dot(norm, half_vector)), shininess);
	/*color = min(color * color_ambient, vec4(1.0)) 
			+ diffuse * color_diffuse
			+ specular * color_specular;*/
    
    // 計算陰影
    float shadow = ShadowCalculation(ShadowCoord);       
    vec4 lighting = (1.0 - shadow) * color;    

    fColor = lighting;
}