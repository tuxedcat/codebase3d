#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <assert.h>
#include "common/Vec3.h"
#include "render/Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct EntityAnim{
	std::string name;
	float duration;
	float ticksPerSecond;
	std::vector<std::pair<float,Vec3>> positions;
	std::vector<std::pair<float,glm::quat>> rotations;
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
		const glm::quat& rotate={1,0,0,0},
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
	glm::quat rotate()const{ return _rotate; }
	Vec3 scale()const{ return _scale; }
	void position(const Vec3& v){ _position=v; local2world_dirty=world2local_dirty=true; }
	void rotate(const glm::quat& q){ _rotate=glm::normalize(q); local2world_dirty=world2local_dirty=true; }
	void scale(const Vec3& v){ _scale=v; local2world_dirty=world2local_dirty=true; }
	void position_acc(const Vec3& v){ _position+=v; local2world_dirty=world2local_dirty=true; }
	void rotate_acc(glm::quat q){ _rotate=glm::normalize(q*_rotate); local2world_dirty=world2local_dirty=true; }
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

	Mat44 Quat2Mat44(glm::quat q)const{
		//Idk but conjugate required...
		auto m=glm::mat4_cast(glm::conjugate(q));
		return {m[0][0],m[0][1],m[0][2],m[0][3],m[1][0],m[1][1],m[1][2],m[1][3],m[2][0],m[2][1],m[2][2],m[2][3],m[3][0],m[3][1],m[3][2],m[3][3]};
		float s=1;
		auto [w,x,y,z]=q;
		Mat44 ret={
			1-2*s*(y*y+z*z), 2*s*(q.x*y-z*w), 2*s*(x*z+y*w), 0,
			2*s*(x*y+z*w), 1-2*s*(x*x+z*z), 2*s*(y*z-w*x), 0,
			2*s*(x*z-y*w), 2*s*(y*z+w*x), 1-2*s*(x*x+y*y), 0,
			0, 0, 0, 1,
		};
		// ret.printself();
		return ret;
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
		return mat_position*mat_scale*Quat2Mat44(_rotate);
	}
	Mat44 parent2local()const{
		Mat44 mscale={
			1/scale().x,0,0,0,
			0,1/scale().y,0,0,
			0,0,1/scale().z,0,
			0,0,0,1,
		};
		Mat44 mrotate=Quat2Mat44(_rotate).transposed();
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
	glm::quat _rotate;
	Vec3 _scale;
	Entity* _parent;
	std::list<Entity*> children;
	std::list<std::shared_ptr<Mesh>> meshes;
	std::list<EntityAnim> anims;
};