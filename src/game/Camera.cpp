#include "game/Camera.h"
#include "Entity.h"

Mat44 Camera::world2camera()const{
	auto ret=Mat44::identity();
	ret=ret*parent2local();
	auto parent_iter=parent();
	while(parent_iter){
		ret=ret*parent_iter->parent2local();
		parent_iter=parent_iter->parent();
	}
	return ret;
}