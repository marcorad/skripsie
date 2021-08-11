#pragma once

#include <stdint.h>
#include <stdlib.h> 

#include "defines.h"

struct LUT
{
    uint16_t buf_size;
    Qnum* buf;
};

// create a lut with hamonic index k and size LUT_SIZE
LUT create_LUT(uint8_t k){
    LUT lut;
    lut.buf_size = LUT_SIZE;
    lut.buf = (Qnum*)malloc(lut.buf_size * sizeof(Qnum));    
    return lut;
}

//for Q15 numbers
inline Qnum q_mul(Qnum a, Qnum k){
    Qnum_buf temp;
    temp = ((Qnum_buf)a) * ((Qnum_buf)k);
    temp += Q_MUL_ROUND; //THIS MIGHT NOT BE NECESSARY!
    return (Qnum) (temp >> Q); //SHIFTING NEGATIVE NUMBERS MIGHT NOT BE AN ARITHMETIC SHIFT FOR SOME COMPILERS
}

//for any Q number
inline Qnum q_mul(Qnum a, Qnum k, uint8_t q) {
    Qnum_buf temp;
    temp = ((Qnum_buf)a) * ((Qnum_buf)k);
    //temp += Q_MUL_ROUND; //THIS MIGHT NOT BE NECESSARY!
    return (Qnum)(temp >> q); //SHIFTING NEGATIVE NUMBERS MIGHT NOT BE AN ARITHMETIC SHIFT FOR SOME COMPILERS
}

//only works if mod is a power of 2
inline mapped_index fast_mod(mapped_index x, mapped_index mod){
    return x & (mod-1);
}

// find a value between x and y where d covers the full distance between
inline Qnum q_lerp(Qnum x, Qnum y, Qnum d) {
    return x + q_mul(y - x, d);
}

static inline Qnum lut_lookup(LUT* lut, mapped_index i) {
#ifdef LERP
    index n = (index) (i >> K); //find actual index
    mapped_index i_floor = n << K; //find floored mapped index
    Qnum dq = (i - i_floor) >> (K - Q); //find delta as a Qnum
    Qnum xn = lut->buf[n];
    index np1 = fast_mod(n + 1, lut->buf_size);  //wrap around
    Qnum xnp1 = lut->buf[np1];
    return xn + q_mul(xnp1 - xn, dq);
#endif // LERP

#ifndef LERP
    index n = (index)(i >> K); //find actual index
    return lut->buf[n];
#endif  //PREVIOUS SAMPLE   
}


