#pragma once
#include "defines.h"
#include "LUT.h"
#include "IIR_util.h"
#include <math.h>



struct IIR {
	int32_t d1, d2; // Y coeff
	int32_t n0, n1, n2; // X coeff

	//previous values
	int32_t xm1 = 0, xm2 = 0;
	int32_t ym1 = 0, ym2 = 0;
};


////See Configure the Coefficients for Digital Biquad Filters in TLV320AIC3xxx Family Application report
////15 operations?
//inline Qnum filter(IIR* filter, Qnum x_in) {
//	//iir
//	int32_t y = 0, t = 0, x = (int32_t)x_in >> 1; // scale down x to allow for high Q
//	//should be MULAC ops followed by ASR ops
//	y += filter->d1 * filter->ym1 >> (Q - 1);
//	y += filter->n1 * filter->xm1 >> (Q - 1);
//	y += filter->d2 * filter->ym2 >> Q;
//	y += filter->n2 * filter->xm2 >> Q;
//	y += filter->n0 * x >> Q; //SATURATE EACH OF THESE?
//
//	//shift prev values
//	filter->ym2 = filter->ym1;
//	filter->ym1 = y;
//	filter->xm2 = filter->xm1;
//	filter->xm1 = x;
//	return (Qnum)(filter->ym1);
//}


//See Configure the Coefficients for Digital Biquad Filters in TLV320AIC3xxx Family Application report
//15 operations?
inline Qnum filter_opt(IIR* filter, Qnum x_in) {
	//iir
	int32_t y = 0, t = 0, x = (int32_t)x_in >> 2; // scale down x to allow for high Q
	//should be MULAC ops followed by ASR ops
	//SATURATE EACH OF THESE?
	//floats would be faster, but this allows for table lookups to saturate the values
	y += filter->n0 * x >> (Q_IIR + 2);
	y += filter->n1 * filter->xm1 >> (Q_IIR + 2);
	y += filter->n2 * filter->xm2 >> (Q_IIR + 2);
	y += filter->d1 * filter->ym1 >> Q_IIR;	
	y += filter->d2 * filter->ym2 >> Q_IIR;
	
	
	//shift prev values
	filter->ym2 = filter->ym1;
	filter->ym1 = y;
	filter->xm2 = filter->xm1;
	filter->xm1 = x;
	return (Qnum)(filter->ym1);	
}

inline void iir_reset(IIR* f) {
	f->xm1 = 0;
	f->xm2 = 0;
	f->ym1 = 0;
	f->ym2 = 0;
}

//inline void filter_buffer(IIR* iir, Qnum* buf, uint16_t N) {
//	for (int i = 0; i < N; i++) {
//		buf[i] = filter(iir, buf[i]);
//	}
//}

inline void filter_buffer_opt(IIR* iir, Qnum* buf, uint32_t N) {
	for (int i = 0; i < N; i++) {
		buf[i] = filter_opt(iir, buf[i]);
	}
}

////THIS MUST BE FASTER!!!
//inline void iir_calc_lp_coeff(IIR* filter, float f0, float q) {
//	//constants
//	float w0 = TWO_PI * f0 / FS;
//	float cosw0 = cosf(w0); //LOOKUP TABLES? w0 will be provided by an envelope
//	float sinw0 = sinf(w0);
//	float alpha = sinw0 / 2.0f / q;
//	float a0, a1, a2, b0, b1, b2;
//
//	b1 = (1.0f - cosw0);
//	b0 = b1 / 2.0f;
//	b2 = b0;
//	a0 = 1.0f + alpha;
//	a1 = -2.0f * cosw0;
//	a2 = 1.0f - alpha;
//	
//	float N0r = b0 / a0;
//	//float factor = N0r;
//	float N1r = b1 / a0 / 2.0f; 
//	//factor = fmaxf(factor, N1r);
//	float N2r = b2 / a0;
//	//factor = fmaxf(factor, N2r);
//
//	float range = (float)(Q_MAX); //SEE IF Q NUMBERS CAN RATHER BE USED
//	float n0, n1, n2, d1, d2;
//	float scale = range;
//	n0 = N0r * scale;
//	n1 = N1r * scale;
//	n2 = N2r * scale;
//	d1 = (a1 / a0 / -2.0f) * range;
//	d2 = (-a2 / a0) * range;
//
//	filter->n0 = (int32_t)n0;
//	filter->n1 = (int32_t)n1;
//	filter->n2 = (int32_t)n2;
//	filter->d1 = (int32_t)d1;
//	filter->d2 = (int32_t)d2;
//}


//FIX!! LOW FREQUENCIES DO NOT WORK WELL
//q_recip is Q14, f0 is Q16 [0, 1/2) unsigned
//ensure q_recip is element of [1/5, 2] in Q14 format
inline void iir_calc_lp_coeff_opt(IIR* filter, uint16_t f0, int32_t q_recip) {
	//using int32_t for convenience
	int32_t cosw = cos_lookup(f0);
	int32_t sinw = sin_lookup(f0);
	int32_t alpha;
	alpha = (sinw * q_recip) >> (Q_IIR + 1); //[1/10,1] [0,1]
	//if (alpha == 0) alpha = 1;
	int32_t a0_recip = (Q_IIR_ONE << Q_IIR) / (alpha + Q_IIR_ONE); //[1/2, 1], max 12 + 2 cycles
	int32_t b0, b1, b2, a1, a2;
	b1 = ((int32_t)(Q_IIR_ONE) - cosw) << 2; //[0,2], multiply by 4 to prevent n0, n1 and n2 from being 0
	b0 = b1 >> 1;
	b2 = b0;
	a1 = -cosw;
	a2 = Q_IIR_ONE - alpha;
	int32_t n0 = b0 * a0_recip >> Q_IIR;
	int32_t n1 = b1 * a0_recip >> Q_IIR;
	int32_t n2 = b2 * a0_recip >> Q_IIR;
	int32_t d1 = -a1 * a0_recip >> (Q_IIR-1);
	int32_t d2 = -a2 * a0_recip >> Q_IIR;

	//make sure they are never zero
	filter->n0 = n0;// > 0 ? n0 : 1;
	filter->n1 = n1;// > 0 ? n1 : 1;
	filter->n2 = n2;// > 0 ? n2 : 1;
	filter->d1 = d1;// > 0 ? d1 : 1;
	filter->d2 = d2;// > 0 ? d2 : 1;
}

inline void iir_copy_params(IIR* orig, IIR* copy) {
	copy->d1 = orig->d1;
	copy->d2 = orig->d2;
	copy->n0 = orig->n0;
	copy->n1 = orig->n1;
	copy->n2 = orig->n2;
}

