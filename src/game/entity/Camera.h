#pragma once
#include "game/entity/Entity.h"

//Initially, it looks at (0,0,-1)
struct Camera: public Entity{
	Camera(
		Entity* parent=nullptr,
		const Vec3& position={0,0,0},
		const Quaternion& rotate={1,0,0,0},
		const Vec3& scale={1,1,1}):
		Entity(parent,position,rotate,scale){}
	Mat44 world2camera()const;
};