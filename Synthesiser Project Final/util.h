#pragma once

#include <math.h>

//clamp a value to a range defined by min and max
inline float clamp(float x, float min, float max) {
	return fmaxf(fminf(x, max), min);
}