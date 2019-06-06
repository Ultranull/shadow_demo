#pragma once

#include<glad/glad.h>
#include<vector>
#include<map>
#include<string>

#include "Material.h"

namespace bhtk {
	class FrameBuffer {
		GLuint id;
		GLuint width, height;
		std::map<std::string, Texture> textures;
		std::vector<GLenum> buffers;

	public:
		FrameBuffer();
		FrameBuffer(GLuint w, GLuint h);
		~FrameBuffer();

		void bind();
		void read();
		void draw();
		void addDepth();
		void addTexture2D(std::string name, GLuint internalformat, GLenum format, GLenum attachment);
		void addTexture3D(std::string name, GLuint internalformat, GLenum format, GLenum attachment);
		void drawBuffers();
		Texture getTexture(std::string name);
		void check();

		inline GLuint getWidth() { return width; }
		inline GLuint getHeight() { return height; }

		void cleanup();
		static void bindDefualt();
	};
}
