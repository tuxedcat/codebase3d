#include <cmath>

class Vec3{
public:
	float x,y,z;
	static Vec3 zero(){ return {0,0,0}; }
	static Vec3 one(){ return {1,1,1}; }
	Vec3():Vec3(0,0,0){}
	Vec3(float x, float y, float z):x(x),y(y),z(z){}

	Vec3 operator-()const{ return {-x,-y,-z}; }

	Vec3 operator+(const Vec3& o)const{ return {x+o.x, y+o.y, z+o.z}; }
	Vec3 operator-(const Vec3& o)const{ return {x-o.x, y-o.y, z-o.z}; }
	Vec3 operator*(const Vec3& o)const{ return {x*o.x, y*o.y, z*o.z}; }
	Vec3 operator/(const Vec3& o)const{ return {x/o.x, y/o.y, z/o.z}; }
	Vec3 operator+(float w)const{ return {x+w, y+w, z+w}; }
	Vec3 operator-(float w)const{ return {x-w, y-w, z-w}; }
	Vec3 operator*(float w)const{ return {x*w, y*w, z*w}; }
	Vec3 operator/(float w)const{ return {x/w, y/w, z/w}; }

	float len(const Vec3& o)const{ return sqrtf(this->dot(*this)); }
	float dot(const Vec3& o)const{ return x*o.x+y*o.y+z*o.z; }
	Vec3 cross(const Vec3& o)const{ throw "Unimplemented"; }
};