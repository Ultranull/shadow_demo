#version 330 core
out vec4 fragColor;

in vec3 normal;
in vec2 uv;
in vec4 shadowCoord;  
in vec3 FragPos;  

uniform sampler2D uvmap;
uniform sampler2D depthmap;

void main(){

	vec2 poissonDisk[4] = vec2[](
	  vec2( -0.94201624, -0.39906216 ),
	  vec2( 0.94558609, -0.76890725 ),
	  vec2( -0.094184101, -0.92938870 ),
	  vec2( 0.34495938, 0.29387760 )
	);

	vec3 projcoords = shadowCoord.xyz/shadowCoord.w;
    projcoords = projcoords * 0.5 + 0.5;
	float visibility = 1.0;
	for (int i=0;i<4;i++){
		if ( texture( depthmap, projcoords.xy + poissonDisk[i]/700.0 ).r  <  projcoords.z-0.005 ){
			visibility-=0.2;
		}
	}

	float intensity=clamp(dot(normal,vec3(1,1,1)),0,1);
	fragColor=visibility*texture(uvmap,uv)*(.7*intensity+.3);
}