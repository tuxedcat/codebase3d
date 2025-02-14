#include <iostream>
#include <limits>
#include <cmath>
#include <chrono>
#include <unistd.h>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


#include "game/entity/Entity.h"
#include "game/component/Mesh.h"
#include "game/event/Manager.h"
#include "render/RendererOGL.h"

using namespace std;

const float PI=acosf(-1);
GLFWwindow* window;
Entity* entity_root;
Entity* camera;
Renderer* renderer;

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
	renderer=new RendererOGL();

	entity_root= new Entity();
	camera = new Entity(entity_root);
	camera->name="camera";
	camera->position({-80,10,20});
	camera->rotate(glm::angleAxis(-PI/8,glm::vec3{1,0,0}));
	camera->onUpdate=[](Entity*self, float delta_time){
		self->position_acc(Vec3(delta_time*8,0,0));
	};
	static bool mouse_pressing=false;
	evt::Manager<evt::MouseMove>::addHandler([&](const evt::MouseMove& e){
		static double prev_x=numeric_limits<double>::quiet_NaN(), prev_y=numeric_limits<double>::quiet_NaN();
		if(mouse_pressing && isnan(prev_x)==false and isnan(prev_y)==false){
			float dx = e.x-prev_x;
			float dy = e.y-prev_y;
			camera->rotate_acc(glm::angleAxis((fabs(dx)+fabs(dy))/100,glm::vec3{dy,dx,0}));
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

	auto cube = Entity::loadFromFile("models/cube/cube.obj");
	cube->onUpdate=[](Entity*self, float delta_time){
		self->rotate_acc(glm::angleAxis(delta_time,glm::vec3{0,1,0}));
	};
	cube->position({-40,0,0});
	entity_root->adopt(cube);
	
	auto dv = Entity::loadFromFile("models/dancing_vampire/dancing_vampire.dae");
	dv->scale({0.05, 0.05, 0.05});
	dv->position({20,-2,0});
	dv->onUpdate=[](Entity*self, float delta_time){
		self->rotate_acc(glm::angleAxis(delta_time,glm::vec3{0,1,0}));
	};
	entity_root->adopt(dv);
	
	auto tcf = Entity::loadFromFile("models/toon-cat-free/model.gltf");
	tcf->scale({1.5, 1.5, 1.5});
	tcf->rotate(glm::angleAxis(PI/2,glm::vec3{0, 1, 0}));
	tcf->position({-20,0,0});
	tcf->onUpdate=[](Entity*self, float delta_time){
	};
	entity_root->adopt(tcf);

	auto fox = Entity::loadFromFile("models/fox/fox.gltf");
	fox->scale({0.1, 0.1, 0.1});
	fox->rotate(glm::angleAxis(PI/2,glm::vec3{1, 0, 0}));
	fox->onUpdate=[](Entity*self, float delta_time){
		self->rotate_acc(glm::angleAxis(delta_time*2,glm::vec3{0, 1, 0}));
	};
	entity_root->adopt(fox);
	
	auto dv2 = Entity::loadFromFile("models/dancing_vampire/dancing_vampire.dae");
	dv2->scale({0.5, 0.5, 0.5});
	dv2->position({200,0,0});
	dv2->onUpdate=[](Entity*self, float delta_time){
		self->rotate_acc(glm::angleAxis(delta_time*4,glm::vec3{0,1,0}));
	};
	dv->adopt(dv2);
	
	// auto koume = Entity::loadFromFile("models/koume/koume.fbx");
	// koume->position({5,5,0});
	// koume->scale({0.05, 0.05, 0.05});
	// koume->onUpdate=[](Entity*self, float delta_time){
	// 	self->rotate_acc({{0,1,0}, delta_time});
	// };
	// entity_root->adopt(koume);

	// auto backpack = Entity::loadFromFile("models/backpack/backpack.obj");
	// backpack->position({5,0,0});
	// entity_root->adopt(backpack);
}

void render(){
	if(!camera)
		throw "No camera selected";
	vector<RenderRequest> render_q;
	entity_root->draw(camera->world2local(), Mat44::identity(), render_q);
	renderer->render(render_q);
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
	// try{
		init();
		while(!glfwWindowShouldClose(window)){
			update();

			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			renderer->setViewport(width, height);
			renderer->setProjection(60.0, static_cast<double>(width) / static_cast<double>(height), 0.5, 1000.0);
			render();
		
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	// }catch(const char* err){
	// 	cout<<"Unhandled exception(const char*): "<<err<<endl;
	// 	throw err;
	// }catch(const string& err){
	// 	cout<<"Unhandled exception(string): "<<err<<endl;
	// 	throw err;
	// }
	glfwTerminate();
	return 0;
}