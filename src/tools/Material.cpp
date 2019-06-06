#include "Material.h"




void Texture::load(){
	glTexParameteri(params.target, GL_TEXTURE_MAG_FILTER, params.FILTER);
	glTexParameteri(params.target, GL_TEXTURE_MIN_FILTER, params.FILTER);
	glTexParameteri(params.target, GL_TEXTURE_WRAP_S, params.WRAP);
	glTexParameteri(params.target, GL_TEXTURE_WRAP_T, params.WRAP);
	glTexParameteri(params.target, GL_TEXTURE_WRAP_R, params.WRAP);
	glGenerateMipmap(params.target);

}

void Texture::setSize(int w, int h){
	width = w;
	height = h;
}

void Texture::bind(){
	glBindTexture(params.target, id);
}

GLint Texture::activate(GLenum texture) {
	glActiveTexture(texture);
	glBindTexture(params.target, id);
	return texture - GL_TEXTURE0;
}

void Texture::cleanup(){
	glDeleteTextures(1, &id);
}

void Texture::unbind(GLuint target){
	glBindTexture(target, 0);
}
