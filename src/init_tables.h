#pragma once

#include <math.h>
#include <stdint.h>

#include "LUT.h"


#define PI 3.14159265359f
#define TWO_PI 6.28318530718f
#define PI_INV 0.31830988618f

static float square_sample(float pos, int k){
    return sinf(TWO_PI * (float)k *  pos) / (2.0f * (float) k - 1);
}

static float saw_sample(float pos, int k){
    float sign = k % 2 == 0 ? 1.0f : -1.0f;
    return sinf(TWO_PI * (float)k *  pos) * sign / (float) k;
}

static float tri_sample(float pos, int k){
    k = k-1; //account for different summation limit
    float sign = k % 2 == 0 ? 1.0f : -1.0f;
    return sinf(TWO_PI * (float)(2*k+1) *  pos) * sign / (2.0f * (float) k + 1) / (2.0f * (float) k + 1);
}

static float sin_sample(float pos, int k){
    if(k != 1) return 0.0f; //only fundamental  
    return sinf(TWO_PI * pos);
}

static void load_wave(LUT* lut, float (*wave)(float, int), float scale, float offset){
    float max = -INFINITY;
    float min = INFINITY;
    float* vals = (float*)malloc(lut->buf_size * sizeof(float));
    for(int i = 0; i < lut->buf_size; i++){
        float pos = (float)i / (float) lut->buf_size;
        float val = 0.0f;
        for(int k = 1; k <= lut->buf_size/2; k++){
            val += wave(pos, k);
        }
        val = scale * val + offset; //scale and offset
        vals[i] = val; 
        if (val < min) min = val;
        if (val > max) max = val;       
    }

    float o = (max-min)/2.0f;
    float s = 1.0f/(max-min);

     for(int i = 0; i < lut->buf_size; i++){
         //convert to maximal range in [0, 1] to ensure no clipping occurs from overshoot
        float val = (vals[i]-o) * s + 0.5f; 
        lut->buf[i] = (Qnum)((float)Q_MAX * val);
     }
}

static void load_basic_tables(LUT* lut_sin, LUT* lut_tri, LUT* lut_saw, LUT* lut_square, int N_luts){
    load_wave(lut_sin, sin_sample, 0.5f, 0.5f); //only one sin lut
    for(int i = 0; i < N_luts; i++){
        load_wave(&lut_tri[i], tri_sample, 8.0f * PI_INV * PI_INV, 0.0f);
        load_wave(&lut_saw[i], saw_sample, -PI_INV, 0.5f);
        load_wave(&lut_square[i], square_sample, 2.0f * PI_INV, 0.5f);
    }
}
