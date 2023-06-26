#include "GL/glut.h"
#include <cmath>
#include <iostream>
#include "GLFW/glfw3.h"
#include "game/entity/Entity.h"
#include "game/component/Mesh.h"
#include "game/entity/Camera.h"
#include "render/impl/ogl/RendererOGL.h"
#include <unistd.h>

using namespace std;

const int PI=acosf(-1);
Entity* entity_root;
Entity* cube;
Camera* camera;

void render(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	vector<RenderRequest> render_q;
	entity_root->draw(camera->world2camera(), render_q);
	RendererOGL().render(render_q);
}

#include <chrono>
void update(){
	using namespace std::chrono;
	static auto prev_time = steady_clock::now();
	auto cur_time = steady_clock::now();
	auto delta_time = duration_cast<duration<double>>(cur_time - prev_time).count();
	entity_root->update(delta_time);
	prev_time = cur_time;
	// std::cout<<"FPS: "<<int(1/delta_time)<<std::endl;
}

int main(){
	if(!glfwInit()){
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Game Engine", NULL, NULL);
	if(!window){
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glClearColor(0.6, 0.85, 1.0, 0.0);

	entity_root= new Entity();

	camera = new Camera(entity_root);
	camera->position({0,5,5});
	camera->rotate({1,0,0},-PI/2);
	
	cube = new Entity(entity_root);
	cube->LoadMesh("untitled.obj");
	cube->onUpdate=[](Entity*self, float delta_time){
		self->rotate({1,1,-1}, delta_time);
	};

	while(!glfwWindowShouldClose(window)){
		// Set viewport
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Set projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, static_cast<double>(width) / static_cast<double>(height), 0.1, 100.0);

		update();
		render();
	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}