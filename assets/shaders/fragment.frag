#version 330 core
out vec4 fragColor;

in vec3 normal;


void main(){
	vec4 d=vec4(1,0,0,1);
	float intensity=clamp(dot(normal,vec3(1,1,1)),0,1);
	fragColor=vec4(1)*(.7*intensity+.3);
}