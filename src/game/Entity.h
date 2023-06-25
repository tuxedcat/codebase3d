#pragma once
#include <list>
#include "common/Vec3.h"
struct Mesh;
struct Entity{
public:
	Entity(
		const Vec3& position={0,0,0},
		const Vec3& scale={1,1,1},
		const Vec3& lookat={0,0,-1}):
		position(position), scale(scale), lookat(lookat), mesh(nullptr){}
	Vec3 position;
	Vec3 scale;
	Vec3 lookat;// should be nonzero
	std::list<Entity*> children;
	Mesh* mesh;
};