#include <iostream>
#include <limits>
#include <cmath>
#include <chrono>
#include <unistd.h>

#include "GLFW/glfw3.h"

#include "game/entity/Entity.h"
#include "game/component/Mesh.h"
#include "game/event/Manager.h"
#include "render/impl/ogl/RendererOGL.h"

using namespace std;

const float PI=acosf(-1);
GLFWwindow* window;
Entity* entity_root;
Entity* camera;

void init(){
	if(!glfwInit())
		throw "glfwInit() failed";
	window = glfwCreateWindow(1280, 720, "Game Engine", NULL, NULL);
	if(!window)
		throw "glfwCreateWindow() failed";
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos){
		evt::Manager<evt::MouseMove>::pushEvent({float(xpos), float(ypos)});
	});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods){
		switch(action){
		case GLFW_PRESS:
			switch(button){
			case GLFW_MOUSE_BUTTON_LEFT:
				evt::Manager<evt::MousePress>::pushEvent({evt::MouseButtonType::left});
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				evt::Manager<evt::MousePress>::pushEvent({evt::MouseButtonType::right});
				break;
			default:
				throw "Unknown button type";
			}
			break;
		case GLFW_RELEASE:
			switch(button){
			case GLFW_MOUSE_BUTTON_LEFT:
				evt::Manager<evt::MouseRelease>::pushEvent({evt::MouseButtonType::left});
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				evt::Manager<evt::MouseRelease>::pushEvent({evt::MouseButtonType::right});
				break;
			default:
				throw "Unknown button type";
			}
			break;
		default:
			throw "Unknown";
		}
	});

	glClearColor(0.6, 0.85, 1.0, 0.0);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	entity_root= new Entity();
	camera = new Entity(entity_root);
	camera->name="camera";
	camera->position({0,100,200});
	camera->rotate({1,0,0},-PI/8);
	static bool mouse_pressing=false;
	evt::Manager<evt::MouseMove>::addHandler([&](const evt::MouseMove& e){
		static double prev_x=numeric_limits<double>::quiet_NaN(), prev_y=numeric_limits<double>::quiet_NaN();
		if(mouse_pressing && isnan(prev_x)==false and isnan(prev_y)==false){
			float dx = e.x-prev_x;
			float dy = e.y-prev_y;
			camera->rotate(Vec3{dy,dx,0},(fabs(dx)+fabs(dy))/100);
		}
		prev_x=e.x;
		prev_y=e.y;
	});
	evt::Manager<evt::MousePress>::addHandler([&](const evt::MousePress& e){
		if(e.btnType==evt::MouseButtonType::left)
			mouse_pressing=true;
	});
	evt::Manager<evt::MouseRelease>::addHandler([&](const evt::MouseRelease& e){
		if(e.btnType==evt::MouseButtonType::left)
			mouse_pressing=false;
	});
	
	auto cube = Entity::loadFromFile("models/koume/koume.fbx");
	// auto cube = Entity::loadFromFile("models/cube/cube.obj");
	// auto cube = Entity::loadFromFile("models/backpack/backpack.obj");
	// cube->scale({0.1, 0.1, 0.1});
	cube->onUpdate=[](Entity*self, float delta_time){
		self->rotate({0,1,0}, delta_time);
	};
	entity_root->adopt(cube);
}

void render(){
	if(!camera)
		throw "No camera selected";
	auto world2camera=Mat44::identity();
	world2camera=world2camera*camera->parent2local();
	auto parent_iter=camera->parent();
	while(parent_iter){
		world2camera=world2camera*parent_iter->parent2local();
		parent_iter=parent_iter->parent();
	}

	vector<RenderRequest> render_q;
	entity_root->draw(world2camera, render_q);
	RendererOGL().render(render_q);
}

void update(){
	using namespace std::chrono;
	static auto prev_time = steady_clock::now();
	auto cur_time = steady_clock::now();
	auto delta_time = duration_cast<duration<double>>(cur_time - prev_time).count();
	evt::Manager<evt::MouseMove>::patchEvents();
	evt::Manager<evt::MousePress>::patchEvents();
	evt::Manager<evt::MouseRelease>::patchEvents();
	entity_root->update(delta_time);
	prev_time = cur_time;
	// std::cout<<"FPS: "<<int(1/delta_time)<<std::endl;
}

int main(){
	try{
		init();
		while(!glfwWindowShouldClose(window)){
			// Set viewport
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);

			// Set projection
			//https://www.khronos.org/opengl/wiki/GluPerspective_code
			auto glhPerspectivef2=[](float* matrix, float fovyInDegrees, float aspectRatio, float znear, float zfar){
				float ymax, xmax;
				ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
				xmax = ymax * aspectRatio;
				auto glhFrustumf2=[](float* mtx, float left, float right, float bottom, float top, float znear, float zfar){
					float temp1, temp2, temp3, temp4;
					temp1 = 2.0 * znear;
					temp2 = right - left;
					temp3 = top - bottom;
					temp4 = zfar - znear;
					mtx[0] = temp1 / temp2;
					mtx[1] = mtx[2] = mtx[3] = mtx[4] = 0.0;
					mtx[5] = temp1 / temp3;
					mtx[6] = mtx[7] = 0.0;
					mtx[8] = (right + left) / temp2;
					mtx[9] = (top + bottom) / temp3;
					mtx[10] = (-zfar - znear) / temp4;
					mtx[11] = -1.0;
					mtx[12] = mtx[13] = 0.0;
					mtx[14] = (-temp1 * zfar) / temp4;
					mtx[15] = 0.0;
				};
				glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
			};
			Mat44 projection;
			glhPerspectivef2((float*)projection.a, 60.0, static_cast<double>(width) / static_cast<double>(height), 0.1, 1000.0);
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf((float*)projection.a);

			update();
			render();
		
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}catch(const char* err){
		cout<<"Unhandled exception(const char*): "<<err<<endl;
		throw err;
	}catch(const string& err){
		cout<<"Unhandled exception(string): "<<err<<endl;
		throw err;
	}
	glfwTerminate();
	return 0;
}