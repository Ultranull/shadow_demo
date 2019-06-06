#include "FrameBuffer.h"

using namespace bhtk;

FrameBuffer::FrameBuffer():FrameBuffer(500,500){
}

FrameBuffer::FrameBuffer(GLuint w, GLuint h):width(w),height(h) {
	glGenFramebuffers(1, &id);
}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, width, height);
}

void FrameBuffer::read(){
	glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
}

void FrameBuffer::draw(){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
}

void FrameBuffer::addDepth(){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::addTexture2D(std::string name, GLuint internalformat, GLenum format,GLenum attachment){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	TextureTraits params = RGBA_2D;
	params.internalformat = internalformat;
	params.format = format;
	Texture text (params);
	text.bind();
	text.load();
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, text.id, 0);
	textures.insert(std::make_pair(name, text));
	if(attachment!= GL_DEPTH_ATTACHMENT)
		buffers.push_back(attachment);
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::addTexture3D(std::string name, GLuint internalformat, GLenum format, GLenum attachment) {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	TextureTraits params = DEPTH_3D;
	params.internalformat = internalformat;
	params.format = format;
	Texture text(params);
	text.bind();
	text.load(); 
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat,
			width, height, 0, format, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, text.id, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, attachment, text.id, 0);
	textures.insert(std::make_pair(name, text));
	if (attachment != GL_DEPTH_ATTACHMENT)
		buffers.push_back(attachment);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::drawBuffers(){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (buffers.empty())
		glDrawBuffer(GL_NONE);
	else
		glDrawBuffers(buffers.size(), &buffers[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture FrameBuffer::getTexture(std::string name){
	return textures[name];
}

void FrameBuffer::check(){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("fb failed");
		getchar();
		exit(-1);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::cleanup(){
	glDeleteFramebuffers(1, &id);
	std::map<std::string, Texture>::iterator textit = textures.begin();
	for (; textit != textures.end(); textit++) {
		textit->second.cleanup();
	}
}

void FrameBuffer::bindDefualt(){
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}
