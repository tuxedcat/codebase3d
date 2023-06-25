#include "common/Vec3.h"

std::ostream& operator<<(std::ostream& os, const Vec3& vec){
	return os<<'('<<vec.x<<','<<vec.y<<','<<vec.z<<')';
}