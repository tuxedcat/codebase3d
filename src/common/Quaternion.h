#pragma once
#include <cmath>
#include <cassert>
#include "common/Config.h"
#include "common/Mat44.h"
#include "common/Float.h"

class Mat44;
class Quaternion{
public:
	float w,x,y,z;
	Quaternion():w(1),x(0),y(0),z(0){}
	Quaternion(float w,float x,float y,float z):w(w),x(x),y(y),z(z){}
	Quaternion(const Mat44& mat);
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

	Quaternion normalized()const{		
		float lsq=lensq();
		if(float_neq(lsq, 1))
			return *this/fsqrt(lsq);
		return *this/len();
	}
	void normalize(){ *this=normalized(); }
	Quaternion conjugate()const{ return {w,-x,-y,-z}; }
	float lensq()const{ return w*w + x*x + y*y + z*z; }
	float len()const{ return sqrtf(lensq()); }
	bool isUnit()const{ return fabs(lensq()-1)<eps; }

	//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Quaternion-derived_rotation_matrix
	//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
	Mat44 toRotationMatrix()const;
};