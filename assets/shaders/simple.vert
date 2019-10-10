#version 430 core
precision highp float;

layout(location = 0) in vec3 vertpos;
layout(location = 2) in vec2 vertuv;

out vec2 uv;

layout(std140,binding=0)uniform camera{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main(){
    gl_Position=projection*view*model*vec4(vertpos,1);
	uv=vertuv;
}