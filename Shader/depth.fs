#version 430 core

in vec4 pos;

layout(location = 0) out float fragmentdepth;


void main(){
	//gl_FragColor = vec4( 1, 1, 0, 1);
	fragmentdepth = gl_FragCoord.z;
	//fragmentdepth = gl_FragDepth;
	//gl_FragDepth = gl_FragCoord.z;
	/*if(gl_FragCoord.x>450){
		fragmentdepth = 0.8;
	}else{
		fragmentdepth = 0.2;
	}*/	
}

/*
in vec4 pos;
 
vec4 pack( float depth )
{
    const vec4 bitSh = vec4( 256.0 * 256.0 * 256.0,
                             256.0 * 256.0,
                             256.0,
                             1.0 );
    const vec4 bitMsk = vec4( 0.0,
                              1.0 / 256.0,
                              1.0 / 256.0,
                              1.0 / 256.0 );
    vec4 comp = fract( depth * bitSh );//3210
    comp -= comp.xxyz * bitMsk;//3321*0---=3210
	
	//comp = dot(depth * bitSh);
	
    return comp;
}
 
void main( void )
{
    float normalizedZ = pos.z / pos.w;
    normalizedZ = ( normalizedZ + 1.0 ) / 2.0;
    gl_FragColor = pack( normalizedZ );
}*/