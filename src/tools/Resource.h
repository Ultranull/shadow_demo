#pragma once

#include <map>
#include <string>
#include <stdio.h>

#include <glad/glad.h>

#include"ShaderProgram.h"
#include"Material.h"

namespace bhtk {

	class Resource {
		std::map<std::string, Texture> textures;

		std::map<std::string, Shader> shaders;

	public:
		std::string path = "assets/", texturePath = "textures/", shaderPath = "shaders/";
		Resource() {}

		Texture addTexture(std::string name, const char *tex);
		void addTextures(std::string name, const char *tar, int sub_width, int sub_height, int ir, int ic);
		Texture getTexture(std::string name);
		GLint bindTexture(std::string name, GLuint sample);

		Shader addShader(std::string name);
		Shader getShader(std::string name);

		Texture LoadGLTexture(const char *filename);
		Texture LoadGLsubTexture(const char *filename, int sub_x, int sub_y, int sub_width, int sub_height);

		void setPath(std::string texturePath, std::string shaderPath, std::string path);
		std::string getShaderPath(std::string file);

		void cleanup();

		static Resource& getInstance() {
			static Resource instance;
			return instance;
		}
		Resource(Resource const&) = delete;
		void operator=(Resource const&) = delete;

	};

}