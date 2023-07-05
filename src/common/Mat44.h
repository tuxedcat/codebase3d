#pragma once
#include <initializer_list>
#include <algorithm>

class Vec3;
class Mat44{
public:
	static Mat44 identity(){ return {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1};
	}
	float a[4][4];
	Mat44():a({}){}
	Mat44(std::initializer_list<float> il){
		auto it=il.begin();
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				a[i][j]=*it++;
	}
	Mat44 operator*(const Mat44& o)const{
		Mat44 b;
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				for(int k=0;k<4;k++)
					b.a[i][j]+=a[i][k]*o.a[k][j];
		return b;
	}
	Vec3 operator*(const Vec3& o)const;
	Mat44& operator*=(const Mat44& o){ return *this = *this * o; }
	Mat44 transposed()const{
		auto ret=*this;
		for(int i=0;i<4;i++)
			for(int j=0;j<i;j++)
				std::swap(ret.a[i][j],ret.a[j][i]);
		return ret;
	}
};

#include <ostream>
std::ostream& operator<<(std::ostream& os, const Mat44& m);