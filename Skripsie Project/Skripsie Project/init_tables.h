#pragma once

#include <math.h>
#include <stdint.h>

#include "LUT.h"


#define PI 3.14159265359f
#define TWO_PI 6.28318530718f
#define PI_INV 0.31830988618f

#define MAX_ADSR_TIME 5.0f //seconds
#define FS 44100.0f //sample rate

static float square_sample(float pos, int k){
    float m = (2.0f * (float)k - 1.0f);
    return sinf(TWO_PI * m *  pos) / m;
}

static float saw_sample(float pos, int k){
    float sign = k % 2 == 0 ? 1.0f : -1.0f;
    return sinf(TWO_PI * (float)k *  pos) * sign / (float) k;
}

static float tri_sample(float pos, int k){
    k = k-1; //account for different summation limit
    float sign = k % 2 == 0 ? 1.0f : -1.0f;
    float m = (float)(2 * k + 1);
    return sinf(TWO_PI * m *  pos) * sign / m / m;
}

static float sin_sample(float pos, int k){
    return sinf(TWO_PI * pos);
}

static void load_wave(LUT* lut, float (*wave)(float, int)){
    float max = -INFINITY;
    float min = INFINITY;
    float* vals = (float*)malloc(lut->buf_size * sizeof(float));
    for(int i = 0; i < lut->buf_size; i++){
        float pos = (float)i / (float) lut->buf_size;
        float val = 0.0f;
        for(int k = 1; k <= lut->buf_size/2; k++){
            val += wave(pos, k);
        }
        vals[i] = val; 
        if (val < min) min = val;
        if (val > max) max = val;       
    }

    float o = (max-min);
    float s = 1.0f/(max-min);

     for(int i = 0; i < lut->buf_size; i++){
         //convert to maximal range in [0, 1] to ensure no clipping occurs from overshoot
        float y = (vals[i] * 0.99f -o) * s  + 0.5f; //0.99 elimates wrapping
        lut->buf[i] = (Qnum)((float)Q_MAX * y);
     }
}

static void load_basic_tables(LUT* lut_sin, LUT* lut_tri, LUT* lut_saw, LUT* lut_square, int N_luts){
    for(int i = 0; i < N_luts; i++){
        load_wave(&lut_sin[i], sin_sample);
        load_wave(&lut_tri[i], tri_sample);
        load_wave(&lut_saw[i], saw_sample);
        load_wave(&lut_square[i], square_sample);
    }
}

static void load_exp_decay(LUT* lut) {
    float a = powf(1.0f/3.0f, 1.0f/(float)lut->buf_size);
    for (int n = 0; n < lut->buf_size; n++) {
        float e = 1.5f * (1.0f - powf(a, (float)n));
        lut->buf[n] = (Qnum)((float)Q_MAX * e);
    }
}

//takes in a lut with b=1
static void load_exp_stride_lut(LUT* lut) {
    float fw = MAX_ADSR_TIME / FS;
    float stride_n = fw * (float)lut->buf_size;
    float stride_i = roundf(stride_n * pow(2.0f, Q - lut->b));
    lut->buf[0] = 0;
    lut->buf[1] = (Qnum)((uint32_t)stride_i >> (32 - Q));
}

//create luts for b = 2 ... last_b
static LUT* create_n_luts(int last_b) {
    LUT* luts = (LUT*)malloc((last_b - 1) * sizeof(LUT));
    for (int b = 2; b <= last_b; b++) {
        luts[b - 2] = create_LUT(b);
    }
    return luts;
}
