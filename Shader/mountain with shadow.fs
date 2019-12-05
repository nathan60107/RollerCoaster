#version 330 core

in vec2 texCord;
in vec4 ShadowCoord;

out vec4 fColor;

uniform sampler2D Texture;
uniform sampler2D shadowMap;

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
    vec3 color = texture(Texture, texCord).rgb;
    
    // 計算陰影
    float shadow = ShadowCalculation(ShadowCoord);       
    vec3 lighting = (1.0 - shadow) * color;    

    fColor = vec4(lighting, 1.0f);
}


/*
in vec2 texCord;
in vec4 ShadowCoord;

out vec3 fColor;

uniform sampler2D Texture;
uniform sampler2D shadowMap;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

void main()
{
	float visibility = texture( shadowMap, ShadowCoord.xy ).r;
	
	/*float visibility=1.0;

	float bias = 0.005;

	for (int i=0;i<4;i++){
		int index = i;
		visibility -= 0.2*(1.0-texture( shadowMap, vec3(ShadowCoord.xy + poissonDisk[index]/700.0,  (ShadowCoord.z-bias)/ShadowCoord.w) ));
	}

	fColor = visibility * texture(Texture, texCord).rgb;
}*/



/*
in vec2 texCord;
in vec4 ShadowCoord;

out vec4 fColor;

uniform sampler2D Texture;
uniform sampler2D shadowMap;

float unpack (vec4 colour)
{
    const vec4 bitShifts = vec4(1.0 / (256.0 * 256.0 * 256.0),
                                1.0 / (256.0 * 256.0),
                                1.0 / 256.0,
                                1);
    return dot(colour , bitShifts);
}

float shadowSimple( )
{
    vec4 shadowMapPosition = ShadowCoord / ShadowCoord.w;
 
    shadowMapPosition = (shadowMapPosition + 1.0) /2.0;
 
    vec4 packedZValue = texture2D(shadowMap, shadowMapPosition.st);
 
    float distanceFromLight = unpack(packedZValue);
 
    //add bias to reduce shadow acne (error margin)
    float bias = 0.0005;
 
    //1.0 = not in shadow (fragment is closer to light than the value stored in shadow map)
    //0.0 = in shadow
	if(distanceFromLight > shadowMapPosition.z - bias){
		return 1.0;
	}else
		return 0.0;
    //return float(distanceFromLight > shadowMapPosition.z - bias);
}

void main()
{

	float shadow = 1.0;
    if ( ShadowCoord.w > 0.0 )
    {
        shadow = shadowSimple();
        shadow = shadow * 0.8 + 0.2;
    }

	fColor = shadow * texture(Texture, texCord);
	 
	//fColor = vec4(gl_FragCoord.z, 0, 0, 1.0);
}*/