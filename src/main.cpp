#include <fstream>
#include <sstream>

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "tools/App.h"
#include "tools/Camera.h"
#include "tools/Mesh.h"
#include "tools/Resource.h"
#include "tools/ShaderProgram.h"
#include "tools/FrameBuffer.h"


using namespace glm;
using namespace std;



Mesh loadOBJ(string fn) {

	vector<vec3> verts_;
	vector<vec3> norms_;
	vector<vec2> uv_;
	vector<Vertex> verts;

	ifstream in;
	in.open(fn.c_str(), ifstream::in);
	if (in.fail()) return Mesh();
	string line;
	while (!in.eof()) {
		getline(in, line);
		istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			vec3 v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			vec3 n;
			for (int i = 0; i < 3; i++) iss >> n[i];
			norms_.push_back(n);
		}
		else if (!line.compare(0, 3, "vt ")) {
			iss >> trash >> trash;
			vec2 uv;
			for (int i = 0; i < 2; i++) iss >> uv[i];
			uv_.push_back(vec2(uv.x, 1 - uv.y));
		}
		else if (!line.compare(0, 2, "f ")) {
			vector<ivec3> f;
			int vert, uv, norm;
			iss >> trash;
			while (iss >> vert >> trash >> uv >> trash >> norm) {
				vert--; uv--; norm--;
				verts.push_back(Vertex{ verts_[vert],{1,1,1} ,uv_[uv], norms_[norm] });
			}
		}
	}
	return Mesh(verts);
}

void dumpMesh(Mesh& mesh, string fn) {
	ofstream out(fn.c_str(), ios::binary);
	size_t size = mesh.vertices.size();
	out.write((char*)& size, sizeof(size));
	for (int i = 0; i < mesh.vertices.size(); i++) {
		Vertex v = mesh.vertices[i];
		out.write((char*)& v, sizeof(v));
	}
	out.close();
}

Mesh loadMesh(string fn) {
	vector<Vertex> verts;
	ifstream in(fn.c_str(), ios::binary);
	if (!in) {
		string obj = fn;
		int dot = obj.find('.');
		obj.replace(obj.begin() + dot + 1, obj.end(), "obj");
		Mesh m = loadOBJ(obj);
		dumpMesh(m, fn);
		return m;
	}
	size_t size;
	in.read((char*)& size, sizeof(size));
	for (int i = 0; i < size; i++) {
		Vertex v;
		in.read((char*)& v, sizeof(v));
		verts.push_back(v);
	}
	in.close();
	return Mesh(verts);
}

class Game :public App {
	Resource *R;
	Mesh mesh,geom,box,lamp,plane;

	Camera cam;
	Program basic,uvmaped,depthpass,simple;
	FrameBuffer depth;
	FrameBuffer depthPoint;

	vec3 lightpos = vec3(2, 2, 2);
	vec3 meshpos = vec3(5);

	UniformBuffer cBuffer;

	void initGL() {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(.1, .1, .1, 1);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_CULL_FACE);
	}

	void loading() {
	}

	void init() {
		R = &Resource::getInstance();

		basic = Program(R->addShader("vertex.vert"), R->addShader("fragment.frag"));
		simple = Program(R->addShader("simple.vert"), R->addShader("simple.frag"));
		uvmaped = Program(R->addShader("uvmaped.vert"), R->addShader("uvmaped.frag"));
		depthpass = Program(R->addShader("depthPoint.vert"), R->addShader("depthPoint.frag"), R->addShader("depthPoint.geom"));
		R->addTexture("uvmap", "uvmap.png");
		R->addTexture("gridmap", "gridmap.png");
		mesh = loadMesh(R->path + "knot.mesh");
		geom = loadMesh(R->path + "monkey.mesh");
		box= loadMesh(R->path + "insideout_cube.mesh");
		lamp= loadMesh(R->path + "sphere.mesh");
		plane = loadMesh(R->path + "plane.mesh");
		
		cam.perspective(window, 45, .1, 100);


		cBuffer = UniformBuffer();
		cBuffer.bind();
		cBuffer.setData<mat4>(2, GL_DYNAMIC_DRAW);
		cBuffer.setSubData(0, sizeof(glm::mat4), glm::value_ptr(cam.P()));
		cBuffer.setSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam.V()));
		cBuffer.blockBinding(basic.getProgramID(), 0, "camera");
		cBuffer.unbind();

		depthPoint = FrameBuffer(900, 900);
		depthPoint.addTexture3D("depth", GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
		depthPoint.drawBuffers();
		depthPoint.check();
	}

	void onClose() {
		R->cleanup(); 
	}

	void update(float delta) {
		cam.perspective(window, 45, .1, 100);
		//cam.orbit(window, delta, {0,0,0});
		cam.apply(window, delta);

		cBuffer.bind();
		cBuffer.setSubData(0, sizeof(glm::mat4), glm::value_ptr(cam.P()));
		cBuffer.setSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam.V()));
		cBuffer.unbind();

		//printf("%d\n", fps);
		glfwSetWindowTitle(window, to_string(fps).c_str());

	}
	void scene(Program prog,bool renderpass=true){
		prog.setUniform("uvmap", R->bindTexture("uvmap", GL_TEXTURE1));

		mat4 rot = rotate(ticks*2.f, vec3(1));
		mat4 model = translate(vec3(cos(ticks),sin(ticks), (cos(ticks) + sin(ticks))/2.)*7.f)* rot;
		prog.setUniform("model", &model);
		geom.renderVertices(GL_TRIANGLES);


		if (!renderpass) {
			model = inverse(cam.V()) * rotate(radians(180.f), vec3(0., 1., 0.));
			prog.setUniform("model", &model);
			geom.renderVertices(GL_TRIANGLES);
		}

		prog.setUniform("model", &mat4(1));
		mesh.renderVertices(GL_TRIANGLES);

		prog.setUniform("uvmap", R->bindTexture("gridmap", GL_TEXTURE1));
		prog.setUniform("model", &scale(vec3(15)));
		if (renderpass) {
			box.renderVertices(GL_TRIANGLES);

		}
	}
	void render(float delta) {
		depthPoint.bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		depthpass.bind();
		float near_plane = .10f;
		float far_plane = 100.0f;
		mat4 shadowP = perspective(radians(90.f), (float)depth.getWidth() / depth.getHeight(), near_plane, far_plane);
		vector<mat4> shadowV = {
			shadowP*lookAt(lightpos,lightpos + vec3( 1.f, 0.f, 0.f),vec3(0.f,-1.f, 0.f)),
			shadowP*lookAt(lightpos,lightpos + vec3(-1.f, 0.f, 0.f),vec3(0.f,-1.f, 0.f)),
			shadowP*lookAt(lightpos,lightpos + vec3(0.f, 1.f, 0.f),vec3(0.f, 0.f, 1.f)),
			shadowP*lookAt(lightpos,lightpos + vec3(0.f,-1.f, 0.f),vec3(0.f, 0.f,-1.f)),
			shadowP*lookAt(lightpos,lightpos + vec3(0.f, 0.f, 1.f),vec3(0.f,-1.f, 0.f)),
			shadowP*lookAt(lightpos,lightpos + vec3(0.f, 0.f,-1.f),vec3(0.f,-1.f, 0.f)),
		};


		for (int i = 0; i < 6; i++)
			depthpass.setUniform("shadowmats[" + to_string(i) + "]", &shadowV[i]);

		depthpass.setUniform("far_plane", far_plane);
		depthpass.setUniform("lightPos", &lightpos);

		scene(depthpass,0);


		FrameBuffer::bindDefualt();
		viewportinit(window);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		uvmaped.bind();
		uvmaped.setUniform("lightPos", &lightpos);
		uvmaped.setUniform("viewPos", &cam.getPosition());
		uvmaped.setUniform("far_plane", far_plane);
		Texture d = depthPoint.getTexture("depth");
		uvmaped.setUniform("depthmap", d.activate(GL_TEXTURE0));

		scene(uvmaped);

		basic.bind();
		basic.setUniform("model", &(translate(lightpos)*scale(vec3(.25))));
		lamp.renderVertices(GL_TRIANGLES);



		simple.bind();
		mat4 model = translate(meshpos) * translate(-cam.getPosition()) * inverse(cam.V())* rotate(radians(90.f), vec3(1, 0, 0));// rotate(radians(90.f), vec3(1, 0, 0))* translate(vec3(.5))* scale(vec3(.25));
		simple.setUniform("model", &model);
		simple.setUniform("uvmap", d.activate(GL_TEXTURE0));
		plane.renderVertices(GL_TRIANGLES);
	}

	void inputListener(float delta) {
		running = glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			lightpos = cam.getPosition();
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			meshpos = cam.getPosition();
	}
public:

	Game(int width, int height, const char *title) :App(width, height, title) {

	}
};

int main() {
	Game game(700, 700, "");
	game.start();
	return 0;
}