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
		const Vec3& lookat={0,0,1},
		const Vec3& up={0,1,0}):
			_position(position),
			_scale(scale),
			_lookat(lookat),
			_up(up),
			mesh(nullptr){
			assert(lookat.isUnit());
		}
	~Entity();

	Vec3 position()const{return _position;}
	Vec3 scale()const{return _scale;}
	Vec3 lookat()const{
		assert(_lookat.isUnit());
		assert(_lookat.isOrtho(_up));
		return _lookat;
	}
	Vec3 up()const{
		assert(_up.isUnit());
		assert(_up.isOrtho(_lookat));
		return _up;
	}
	Vec3 right()const{
		assert(_lookat.isOrtho(_up));
		return _lookat.cross(_up);
	}
	void position(const Vec3& v){_position=v;}
	void scale(const Vec3& v){_scale=v;}
	void rotate(const Vec3& axis, float angle){
		_lookat = _lookat.rotated(axis,angle);
		_up = _up.rotated(axis,angle);
	}

	void draw(Mat44 parent2world, std::vector<RenderRequest>& render_q)const;
	void LoadMesh(const std::string& file_path);
private:
	Vec3 _position;
	Vec3 _scale;
	Vec3 _lookat;
	Vec3 _up;
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
			right().x,right().y,right().z,0,
			up().x,up().y,up().z,0,
			lookat().x,lookat().y,lookat().z,0,
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