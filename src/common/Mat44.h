#pragma once
#include <initializer_list>
struct Mat44{
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
					b.a[i][j]+=a[i][k]*a[k][j];
		return b;
	}
	Mat44& operator*=(const Mat44& o){
		return *this = *this * o;
	}
};