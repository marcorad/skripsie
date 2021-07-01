#pragma once

#include "LUT.h"

struct ADSR {
	mapped_index phase;
	mapped_index A, D, S, R;
};

struct wavetable {
	uint8_t table_harmonic_index = 0;
	mapped_index stride;
	mapped_index phase = 0; //the mapped position in the wavetable
	LUT** luts;
};

void generate_n_samples(Qnum* buffer, wavetable* wt, int N, int wave) {
	LUT* table = wt->luts[wave];
	LUT lut = table[wt->table_harmonic_index];
	for (int i = 0; i < N; i++) {
		buffer[i] = lerp_lut(lut, wt->phase);
		wt->phase += wt->stride;
	}
}
