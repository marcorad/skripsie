#pragma once
#include "defines.h"

struct IIR {
	float d1, d2; // Y coeff
	float n0, n1, n2; // X coeff
	//previous values
	float xm1 = 0.0f, xm2 = 0.0f;
	float ym1 = 0.0f, ym2 = 0.0f;
	//ADD FNC POINTER TO DETERMINE HOW TO CALC COEFF
};


//approximates cos(2*pi*f) for f [0,0.5]
inline float cos_lookup(float f) {
	return lut_lookup(basic_luts[SINE][0] + LUT_SIZE / 4, LUT_SIZE, f * (float)LUT_SIZE);
}

//approximates sin(2*pi*f) for f [0,0.5]
inline float sin_lookup(float f) {
	return lut_lookup(basic_luts[SINE][0], LUT_SIZE, f * (float)LUT_SIZE);
}


inline float iir_filter_sample(IIR * filter, float x) {
	float y = filter->n0 * x;
	y += filter->n1 * filter->xm1; //MULAC ops ?
	y += filter->n2 * filter->xm2;
	y += filter->d1 * filter->ym1;
	y += filter->d2 * filter->ym2;

	//shift prev values
	filter->ym2 = filter->ym1;
	filter->ym1 = y;
	filter->xm2 = filter->xm1;
	filter->xm1 = x;
	return filter->ym1;
}

//LP calcs, f0 in samples/sec
inline void iir_calc_lp_coeff(IIR* filter, float f0, float q_recip) {
	float cosw = cos_lookup(f0); 
	float sinw = sin_lookup(f0);
	float alpha = sinw * 0.5f * q_recip;
	float a0_recip = 1.0f / (alpha + 1.0f); //devision is unavoidable here
	float b0, b1, b2, a1, a2;
	b1 = 1.0f - cosw; 
	b0 = b1 * 0.5f;
	b2 = b0;
	a1 = 2.0f * cosw; //negated to save clock cycles in filtering
	a2 = -1.0f + alpha; //negated to save clock cycles in filtering
	float n0 = b0 * a0_recip;
	float n1 = b1 * a0_recip;
	float n2 = b2 * a0_recip;
	float d1 = a1 * a0_recip;
	float d2 = a2 * a0_recip;

	filter->n0 = n0;
	filter->n1 = n1;
	filter->n2 = n2;
	filter->d1 = d1;
	filter->d2 = d2;
}

//HP calcs, f0 in samples/sec
inline void iir_calc_hp_coeff(IIR* filter, float f0, float q_recip) {
	float cosw = cos_lookup(f0); 
	float sinw = sin_lookup(f0);
	float alpha = sinw * 0.5f * q_recip;
	
	float a0_recip = 1.0f / (alpha + 1.0f);
	float b0, b1, b2, a1, a2;
	b1 = 1.0f + cosw; //PLUS i.s.o MINUS
	b0 = b1 * 0.5f;
	b2 = b0;
	a1 = 2.0f * cosw; //negated to save clock cycles in filtering
	a2 = alpha - 1.0f; //negated to save clock cycles in filtering
	float n0 = b0 * a0_recip;
	float n1 = b1 * a0_recip;
	float n2 = b2 * a0_recip;
	float d1 = a1 * a0_recip;
	float d2 = a2 * a0_recip;

	
	filter->n0 = n0;
	filter->n1 = n1;
	filter->n2 = n2;
	filter->d1 = d1;
	filter->d2 = d2;
}

inline void iir_copy_coeff(IIR* orig, IIR* copy) {
	copy->d1 = orig->d1;
	copy->d2 = orig->d2;
	copy->n0 = orig->n0;
	copy->n1 = orig->n1;
	copy->n2 = orig->n2;
}

inline void iir_reset(IIR* f) {
	f->xm1 = 0.0f;
	f->xm2 = 0.0f;
	f->ym1 = 0.0f;
	f->ym2 = 0.0f;
}

