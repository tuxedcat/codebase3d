#include <assert.h>
#include <iostream>
#include "common/Mat44.h"
#include "common/Vec3.h"

std::ostream& operator<<(std::ostream& os, const Mat44& m){
	os<<'[';
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			os<<m.a[i][j]<<',';
	return os<<']';
}

Vec3 Mat44::operator*(const Vec3& o)const{
	if(!float_eq(1, a[3][0]*o.x+a[3][1]*o.y+a[3][2]*o.z+a[3][3])){
		std::cerr << "[WARNING]: float_eq(1, a[3][0]*o.x+a[3][1]*o.y+a[3][2]*o.z+a[3][3]) failed." << std::endl;
	}
	return Vec3(
		a[0][0]*o.x + a[0][1]*o.y + a[0][2]*o.z + a[0][3],
		a[1][0]*o.x + a[1][1]*o.y + a[1][2]*o.z + a[1][3],
		a[2][0]*o.x + a[2][1]*o.y + a[2][2]*o.z + a[2][3]);
}