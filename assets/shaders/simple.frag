#version 330 core
out vec4 fragColor;

in vec2 uv;
in vec3 normal;

uniform samplerCube uvmap;


void main(){
	vec2 uv2 = uv*2-1;//vec3(uv2,-1*cos(sqrt(uv2.x*uv2.x+uv2.y*uv2.y)))
	fragColor=texture(uvmap,normal).rrra;
}