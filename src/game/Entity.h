#pragma once
#include <list>
#include <vector>
#include <string>
#include "common/Vec3.h"
#include "render/Renderer.h"
struct Mesh;
struct Entity{
public:
	Vec3 position;
	Vec3 scale;
	Vec3 lookat;// should be nonzero
	Entity(
		const Vec3& position={0,0,0},
		const Vec3& scale={1,1,1},
		const Vec3& lookat={0,0,-1}):
		position(position), scale(scale), lookat(lookat), mesh(nullptr){}
	~Entity();
	void draw(Mat44 parent2world, std::vector<RenderRequest>& render_q)const;
	void LoadMesh(const std::string& file_path);
private:
	std::list<Entity*> children;
	Mesh* mesh;
	Mat44 local2parent()const{
		Mat44 mscale={
			scale.x,0,0,0,
			0,scale.y,0,0,
			0,0,scale.z,0,
			0,0,0,1,
		};
		Mat44 mrotate={
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1,
		};
		Mat44 mposition={
			1,0,0,position.x,
			0,1,0,position.y,
			0,0,1,position.z,
			0,0,0,1,
		};
		return mposition*mrotate*mscale;
	}
};