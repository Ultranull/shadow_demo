#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <sstream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace bhtk;

string readFile(const char *file) {//FIX! place in a header as a util
	string content;
	ifstream stream(file);
	if (stream.is_open()) {
		stringstream ss;
		ss << stream.rdbuf();
		content = ss.str();
		stream.close();
	}
	else {
		printf("Failed to open %s\n", file);
		return "";
	}
	return content;
}
bool compileshader(const char* file, GLuint id) {
	GLint result = GL_FALSE;
	int infoLogLength;
	printf("Compiling shader: %s\n", file);
	string content = readFile(file);
	if (content.compare("") == 0) { return NULL; }
	char const * src = content.c_str();
	const GLint length = content.size();
	glShaderSource(id, 1, &src, &length);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetShaderInfoLog(id, infoLogLength, NULL, &errormessage[0]);
		printf("%s compile error:\n\t%s\n", file, &errormessage[0]);
		return false;
	}
	return true;
}

GLuint loadshaders(const char *vertexfile, const char *fragmentfile) {
	GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int infoLogLength;

	if (!compileshader(vertexfile, vertexID)) { return NULL; }
	if (!compileshader(fragmentfile, fragmentID)) { return NULL; }

	printf("linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errormessage[0]);
		printf("link error:\n%s\n", &errormessage[0]);
		return NULL;
	}

	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragmentID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

	return programID;
}
GLuint loadshaders(const char *vertexfile, const char *fragmentfile, const char *geometryfile) {
	GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometryID = glCreateShader(GL_GEOMETRY_SHADER);

	GLint result = GL_FALSE;
	int infoLogLength;

	if (!compileshader(vertexfile, vertexID)) { return NULL; }
	if (!compileshader(fragmentfile, fragmentID)) { return NULL; }
	if (!compileshader(geometryfile, geometryID)) { return NULL; }

	printf("linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	glAttachShader(programID, geometryID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errormessage[0]);
		printf("link error:\n%s\n", &errormessage[0]);
		return NULL;
	}

	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragmentID);
	glDetachShader(programID, geometryID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	glDeleteShader(geometryID);

	return programID;
}
	Texture Resource::addTexture(string name, const char *tex) {
		string fn = (path + texturePath + string(tex));
		const char *file = fn.c_str();
		Texture t = LoadGLTexture(file);
		printf("setting texture: %s\n", name.c_str());
		textures.insert(make_pair(name, t));
		return t;
	}

	void Resource::addTextures(string name, const char *tar, int sub_width, int sub_height, int ir, int ic) {
		const char *file = (path + texturePath + tar).c_str();
		for (int i = 0;i < ic;i++)
			for (int c = 0;c < ir;c++) {
				stringstream n;
				n << name << "-"<<i<<","<<c;
				Texture t = LoadGLsubTexture(file, c*sub_width, i*sub_height, sub_width, sub_height);
				printf("setting texture: %s\n", n.str().c_str());
				textures.insert(make_pair(n.str(), t));
			}

	}
	Texture Resource::getTexture(string name) {
		return textures[name];
	}

	GLint Resource::bindTexture(std::string name, GLuint sample){
		return getTexture(name).activate(sample);
	}

	Shader Resource::addShader(string file) {
		GLuint type;
		string ext = file.substr(file.size() - 4, 4);
		if (ext.compare("vert") == 0)
			type = GL_VERTEX_SHADER;
		else if(ext.compare("frag") == 0)
			type = GL_FRAGMENT_SHADER;
		Shader sh(path + shaderPath + file, type);
		shaders.insert({ file,sh });
		return sh;
	}
	Shader Resource::getShader(string name) {
		return shaders[name];
	}

	Texture Resource::LoadGLTexture(const char *filename) {
		printf("loading texture: %s\n", filename);
		//glEnable(GL_TEXTURE_2D);

		Texture t;
		t.bind();
		t.load();
		int w, h, bpp;
		unsigned char *data = stbi_load(filename, &w, &h, &bpp, 0);
		if (data == NULL) {
			printf("error loading texture %s!\n", filename);
			getchar();
			exit(-1);
		}
		t.setSize(w, h);
		if (bpp == 3) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(t.params.target, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (bpp == 4)
			glTexImage2D(t.params.target, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		Texture::unbind();
		return t;
	}
	Texture Resource::LoadGLsubTexture(const char *filename, int sub_x, int sub_y, int sub_width, int sub_height) {
		printf("loading texture: %s\n", filename);
		glEnable(GL_TEXTURE_2D);

		unsigned char *data;
		int width, height;

		Texture t;
		t.bind();
		t.load();

		data = stbi_load(filename, &width, &height, 0, STBI_rgb_alpha);
		if (data == NULL) {
			printf("error loading texture %s!\n", filename);
			getchar();
			exit(-1);
		}
		t.setSize(width, height);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
		char *subimg = (char*)data + (sub_x + sub_y * width) * 4;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sub_width, sub_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, subimg);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		stbi_image_free(data);
		Texture::unbind();
		return t;
	}

	void Resource::setPath(string texturePath, string shaderPath, string path="assets/"){
		this->path = path;
		this->texturePath = texturePath;
		this->shaderPath = shaderPath;
	}

	string Resource::getShaderPath(std::string file){
		return (path+shaderPath+file);
	}

	void Resource::cleanup() {
		map<string,Texture>::iterator textit = textures.begin();
		map<string,Shader>::iterator shadit = shaders.begin();
		for (;textit != textures.end();textit++) {
			textit->second.cleanup();
		}
		for (;shadit != shaders.end();shadit++) {
			shadit->second.cleanup();
		}
	}
