#pragma once

#include "defines.h"
#include "init_luts.h"

struct IIR_coeff {
	float d1, d2; // Y coeff
	float n0, n1, n2; // X coeff
};

struct IIR_prev_values {
	//previous values
	float xm1 = 0.0f, xm2 = 0.0f;
	float ym1 = 0.0f, ym2 = 0.0f;
};


inline float iir_filter_sample(IIR_coeff* coeff, IIR_prev_values* prev, float x) {
	float y = coeff->n0 * x;
	y += coeff->n1 * prev->xm1; //MULAC ops ?
	y += coeff->n2 * prev->xm2;
	y += coeff->d1 * prev->ym1;
	y += coeff->d2 * prev->ym2;

	//shift prev values
	prev->ym2 = prev->ym1;
	prev->ym1 = y;
	prev->xm2 = prev->xm1;
	prev->xm1 = x;
	return prev->ym1;
}




//TO DO: UPDATE SO THAT Q IS NOT RECIP

//LP calcs, f0 in samples/sec
inline void iir_calc_lp24_coeff(IIR_coeff* filter, float f0, float q) {
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
inline void iir_calc_hp24_coeff(IIR_coeff* filter, float f0, float q) {
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
inline void iir_calc_bp12_coeff(IIR_coeff* filter, float f0, float q) {
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

//LP calcs, f0 in samples/sec
inline void iir_calc_lp12_coeff(IIR_coeff* filter, float f0, float q) {
	float cosw = cos_lookup(f0);
	float sinw = sin_lookup(f0);
	float one_minus_cosw = 1.0f - cosw;

	//denominator
	float a0_recip = 1.0f / (one_minus_cosw + sinw); //devision is unavoidable here
	float a1 = 2.0f * one_minus_cosw; //negated version of what is in table, to account for d1 = -a1/a0
	float a2 = one_minus_cosw - sinw; //negated version of what is in table, to account for d2 = -a2/a0


	float b0, b1, b2;
	b0 = one_minus_cosw;
	b2 = b0;
	b1 = 2.0f * b0;

	float n0 = b0 * a0_recip;
	float n1 = b1 * a0_recip;
	float n2 = b2 * a0_recip;
	float d1 = -a1 * a0_recip; //NEGATE!!
	float d2 = -a2 * a0_recip;

	filter->n0 = n0;
	filter->n1 = n1;
	filter->n2 = n2;
	filter->d1 = d1;
	filter->d2 = d2;
}

//HP calcs, f0 in samples/sec
inline void iir_calc_hp12_coeff(IIR_coeff* filter, float f0, float q) {
	float cosw = cos_lookup(f0);
	float sinw = sin_lookup(f0);
	float one_minus_cosw = 1.0f - cosw;

	//denominator
	float a0_recip = 1.0f / (one_minus_cosw + sinw); //devision is unavoidable here
	float a1 = 2.0 * one_minus_cosw; //negated version of what is in table, to account for d1 = -a1/a0
	float a2 = one_minus_cosw - sinw; //negated version of what is in table, to account for d2 = -a2/a0


	float b0, b1, b2;
	b0 = sinw;
	b2 = -b0;
	b1 = 0.0f;

	float n0 = b0 * a0_recip;
	float n1 = b1 * a0_recip;
	float n2 = b2 * a0_recip;
	float d1 = -a1 * a0_recip;
	float d2 = -a2 * a0_recip;

	filter->n0 = n0;
	filter->n1 = n1;
	filter->n2 = n2;
	filter->d1 = d1;
	filter->d2 = d2;
}

//HP calcs, f0 in samples/sec
inline void iir_calc_no_coeff(IIR_coeff* filter, float f0, float q) {
	*filter = { 0.0f,0.0f,0.0f,0.0f,0.0f };
	filter->n0 = 1.0f;
}

inline void iir_copy_coeff(IIR_coeff* orig, IIR_coeff* copy) {
	copy->d1 = orig->d1;
	copy->d2 = orig->d2;
	copy->n0 = orig->n0;
	copy->n1 = orig->n1;
	copy->n2 = orig->n2;
}

inline void iir_reset(IIR_prev_values* f) {
	f->xm1 = 0.0f;
	f->xm2 = 0.0f;
	f->ym1 = 0.0f;
	f->ym2 = 0.0f;
}

