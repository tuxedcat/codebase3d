#include "common/Quaternion.h"

Quaternion::Quaternion(const Mat44& mat):
	Quaternion(
		sqrtf(1+mat.a[0][0]+mat.a[1][1]+mat.a[2][2]),
		(mat.a[2][1]-mat.a[1][2])/(4*w),
		(mat.a[0][2]-mat.a[2][0])/(4*w),
		(mat.a[1][0]-mat.a[0][1])/(4*w)){
	//https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	//TODO: assert special orthogonal matrix
	if(isnanf(w))
		*this=Quaternion();
}

//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Quaternion-derived_rotation_matrix
//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
Mat44 Quaternion::toRotationMatrix()const{
	assert(isUnit());
	float s=1/lensq();
	return {
		1-2*s*(y*y+z*z), 2*s*(x*y-z*w), 2*s*(x*z+y*w), 0,
		2*s*(x*y+z*w), 1-2*s*(x*x+z*z), 2*s*(y*z-w*x), 0,
		2*s*(x*z-y*w), 2*s*(y*z+w*x), 1-2*s*(x*x+y*y), 0,
		0, 0, 0, 1,
	};
}