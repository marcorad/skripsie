#pragma once

#include <math.h>

//clamp a value to a range defined by min and max
inline float clamp(float x, float min, float max) {
	return fmaxf(fminf(x, max), min);
}

inline float fract(float x) {
	return x - floorf(x);
}

inline float sgn(float x) {
	if (x >= 0.0f) return 1.0f;
	return -1.0f;
}
