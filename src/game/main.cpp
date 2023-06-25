#include "GL/glut.h"
#include <cmath>
#include "GLFW/glfw3.h"
#include "game/Mesh.h"

using namespace std;

const double PI = 3.14159265358979323846;
void drawSphere(int slices, int stacks){
	for (int i = 0; i < slices; ++i){
		double lat0 = PI * (-0.5 + static_cast<double>(i) / slices);
		double lat1 = PI * (-0.5 + static_cast<double>(i + 1) / slices);
		double y0 = sin(lat0);
		double y1 = sin(lat1);
		double cosLat0 = cos(lat0);
		double cosLat1 = cos(lat1);
		
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= stacks; ++j){
			double lng = 2 * PI * static_cast<double>(j) / stacks;
			double x = cos(lng);
			double z = sin(lng);
			
			glNormal3f(x * cosLat0, y0, z * cosLat0);
			glTexCoord2f(static_cast<double>(j) / stacks, static_cast<double>(i) / slices);
			glVertex3f(x * cosLat0, y0, z * cosLat0);
			
			glNormal3f(x * cosLat1, y1, z * cosLat1);
			glTexCoord2f(static_cast<double>(j) / stacks, static_cast<double>(i + 1) / slices);
			glVertex3f(x * cosLat1, y1, z * cosLat1);
		}
		glEnd();
	}
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,-10);
	drawSphere(8, 8);
	Mesh("untitled.obj");
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