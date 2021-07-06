#pragma once

#include "LUT.h"
#include "predefined_luts.h"
#include "init_luts.h"


struct wavetable {
	uint8_t table_harmonic_index = 0;
	mapped_index pos; //wavetable position mapped from 0 to 2^32-1
	mapped_index stride;
	mapped_index phase = 0; //the mapped position in the wavetable
	mapped_index duty_cycle = HALF_MAPPED_INDEX_MAX;
	float duty_cycle_m1 = 1.0f;
	float duty_cycle_m2 = 1.0f;
	uint8_t num_tables_power;
	uint8_t num_tables;
	LUT** luts;
};

//n'th LUT and a position from [0,1)
inline mapped_index wt_float_to_mapped_index(mapped_index n, float pos) {
	mapped_index offset = (mapped_index)((float)(1 << (B_lut - 2)) * pos);
	return offset + (n << (B_lut - 2));
}

inline mapped_index wt_duty_cycle_from_float(float d) {
	return (mapped_index)((float)(MAPPED_INDEX_MAX)*d);
}

//could this be faster?
inline mapped_index duty_cycle_index(wavetable* wt) {
	if (wt->table_harmonic_index > 2) {
		mapped_index dc = wt->duty_cycle;
		mapped_index i = wt->phase;
		if (i <= dc) {
			return (mapped_index)(wt->duty_cycle_m1 * (float)(i));
		}
		else {
			return (mapped_index)(wt->duty_cycle_m2 * (float)(i - dc)) + HALF_MAPPED_INDEX_MAX;
		}
	}
	else {
		return wt->phase;
	}
}


inline Qnum wt_sample(wavetable* wt) {
	//find luts and interpolation
	uint8_t shift = B_lut - wt->num_tables_power;
	mapped_index i = wt->pos;
	uint8_t n =  i >> shift;
	mapped_index i_floor = n << shift;
	uint8_t np1 = fast_mod(n + 1, wt->num_tables); //should not wrap. This must be ensured.
	LUT lut1 = wt->luts[n][wt->table_harmonic_index];
	LUT lut2 = wt->luts[np1][wt->table_harmonic_index];
		
#ifdef DUTY
	mapped_index dc_lookup = duty_cycle_index(wt);
#endif // DUTY

#ifndef DUTY
	mapped_index dc_lookup = wt->phase;
#endif // !DUTY
	
	Qnum s1 = lut_lookup(&lut1, dc_lookup);
	Qnum s2 = lut_lookup(&lut2, dc_lookup);

	wt->phase += wt->stride;

	//interpolate
	Qnum d = (Qnum) ((i - i_floor) >> (shift - Q)); //mapped index to q num
	return q_lerp(s1, s2, d);
}

void generate_n_samples(Qnum* buffer, wavetable* wt, int N) {
	for (int i = 0; i < N; i++) {
		buffer[i] = wt_sample(wt);
	}
}

inline void wt_note(wavetable* wt, uint8_t note, mapped_index wt_pos) {
	wt->table_harmonic_index = harmonic_table_indices[note];
	wt->stride = note_stride[note];
	wt->pos = wt_pos;
}

inline void wt_set_dutycyle(wavetable* wt, mapped_index duty_cycle) {
	wt->duty_cycle = duty_cycle;
	wt->duty_cycle_m1 = (float)HALF_MAPPED_INDEX_MAX / (float)(duty_cycle);
	wt->duty_cycle_m2 = (float)HALF_MAPPED_INDEX_MAX / (float)(MAPPED_INDEX_MAX - duty_cycle);
}


inline wavetable load_basic_wavetable() {
	LUT lut_exp = create_LUT(6);
	int N_luts = 8;//2 3 4 5 6 7 8 9
	LUT* lut_saw = create_luts();
	LUT* lut_tri = create_luts();
	LUT* lut_square = create_luts();
	LUT* lut_sin = create_luts();

	LUT** luts = new LUT * [4]{ lut_sin , lut_tri , lut_saw , lut_square };

	load_basic_luts(lut_sin, lut_tri, lut_saw, lut_square, N_luts);

	wavetable wt;
	wt.luts = luts;
	wt.num_tables_power = 2;
	wt.num_tables = 4;
	return wt;
}
