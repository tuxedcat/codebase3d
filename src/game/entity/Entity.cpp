#include "game/entity/Entity.h"
#include "game/component/Mesh.h"

Entity::~Entity(){
	for(auto child: children)
		delete child;
	if(mesh)
		delete mesh;
	mesh=nullptr;
}

void Entity::LoadMesh(const std::string& file_path){
	if(mesh)
		delete mesh;
	mesh = new Mesh(file_path,this);
}

void Entity::draw(Mat44 parent2world, std::vector<RenderRequest>& render_q)const{
	for(auto i: children)
		i->draw(parent2world, render_q);
	if(mesh)
		render_q.emplace_back(
			parent2world*local2parent(),
			mesh->primitive_type,
			mesh->vertices,
			mesh->normals,
			mesh->texcoord,
			mesh->faces,
			mesh->getMaterial()->getTextureID());
}

void Entity::update(float delta_time){
	if(onUpdate)
		onUpdate(this, delta_time);
	for(auto i:children)
		i->update(delta_time);
}