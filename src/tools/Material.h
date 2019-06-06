#pragma once

#include<glad/glad.h>

#include <glm/glm.hpp>


struct TextureTraits {
	GLuint target;
	GLuint FILTER;
	GLuint WRAP;
	GLuint internalformat, format;
};

static TextureTraits RGBA_2D = {GL_TEXTURE_2D,GL_NEAREST,GL_CLAMP_TO_EDGE,GL_RGBA,GL_RGBA};
static TextureTraits RGB_2D = {GL_TEXTURE_2D,GL_NEAREST,GL_CLAMP_TO_EDGE,GL_RGB,GL_RGB};
static TextureTraits DEPTH_3D = { GL_TEXTURE_CUBE_MAP,GL_NEAREST,GL_CLAMP_TO_EDGE,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT };

struct Texture {


	GLuint id;
	TextureTraits params;



	int width, height;
	Texture():Texture(RGBA_2D) {}

	Texture(TextureTraits t):params(t){
		glGenTextures(1, &id);
	}

	void load();
	void setSize(int w,int h);

	void bind();
	GLint activate(GLenum texture);
	void cleanup();


	static void unbind(GLuint target = GL_TEXTURE_2D);
};

struct Material {
	glm::vec3 ambient;
	Texture diffuse;
	glm::vec3 specular;
	glm::vec3 color;
	float shininess;
};

