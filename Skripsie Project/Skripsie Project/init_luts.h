#pragma once

#include <math.h>
#include <stdint.h>

#include "LUT.h"
#include "defines.h"



static float square_sample(float pos, int k){
    if (k % 2 == 0) return 0.0f; //only odd harmonics
    float m = (float)(k);
    return sinf(TWO_PI * m *  pos) / m;
}

static float saw_sample(float pos, int k){
    float sign = k % 2 == 0 ? 1.0f : -1.0f;
    return -sinf(TWO_PI * (float)k *  pos) * sign / (float) k;
}

static float tri_sample(float pos, int k){
    if (k % 2 == 0) return 0.0f; //only odd harmonics
    int i = (k - 1)/2; //account for different summation limit
    float m = (float)(2*i + 1);
    float sign = (i) % 2 == 0 ? 1.0f : -1.0f;
    return sinf(TWO_PI * m *  pos) * sign / m / m;
}

static float sin_sample(float pos, int k){
    return sinf(TWO_PI * pos);
}

static void load_wave(LUT* lut, float (*wave)(float, int), int num_harmonics){
    float max = -INFINITY;
    float min = INFINITY;
    float* vals = new float[lut->buf_size];
    for(int i = 0; i < lut->buf_size; i++){
        float pos = (float)i / (float) lut->buf_size;
        float val = 0.0f;
        for(int k = 1; k < num_harmonics; k++){
            val += wave(pos, k);
        }
        *(vals+i) = val; 
        if (val < min) min = val;
        if (val > max) max = val;       
    }
    float o = (max-min)/2.0f;
    float s =2.0f/(max-min);

     for(int i = 0; i < lut->buf_size; i++){
         //convert to maximal range in [-0.5, 0.5] to ensure no clipping occurs from overshoot
         float y = (vals[i]) * 0.99f * s; //0.99 elimates wrapping
        lut->buf[i] = (Qnum)((float)Q_MAX * y);
     }
}

static void load_basic_luts(LUT* lut_sin, LUT* lut_tri, LUT* lut_saw, LUT* lut_square, int N_luts){
    for(int i = 0; i < N_luts; i++){
        int n_harmonics = 1<<(i+1);
        load_wave(&lut_sin[i], sin_sample, n_harmonics);
        load_wave(&lut_tri[i], tri_sample, n_harmonics);
        load_wave(&lut_saw[i], saw_sample, n_harmonics);
        load_wave(&lut_square[i], square_sample, n_harmonics);
    }
}

static void load_exp_decay(LUT* lut) {
    float a = powf(1.0f/3.0f, 1.0f/(float)lut->buf_size);
    for (int n = 0; n < lut->buf_size; n++) {
        float e = 1.5f * (1.0f - powf(a, (float)n));
        lut->buf[n] = (Qnum)((float)Q_MAX * e);
    }
}

//takes in a lut with k=1
static void load_exp_stride_lut(LUT* lut) {
    float fw = MAX_ADSR_TIME / FS;
    float stride_n = fw * (float)lut->buf_size;
    float stride_i = roundf(stride_n * pow(2.0f, Q-b));
    lut->buf[0] = 0;
    lut->buf[1] = (Qnum)((uint32_t)stride_i >> (32 - Q));
}

//create luts for harmonic indices [0,7]
static LUT* create_luts() {
    LUT* luts = (LUT*)malloc(8 * sizeof(LUT));
    for (int k = 0; k < 8; k++) {
        luts[k] = create_LUT(k);
    }
    return luts;
}


LUT* lut_saw;
LUT* lut_tri; 
LUT* lut_square; 
LUT* lut_sin;

LUT exp_decay;

LUT** basic_luts;

void init_basic_luts() {

    
    lut_saw = create_luts();
    lut_tri = create_luts();
    lut_square = create_luts();
    lut_sin = create_luts();
    basic_luts = new LUT*[]{ lut_sin, lut_tri, lut_saw , lut_square };
    load_basic_luts(lut_sin, lut_tri, lut_saw, lut_square, 8);

    exp_decay = create_LUT(0);
    load_exp_decay(&exp_decay);
}
