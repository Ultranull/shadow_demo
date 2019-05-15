#version 430 core
precision highp float;

layout(location = 0) in vec3 vertpos;


uniform	mat4 projection;
uniform	mat4 view;
uniform mat4 model;

void main(){
    gl_Position=projection*view*model*vec4(vertpos,1);
}