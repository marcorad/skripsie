#pragma once

#include "defines.h"
#include <math.h>
#include "LUT.h"
#include "IIR.h"
#include "util.h"



float lut_tanh[TANH_LUT_SIZE];
const float tanh_grad0 = 9.0f; //can be adjusted. This is the gradient at 0. This will determine what the edges of the LUT looks like
const float tanh_grad0_inv = 1.0f / tanh_grad0;

void load_tanh() {
    float norm = 1.0/tanhf(tanh_grad0 * (1.0f - 0.5f)); //tanh at the max index, to ensure the lut begins and ends at +-1.0f
    for (int i = 0; i < TANH_LUT_SIZE; i++)
    {
        float x = (float)i / (float)TANH_LUT_SIZE;
        lut_tanh[i] = tanhf(tanh_grad0 * (x - 0.5f)) * norm;
    }
}

//saturate a number. returns a lut lookup of tanh(t)
inline float tanh_lookup(float t) {
    float x = t * tanh_grad0_inv + 0.5f; //offset to be centered around 0.5 and cancel out gradient gain
    //hard clip, to approximate tanh behaviour as x->inf
    if (x < 0.0f) return -1.0f;
    if (x > 1.0f) return 1.0f;
    //otherwise return approximated tanh(in)
    return lut_lookup(lut_tanh, TANH_LUT_SIZE, x * (TANH_LUT_SIZE-1));
}

inline float waveshape_calc_tanh_norm(float gain) {
    return 1.0f / tanh_lookup(gain);
}

inline float waveshape_calc_sine_norm(float gain) {
    if (gain > 1.0f) return 1.0f;
    return 1.0f / sin_lookup(0.25f * (fract(gain)));
}

//norm must be 1/tanh(gain) = 1.0f / tanh_lookup(gain)
float waveshape_tanh(float x, float gain, float norm) {
    return norm * tanh_lookup(gain * x);
}

//no modification
float waveshape_none(float x, float gain, float norm) {
    return x;
}

//retuns 0 for values of |gain*x| > 1.0f, norm must be 1/sin(0.5*pi*gain) = 1.0f / sin_lookup(0.25f * (frac(gain))) for |gain| < 1 and 1 otherwise
float waveshape_sine(float x, float gain, float norm) {
    float t = (gain*0.25f*x + 0.5f); //for a gain of 1
    if (t < 0.0f || t > 0.5f) return 0.0f;
    return -1.0f * sin_lookup(t) * norm;
}