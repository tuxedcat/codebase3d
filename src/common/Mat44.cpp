#include "common/Mat44.h"

std::ostream& operator<<(std::ostream& os, const Mat44& m){
	os<<'[';
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			os<<m.a[i][j]<<',';
	return os<<']';
}