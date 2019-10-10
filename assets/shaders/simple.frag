#version 330 core
out vec4 fragColor;

in vec2 uv;

uniform samplerCube uvmap;


void main(){
	vec2 uv2 = uv*2-1;
	fragColor=texture(uvmap,vec3(uv2,-1*cos(sqrt(uv2.x*uv2.x+uv2.y*uv2.y)))).rrra;
}