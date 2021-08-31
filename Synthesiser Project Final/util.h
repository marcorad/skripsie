#pragma once

#include <math.h>

//clamp a value to a range defined by min and max
inline float clamp(float x, float min, float max) {
	return fmaxf(fminf(x, max), min);
}

inline float floor(float x) {
	return (float)((int)x);
}

inline float fract(float x) {
	return x - floor(x);
}