#pragma once

#include <stdint.h>
#include <stdlib.h> 

//lut defs
#define B_lut 32 //precision of mapped index
#define mapped_index uint32_t
#define index uint16_t
#define LERP




//Q number defs
#define Q 16
#define Qnum uint16_t
#define Qnum_buf uint32_t
#define Q_MAX 0xFFFF
#define HALF_Q_MAX 0x8FFF
#define Q_MUL_ROUND   (1 << (Q - 1))

struct LUT
{
    uint8_t b;
    uint32_t buf_size;
    Qnum* buf;
};

// create a lut with size 2^b
static LUT create_LUT(uint8_t b){
    LUT lut;
    lut.b = b;
    lut.buf_size = 1<<b;
    lut.buf = (Qnum*)malloc(lut.buf_size * sizeof(Qnum));    
    return lut;
}

static inline Qnum q_mul(Qnum a, Qnum b){
    Qnum_buf temp;
    temp = ((Qnum_buf)a) * ((Qnum_buf)b);
    temp += Q_MUL_ROUND;
    return (Qnum) (temp >> Q);
}

//only works if mod is a power of 2
static inline mapped_index fast_mod(mapped_index x, mapped_index mod){
    return x & (mod-1);
}

static inline Qnum lerp_lut(LUT lut, mapped_index i){
#ifdef LERP
    uint8_t K = B_lut - lut.b; //32 - 9 = 23 min
    index n = (index) (i >> K); //find actual index
    mapped_index i_floor = n << K; //find floored mapped index
    Qnum dq = (i - i_floor) >> (K - Q); //find delta as a Qnum
    Qnum xn = lut.buf[n];
    index np1 = fast_mod(n + 1, lut.buf_size);  //wrap around
    Qnum xnp1 = lut.buf[np1];
    if (xnp1 > xn) return xn + q_mul(xnp1 - xn, dq);
    else return xn - q_mul(xn - xnp1, dq);
#endif // LERP

#ifndef LERP
    uint8_t K = B_lut - lut.b;
    index n = (index)(i >> K); //find actual index
    return lut.buf[n];
#endif  //PREVIOUS SAMPLE   
}
