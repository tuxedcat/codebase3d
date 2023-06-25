#pragma once
#include <list>
#include <vector>
#include <string>
#include <assert.h>
#include "common/Vec3.h"
#include "render/Renderer.h"
struct Mesh;
struct Entity{
public:
	Entity(
		const Vec3& position={0,0,0},
		const Vec3& scale={1,1,1},
		const Vec3& lookat={0,0,-1}):
			_position(position),
			_scale(scale),
			_lookat(lookat),
			mesh(nullptr){
			assert(lookat.dot(lookat)==1);
		}
	~Entity();

	Vec3 position()const{return _position;}
	Vec3 scale()const{return _scale;}
	Vec3 lookat()const{assert(_lookat.dot(_lookat)==1); return _lookat;}
	void position(const Vec3& v){_position=v;}
	void scale(const Vec3& v){_scale=v;}
	void lookat(const Vec3& v){assert(v.dot(v)==1); _lookat=v;}

	void draw(Mat44 parent2world, std::vector<RenderRequest>& render_q)const;
	void LoadMesh(const std::string& file_path);
private:
	Vec3 _position;
	Vec3 _scale;
	Vec3 _lookat;
	std::list<Entity*> children;
	Mesh* mesh;
	Mat44 local2parent()const{
		Mat44 mscale={
			scale().x,0,0,0,
			0,scale().y,0,0,
			0,0,scale().z,0,
			0,0,0,1,
		};
		Mat44 mrotate={
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1,
		};
		Mat44 mposition={
			1,0,0,position().x,
			0,1,0,position().y,
			0,0,1,position().z,
			0,0,0,1,
		};
		return mposition*mrotate*mscale;
	}
};