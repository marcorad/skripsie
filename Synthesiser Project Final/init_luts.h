#pragma once

#include <math.h>
#include <stdint.h>

#include "LUT.h"
#include "defines.h"



float square_sample(float pos, int k){
    if (k % 2 == 0) return 0.0f; //only odd harmonics
    float m = (float)(k);
    return sinf(TWO_PI * m *  pos) / m;
}

float saw_sample(float pos, int k){
    float sign = k % 2 == 0 ? 1.0f : -1.0f;
    return -sinf(TWO_PI * (float)k *  pos) * sign / (float) k;
}

float tri_sample(float pos, int k){
    if (k % 2 == 0) return 0.0f; //only odd harmonics
    int i = (k - 1)/2; //account for different summation limit
    float m = (float)(2*i + 1);
    float sign = (i) % 2 == 0 ? 1.0f : -1.0f;
    return sinf(TWO_PI * m *  pos) * sign / m / m;
}

float sin_sample(float pos, int k){
    return sinf(TWO_PI * pos);
}

void load_wave(float buf[],  float (*wave)(float, int), int num_harmonics){
    float max = -INFINITY;
    float min = INFINITY;
    float* vals = new float[LUT_SIZE];
    for(int i = 0; i < LUT_SIZE; i++){
        float pos = (float)i / (float) LUT_SIZE;
        float val = 0.0f;
        for(int k = 1; k < num_harmonics; k++){
            val += wave(pos, k);
        }
        *(vals+i) = val; 
        if (val < min) min = val;
        if (val > max) max = val;       
    }
    float offset = min + (max-min)/2.0f;
    float s =2.0f/(max-min);

     for(int i = 0; i < LUT_SIZE; i++){
         //normalise to range [-1.0, 1.0]
         float y = (vals[i] - offset) * s; 
         buf[i] = y;
     }
}


void load_exp_decay(float exp[], uint16_t exp_size) {
    float a = powf(1.0f/3.0f, 1.0f/(float)exp_size);
    for (int n = 0; n < exp_size; n++) {
        float e = 1.5f * (1.0f - powf(a, (float)n));
        exp[n] = e;
    }
}

float basic_luts[4][8][LUT_SIZE];

void load_basic_luts() {
    for (int i = 0; i < 8; i++) {
        int n_harmonics = 1 << (i + 1); // 2 4 8 16 32 64 128 256
        load_wave(basic_luts[SINE][i], sin_sample, n_harmonics); //exception for sin to save memory?
        load_wave(basic_luts[TRIANGLE][i], tri_sample, n_harmonics);
        load_wave(basic_luts[SAWTOOTH][i], saw_sample, n_harmonics);
        load_wave(basic_luts[SQUARE][i], square_sample, n_harmonics);
    }
}


float exp_decay[128];

void init_basic_luts() {
    load_basic_luts();
    load_exp_decay(exp_decay, 128);
}
