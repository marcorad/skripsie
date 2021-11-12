#pragma once
#include "defines.h"
#include "includes.h"


// find a value between x and y where d covers the full distance between
inline float lerp(float x1, float x2, float d) {
    return x1 + d * (x2 - x1); //3 clk
}

//mod must be power of 2
inline uint16_t fast_mod(uint32_t x, uint32_t mod) {
    return x & (mod - 1);
}

//given the lut and lut size, find the interpolated value at index i [0, lut_size), and wrap
inline float lut_lookup(float lut[], uint32_t lut_size, float i) {
    uint32_t floor_i = (uint32_t) i; //VCVT.U32.F32 Sd, Dm  
    float delta = i - (float)floor_i; 
    float x1 = lut[floor_i]; 
    float x2 = lut[fast_mod(floor_i + 1, lut_size)]; 
    return lerp(x1, x2, delta); 
}

//given the lut, find the interpolated value at index i [0, lut_size-1) without wrapping
inline float lut_lookup_no_wrap(float lut[], float i) {
    uint32_t floor_i = (uint32_t)i; //VCVT.U32.F32 Sd, Dm 
    float delta = i - (float)floor_i; 
    float x1 = lut[floor_i]; 
    float x2 = lut[floor_i + 1];
    return lerp(x1, x2, delta); 
}




