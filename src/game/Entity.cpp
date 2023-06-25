#include "game/Entity.h"
#include "game/Mesh.h"

Entity::~Entity(){
	for(auto child: children)
		delete child;
	if(mesh)
		delete mesh;
	mesh=nullptr;
}

void Entity::draw(Mat44 parent2world, std::vector<RenderRequest>& render_q)const{
	for(auto i: children)
		i->draw(parent2world, render_q);
	render_q.emplace_back(parent2world*local2parent(), mesh->primitive_type, mesh->vertices, mesh->faces);
}

void Entity::LoadMesh(const std::string& file_path){
	if(mesh)
		delete mesh;
	mesh = new Mesh(file_path,this);
}