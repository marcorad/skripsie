#pragma once
#include "LUT.h"
#include "defines.h"
#include "trig_buff.h"

//ENSURE f is never higher than 0x8FFF

inline Qnum trig_lookup(const Qnum* buf, uint16_t f) {
    uint16_t n = (uint16_t)(f >> (TRIG_BUFFER_POWER - 1)); //find actual index
    uint16_t f_floor = n << (TRIG_BUFFER_POWER - 1); //find floored index
    Qnum dq = (f - f_floor) << (Q_IIR - (TRIG_BUFFER_POWER - 1)); //find delta as a Qnum
    Qnum xn = buf[n];
    index np1 = n + 1; // NO WRAPPING
    Qnum xnp1 = buf[np1];
    return xn + q_mul(xnp1 - xn, dq, Q_IIR);
}

inline int32_t cos_lookup(uint16_t f) {
	return (int32_t)trig_lookup(cos_buf, f);
}

inline int32_t sin_lookup(uint16_t f) {
	return (int32_t)trig_lookup(sin_buf, f);
}

//convert frequency cutoff (Hz) to Q16 format (cycles/sample)
inline uint16_t iir_freq_float_to_int(float f) {
	return (uint16_t)(f / FS * (float)0xFFFF);
}

//convert Q to 1/Q in Q14 format (cycles/sample)
//Q must be between 1/2 and 5 (>3 can cause instability)
inline int32_t iir_Q_float_to_int_recip(float q) {
	return (int32_t)(1/q * (float)(Q_IIR_ONE));
}



