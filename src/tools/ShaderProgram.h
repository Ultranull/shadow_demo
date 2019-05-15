#pragma once

#include <map>
#include <vector>
#include <string>
#include<glad/glad.h>
#include "glm/glm.hpp"

namespace bhtk {

	struct Shader {
		GLuint id;
		GLuint type;
		Shader() {}
		Shader(std::string file, GLuint t);
		void cleanup();
	};
	class Program {
		GLuint programID;
		Shader vertex, fragment, geometry;
	public:
		Program(Shader vert, Shader frag);
		Program(Shader vert, Shader frag, Shader geom);
		Program() {}
		~Program();

		void cleanup();

		void setUniform(std::string name, glm::vec2 *v);
		void setUniform(std::string name, glm::vec3 *v);
		void setUniform(std::string name, glm::mat4 *m);
		void setUniform(std::string name, float f);
		void setUniform(std::string name, int i);
		void setUniform(std::string name, unsigned int i);


		void bind();

		GLuint getProgramID();
	};

}