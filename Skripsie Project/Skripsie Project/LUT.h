#pragma once

#include <stdint.h>
#include <stdlib.h> 

#define LERP

//lut defs
#define B_lut 32 //precision of mapped index
#define mapped_index uint32_t
#define index uint16_t
#define MAPPED_INDEX_MAX 0xFFFFFFFF
#define HALF_MAPPED_INDEX_MAX 0x7FFFFFFF

#define b 9 //buffer size power
#define LUT_SIZE 1<<b //512
#define K (B_lut-b)




//Q number defs
#define Q 15
#define Qnum int16_t
#define Qnum_buf int32_t
#define Q_MAX 0x7FFF
#define HALF_Q_MAX (Q_MAX >> 1)
#define Q_MUL_ROUND   (1 << (Q - 1))

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

inline Qnum q_mul(Qnum a, Qnum k){
    Qnum_buf temp;
    temp = ((Qnum_buf)a) * ((Qnum_buf)k);
    temp += Q_MUL_ROUND;
    return (Qnum) (temp >> Q); //SHIFTING NEGATIVE NUMBERS MIGHT NOT BE AN ARITHMETIC SHIFT FOR SOME COMPILERS
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
