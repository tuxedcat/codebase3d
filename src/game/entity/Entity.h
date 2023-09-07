#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <assert.h>
#include "common/Vec3.h"
#include "common/Quaternion.h"
#include "render/Renderer.h"

struct EntityAnim{
	std::string name;
	float duration;
	float ticksPerSecond;
	std::vector<std::pair<float,Vec3>> positions;
	std::vector<std::pair<float,Quaternion>> rotations;
	std::vector<std::pair<float,Vec3>> scales;
};

struct Mesh;
struct Entity{
	static Entity* loadFromFile(const std::string& model_path);
public:
	std::string name;
	Entity(
		Entity* parent=nullptr,
		const Vec3& position={0,0,0},
		const Quaternion& rotate={1,0,0,0},
		const Vec3& scale={1,1,1}):
		_position(position),
		_rotate(rotate),
		_scale(scale),
		_parent(parent){
		if(parent)
			parent->adopt(this);
	}
	~Entity();

	Vec3 position()const{ return _position; }
	Quaternion rotate()const{ return _rotate; }
	Vec3 scale()const{ return _scale; }
	void position(const Vec3& v){ _position=v; local2world_dirty=world2local_dirty=true; }
	void rotate(const Quaternion& q){ _rotate=q; local2world_dirty=world2local_dirty=true; }
	void scale(const Vec3& v){ _scale=v; local2world_dirty=world2local_dirty=true; }
	void position_acc(const Vec3& v){ _position+=v; local2world_dirty=world2local_dirty=true; }
	void rotate_acc(Quaternion q){ _rotate=q*_rotate; local2world_dirty=world2local_dirty=true; }
	void scale_acc(const Vec3& v){ _scale*=v; local2world_dirty=world2local_dirty=true; }

	Entity* parent()const{return _parent;}
	void adopt(Entity* child){
		assert(child);
		children.push_back(child);
		child->_parent=this;
	}
	void abandon(Entity* child){
		assert(child);
		children.erase(std::find(children.begin(),children.end(),child));
		child->_parent=nullptr;
	}
	void detachFromParent(){
		if(!_parent)
			return;
		_parent->abandon(this);
	}

	Mat44 local2parent()const{
		Mat44 mat_scale={
			scale().x,0,0,0,
			0,scale().y,0,0,
			0,0,scale().z,0,
			0,0,0,1,
		};
		Mat44 mat_position={
			1,0,0,position().x,
			0,1,0,position().y,
			0,0,1,position().z,
			0,0,0,1,
		};
		return mat_position*mat_scale*_rotate.toRotationMatrix();
	}
	Mat44 parent2local()const{
		Mat44 mscale={
			1/scale().x,0,0,0,
			0,1/scale().y,0,0,
			0,0,1/scale().z,0,
			0,0,0,1,
		};
		Mat44 mrotate=_rotate.toRotationMatrix().transposed();
		Mat44 mposition={
			1,0,0,-position().x,
			0,1,0,-position().y,
			0,0,1,-position().z,
			0,0,0,1,
		};
		return mscale*mrotate*mposition;
	}
	mutable bool local2world_dirty=true;
	mutable Mat44 local2world_memo;
	Mat44 local2world()const{
		if(local2world_dirty){
			local2world_memo=_parent?_parent->local2world()*local2parent():local2parent();
			local2world_dirty=false;
		}
		return local2world_memo;
	}
	mutable bool world2local_dirty=true;
	mutable Mat44 world2local_memo;
	Mat44 world2local()const{
		if(world2local_dirty){
			world2local_memo=_parent?parent2local()*_parent->world2local():parent2local();
			world2local_dirty=false;
		}
		return world2local_memo;
	}

	void draw(const Mat44& world2camera, Mat44 parent2world, std::vector<RenderRequest>& render_q)const;
	void update(float delta_time);

	void(*onUpdate)(Entity* self, float delta_time)=nullptr;

	void addMesh(const std::shared_ptr<Mesh>& mesh){meshes.emplace_back(mesh);}
private:
	Vec3 _position;
	Quaternion _rotate;
	Vec3 _scale;
	Entity* _parent;
	std::list<Entity*> children;
	std::list<std::shared_ptr<Mesh>> meshes;
	std::list<EntityAnim> anims;
};