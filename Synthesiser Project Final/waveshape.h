#pragma once

#include "defines.h"
#include <math.h>
#include "LUT.h"
#include "IIR.h"
#include "util.h"
#include "waveshape_filter_cutoffs.h"

# define tanh_grad0 9.0f //can be adjusted. This is the gradient at 0. This will determine what the edges of the LUT looks like
# define tanh_grad0_inv (1.0f / tanh_grad0)


float lut_tanh[TANH_LUT_SIZE];

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
    return lut_lookup(lut_tanh, TANH_LUT_SIZE, x * (TANH_LUT_SIZE-1)); //makes sure no wrapping occurs, recall that that the last value is 1
}

inline float waveshape_calc_tanh_norm(float gain) {
    return 1.0f / tanh_lookup(gain);
}

inline float waveshape_calc_sine_norm(float gain) {
    if (gain >= 1.0f) return 1.0f;
    return 1.0f / sin_lookup(0.25f * (gain));
}

//norm must be 1/tanh(gain) = 1.0f / tanh_lookup(gain)
float waveshape_tanh(float x, float gain) {
    return tanh_lookup(gain * x);
}

//no modification
float waveshape_none(float x, float gain) {
    return x;
}

//norm must be 1/sin(0.5*pi*gain) = 1.0f / sin_lookup(0.25f * (frac(gain))) for |gain| < 1 and 1 otherwise
float waveshape_sine(float x, float gain) {
    gain = gain < 1.0f ? gain : 1.0f + 0.01f * gain; //gain of smaller than 1 acts like distortion, otherwise it acts as a shaper which must be limited otherwise it becomes unwieldy
    float t = fract(0.25f * gain * x - 0.5f); //gain of 1 has amplitude 1, also modulates index to allow for periodic behaviour in the sin lookup
    return -sin_lookup(t);
}

//inline void ws_config_tanh_saturator(waveshaper* ws, float g) {
//    ws->gain = g;
//    ws->waveshape = &waveshape_tanh;
//    float freq;
//    if (g > 63.0f) { //a point of diminishing returns, and also to save memory
//        freq = tanh_filter_cutoff[63];
//    }
//    else {
//        freq = lut_lookup(tanh_filter_cutoff, 64, g);
//    }
//    iir_calc_lp12_coeff(&ws->filter_AA, freq, 1.0f); //Q does not matter here
//    ws->gain_prime = fmaxf(1.0f, 1.0f / g);
//    ws->norm = 1.0f;
//}

//inline void ws_config_none(waveshaper* ws) {
//    ws->gain = 1.0f;
//    ws->gain_prime = 1.0f;
//    ws->waveshape = &waveshape_none;
//}