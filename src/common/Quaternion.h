#pragma once
#include <cmath>
#include <cassert>
#include "common/Config.h"

class Quaternion{
public:
	float w,x,y,z;
	Quaternion(float w,float x,float y,float z):w(w),x(x),y(y),z(z){}
	Quaternion operator*(const Quaternion& o)const{
		return {
			w*o.w - x*o.x - y*o.y - z*o.z,
			w*o.x + x*o.w + y*o.z - z*o.y,
			w*o.y - x*o.z + y*o.w + z*o.x,
			w*o.z + x*o.y - y*o.x + z*o.w,
		};
	}

	Quaternion operator*(float s)const{ return {w*s, x*s, y*s, z*s}; }
	Quaternion operator/(float s)const{ return {w/s, x/s, y/s, z/s}; }

	Quaternion normalized()const{ return *this/len(); }
	Quaternion conjugate()const{ return {w,-x,-y,-z}; }
	float lensq()const{ return w*w + x*x + y*y + z*z; }
	float len()const{ return sqrtf(lensq()); }
};