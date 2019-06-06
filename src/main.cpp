#include <fstream>
#include <sstream>

#include "tools/BluehatToolkit.h"
#include <glm/gtc/type_ptr.hpp>


using namespace glm;
using namespace std;
using namespace bhtk;



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

class Game :public App {
	Resource *R;
	Mesh mesh,geom,box;

	Camera cam;
	Program simple,uvmaped,depthpass;
	FrameBuffer depth;

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

		simple = Program(R->addShader("vertex.vert"), R->addShader("fragment.frag"));
		uvmaped = Program(R->addShader("uvmaped.vert"), R->addShader("uvmaped.frag"));
		depthpass = Program(R->addShader("depth.vert"), R->addShader("depth.frag"));
		R->addTexture("uvmap", "uvmap.png");
		R->addTexture("gridmap", "gridmap.png");
		mesh = loadOBJ(R->path + "knot.obj");
		geom = loadOBJ(R->path + "monkey.obj");
		box= loadOBJ(R->path + "insideout_cube.obj");
		
		
		cam.perspective(window, 45, .1, 100);


		cBuffer = UniformBuffer();
		cBuffer.bind();
		cBuffer.setData<mat4>(2, GL_DYNAMIC_DRAW);
		cBuffer.setSubData(0, sizeof(glm::mat4), glm::value_ptr(cam.P()));
		cBuffer.setSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam.V()));
		cBuffer.blockBinding(simple.getProgramID(), 0, "camera");
		cBuffer.unbind();

		depth = FrameBuffer(700, 700);
		depth.addTexture2D("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
		depth.drawBuffers();
		depth.check();
	}

	void onClose() {
		R->cleanup(); 
	}

	void update(float delta) {
		cam.perspective(window, 45, .1, 100);
		cam.orbit(window, delta, {0,0,0});

		cBuffer.bind();
		cBuffer.setSubData(0, sizeof(glm::mat4), glm::value_ptr(cam.P()));
		cBuffer.setSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam.V()));
		cBuffer.unbind();

		//printf("%d\n", fps);
		glfwSetWindowTitle(window, to_string(fps).c_str());

	}
	void scene(Program prog,bool renderpass=true){
		uvmaped.setUniform("uvmap", R->bindTexture("uvmap", GL_TEXTURE1));

		mat4 model = rotate(-ticks, vec3(-1, 1, 0))*translate(vec3(5, 5, 0));
		prog.setUniform("model", &model);
		geom.renderVertices(GL_TRIANGLES);

		model = rotate(-ticks, vec3(-1, 1, 0))*translate(vec3(-5, -5, 0));
		prog.setUniform("model", &model);
		if(renderpass)
			geom.renderVertices(GL_TRIANGLES);

		prog.setUniform("model", &mat4(1));
		mesh.renderVertices(GL_TRIANGLES);

		uvmaped.setUniform("uvmap", R->bindTexture("gridmap", GL_TEXTURE1));
		prog.setUniform("model", &scale(vec3(10)));
		box.renderVertices(GL_TRIANGLES);
	}
	void render(float delta) {
		depth.bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		depthpass.bind();

		mat4 depthP =perspective(radians(90.f), (float)depth.getWidth() / depth.getHeight(), 1.f, 1000.f);
		mat4 depthV = lookAt(vec3(8), vec3(0, 0, 0), vec3(0, 1, 0));

		depthpass.setUniform("projection", &depthP);
		depthpass.setUniform("view", &depthV);

		scene(depthpass,0);


		FrameBuffer::bindDefualt();
		viewportinit(window);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		uvmaped.bind();
		Texture d = depth.getTexture("depth");
		uvmaped.setUniform("depthmap", d.activate(GL_TEXTURE0));
		glm::mat4 biasMatrix = (depthP*depthV);
		uvmaped.setUniform("bias", &biasMatrix);

		scene(uvmaped);
	}

	void inputListener(float delta) {
		running = glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS;
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