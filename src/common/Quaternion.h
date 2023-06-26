#pragma once
#include <cmath>
#include <cassert>
#include "common/Config.h"
#include "common/Mat44.h"

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

	Quaternion& normalize(){ auto l=len(); w/=l,x/=l,y/=l,z/=l; return *this;}
	Quaternion normalized()const{ return *this/len(); }
	Quaternion conjugate()const{ return {w,-x,-y,-z}; }
	float lensq()const{ return w*w + x*x + y*y + z*z; }
	float len()const{ return sqrtf(lensq()); }
	bool isUnit()const{ return fabs(lensq()-1)<eps; }

	//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Quaternion-derived_rotation_matrix
	//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
	Mat44 toRotationMatrix()const{
		assert(isUnit());
		float s=1/lensq();
		return {
			1-2*s*(y*y+z*z), 2*s*(x*y-z*w), 2*s*(x*z+y*w), 0,
			2*s*(x*y+z*w), 1-2*s*(x*x+z*z), 2*s*(y*z-w*x), 0,
			2*s*(x*z-y*w), 2*s*(y*z+w*x), 1-2*s*(x*x+y*y), 0,
			0, 0, 0, 1,
		};
	}
};