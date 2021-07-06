#pragma once
#include "LUT.h"
#include "defines.h"
#include "trig_buff.h"



inline int32_t cos_lookup(uint16_t f) {
	return (int32_t)cos_buf[f >> (TRIG_BUFFER_POWER - 1)];
}

inline int32_t sin_lookup(uint16_t f) {
	return (int32_t)sin_buf[f >> (TRIG_BUFFER_POWER - 1)];
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

