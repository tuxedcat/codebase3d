#pragma once
#include <cmath>
#include "common/Quaternion.h"
#include "common/Float.h"

class Vec3{
public:
	float a[3]{},&x,&y,&z;
	static Vec3 zero(){ return {0,0,0}; }
	static Vec3 one(){ return {1,1,1}; }
	Vec3():Vec3(0,0,0){}
	Vec3(float x, float y, float z):x(a[0]),y(a[1]),z(a[2]){ this->x=x; this->y=y; this->z=z; }
	Vec3(const Vec3& o):x(a[0]),y(a[1]),z(a[2]){ this->x=o.x; this->y=o.y; this->z=o.z; }

	Vec3& operator=(const Vec3& o){ x=o.x, y=o.y, z=o.z; return *this; }
	Vec3 operator-()const{ return {-x,-y,-z}; }

	Vec3 operator+(const Vec3& o)const{ return {x+o.x, y+o.y, z+o.z}; }
	Vec3 operator-(const Vec3& o)const{ return {x-o.x, y-o.y, z-o.z}; }
	Vec3 operator*(const Vec3& o)const{ return {x*o.x, y*o.y, z*o.z}; }
	Vec3 operator/(const Vec3& o)const{ return {x/o.x, y/o.y, z/o.z}; }
	Vec3 operator+(float w)const{ return {x+w, y+w, z+w}; }
	Vec3 operator-(float w)const{ return {x-w, y-w, z-w}; }
	Vec3 operator*(float w)const{ return {x*w, y*w, z*w}; }
	Vec3 operator/(float w)const{ return {x/w, y/w, z/w}; }

	float len()const{ return sqrtf(lensq()); }
	float lensq()const{ return this->dot(*this); }
	float dot(const Vec3& o)const{ return x*o.x+y*o.y+z*o.z; }
	Vec3 cross(const Vec3& o)const{ return {y*o.z-z*o.y, z*o.x-x*o.z, x*o.y-y*o.x}; }
	Vec3 rotated(const Vec3& axis, float angle)const{
		auto sin_ha = sin(angle/2);
		auto cos_ha = cos(angle/2);
		auto qaxis = Quaternion(cos_ha, axis.x*sin_ha, axis.y*sin_ha, axis.z*sin_ha).normalized();
		auto qvec = Quaternion(0,x,y,z);
		auto res = qaxis*qvec*qaxis.conjugate();
		return {res.x,res.y,res.z};
	}
	Vec3 normalized()const{	
		float lsq=lensq();
		if(float_neq(lsq, 1))
			return *this/fsqrt(lsq);
		return *this;
	}
	void normalize(){ *this=normalized(); }
	bool isOrtho(const Vec3& o)const{ return float_zero(this->dot(o)); }
	bool isUnit()const{ return float_eq(lensq(), 1); }
};

#include <ostream>
std::ostream& operator<<(std::ostream& os, const Vec3& vec);