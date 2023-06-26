#include "GL/glut.h"
#include <cmath>
#include <iostream>
#include "GLFW/glfw3.h"
#include "game/Entity.h"
#include "game/Mesh.h"
#include "game/Camera.h"
#include "render/impl/ogl/RendererOGL.h"
#include <unistd.h>

using namespace std;

const int PI=acosf(-1);
Entity* entity_root;
Entity* cube;
Camera* camera;

void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cube->rotate({1,1,1},0.01f);
	// camera->rotate({1,0,0},0.003f);
	
	vector<RenderRequest> render_q;
	entity_root->draw(camera->world2camera(), render_q);
	RendererOGL().render(render_q);
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
	// cube->position({0,0,-5});

	while(!glfwWindowShouldClose(window)){
		// Set viewport
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Set projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, static_cast<double>(width) / static_cast<double>(height), 0.1, 100.0);

		display();
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}