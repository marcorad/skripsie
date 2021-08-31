#pragma once
#include "defines.h"

struct IIR {
	float d1, d2; // Y coeff
	float n0, n1, n2; // X coeff
	//previous values
	float xm1 = 0.0f, xm2 = 0.0f;
	float ym1 = 0.0f, ym2 = 0.0f;	
};


//approximates cos(2*pi*f) for f [0,0.75]
inline float cos_lookup(float f) {
	return lut_lookup(basic_luts[SINE][0] + LUT_SIZE / 4, LUT_SIZE, f * (float)LUT_SIZE);
}

//approximates sin(2*pi*f) for f [0,1]
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

inline float iir_filter_sample_feedback_sat(IIR* filter, float x) {
	float y = filter->n0 * x;
	y += filter->n1 * filter->xm1; //MULAC ops ?
	y += filter->n2 * filter->xm2;
	y += filter->d1 * filter->ym1;
	y += filter->d2 * filter->ym2;

	//SATURATE HERE

	//shift prev values
	filter->ym2 = filter->ym1;
	filter->ym1 = y;
	filter->xm2 = filter->xm1;
	filter->xm1 = x;
	return filter->ym1;
}

//TO DO: UPDATE SO THAT Q IS NOT RECIP

//LP calcs, f0 in samples/sec
inline void iir_calc_lp_coeff(IIR* filter, float f0, float q) {
	float cosw = cos_lookup(f0); 
	float sinw = sin_lookup(f0);
	float twoq = 2.0f * q;

	//denominator
	float a0_recip = 1.0f / (twoq + sinw); //devision is unavoidable here
	float a1 = 2.0 * twoq * cosw; //negated version of what is in table, to account for d1 = -a1/a0
	float a2 = sinw - twoq; //negated version of what is in table, to account for d2 = -a2/a0


	float b0, b1, b2;
	b1 = twoq * (1.0f - cosw); 
	b0 = b1 * 0.5f;
	b2 = b0;
	
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
inline void iir_calc_hp_coeff(IIR* filter, float f0, float q) {
	float cosw = cos_lookup(f0);
	float sinw = sin_lookup(f0);
	float twoq = 2.0f * q;

	//denominator
	float a0_recip = 1.0f / (twoq + sinw); //devision is unavoidable here
	float a1 = 2.0 * twoq * cosw; //negated version of what is in table, to account for d1 = -a1/a0
	float a2 = sinw - twoq; //negated version of what is in table, to account for d2 = -a2/a0


	float b0, b1, b2;
	b1 = -twoq * (1.0f + cosw);
	b0 = b1 * -0.5f;
	b2 = b0;

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

//BP calcs, f0 in samples/sec
inline void iir_calc_bp_coeff(IIR* filter, float f0, float q) {
	float cosw = cos_lookup(f0);
	float sinw = sin_lookup(f0);
	float twoq = 2.0f * q;

	//denominator
	float a0_recip = 1.0f / (twoq + sinw); //devision is unavoidable here
	float a1 = 2.0 * twoq * cosw; //negated version of what is in table, to account for d1 = -a1/a0
	float a2 = sinw - twoq; //negated version of what is in table, to account for d2 = -a2/a0


	float b0, b1, b2;
	b1 = 0.0f;
	b0 = q * sinw;
	b2 = -b0;

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

