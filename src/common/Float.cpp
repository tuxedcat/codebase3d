#include "common/Float.h"

bool float_zero(float x){ return fabs(x)<eps; }
bool float_eq(float x, float y){ return float_zero(x-y); }
bool float_neq(float x, float y){ return !float_eq(x,y); }
bool float_lteq(float x, float y){ return y-x>-eps; }
bool float_gteq(float x, float y){ return x-y>-eps; }