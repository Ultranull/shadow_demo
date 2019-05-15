#pragma once

#include <vector>

#include <glad/glad.h>
#include "glm/glm.hpp"

#include "VertexArray.h"

struct Vertex {
	glm::vec3 pos       = glm::vec3(0);
	glm::vec3 color     = glm::vec3(0);
	glm::vec2 UV        = glm::vec2(0);
	glm::vec3 normal    = glm::vec3(0);
};
class Mesh {

public:
	void bindmeshindexed();
	void bindmesh();
	VertexArray vaObject;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Mesh();
	Mesh(std::vector<Vertex>);
	Mesh(std::vector<Vertex>,std::vector<unsigned int>);
	~Mesh() {}

	void render(GLuint);
	void renderVertices(GLuint);
	void cleanup();
};

