#pragma once
#include <list>
#include <vector>
#include <string>
#include <assert.h>
#include "common/Vec3.h"
#include "common/Quaternion.h"
#include "render/Renderer.h"
struct Mesh;
struct Entity{
public:
	Entity(
		const Vec3& position={0,0,0},
		const Quaternion& rotate={1,0,0,0},
		const Vec3& scale={1,1,1}):
			_position(position),
			_rotate(rotate),
			_scale(scale),
			mesh(nullptr){}
	~Entity();

	Vec3 position()const{return _position;}
	Quaternion rotate()const{return _rotate;}
	Vec3 scale()const{return _scale;}
	void position(const Vec3& v){_position=v;}
	void rotate(const Vec3& axis, float angle){
		auto sin_ha = sin(angle/2);
		auto cos_ha = cos(angle/2);
		_rotate=(Quaternion(cos_ha, axis.x*sin_ha, axis.y*sin_ha, axis.z*sin_ha)*_rotate).normalized();
	}
	// void rotate(const Quaternion& q){
	// }
	void scale(const Vec3& v){_scale=v;}

	void draw(Mat44 parent2world, std::vector<RenderRequest>& render_q)const;
	void LoadMesh(const std::string& file_path);
private:
	Vec3 _position;
	Quaternion _rotate;
	Vec3 _scale;
	std::list<Entity*> children;
	Mesh* mesh;
	Mat44 local2parent()const{
		Mat44 mscale={
			scale().x,0,0,0,
			0,scale().y,0,0,
			0,0,scale().z,0,
			0,0,0,1,
		};
		Mat44 mrotate=_rotate.toRotationMatrix();
		Mat44 mposition={
			1,0,0,position().x,
			0,1,0,position().y,
			0,0,1,position().z,
			0,0,0,1,
		};
		return mposition*mrotate*mscale;
	}
};