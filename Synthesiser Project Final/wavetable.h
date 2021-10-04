#pragma once

#include "LUT.h"
#include "init_luts.h"
#include "gen_config.h"




struct wavetable {
	float stride = 0.0f;
	float phase = 0.0f; //wt pos pointer
	float base_stride; //stride without FM, i.e. center freq
	uint8_t harmonic_index = 0; // index for num harmonics
};

//sample from the wavetable
inline float wt_sample_no_interpolation(wavetable* wt, uint8_t pos) {
	uint8_t n = pos;
	float x1 = lut_lookup(basic_luts[n][wt->harmonic_index], LUT_SIZE, wt->phase);
	wt->phase += wt->stride;
	if (wt->phase > (float)LUT_SIZE) wt->phase -= (float)LUT_SIZE; //wrap around
	return x1;
}

//sample from the wavetable
inline float wt_sample(wavetable* wt, gen_config* gc) {
	uint8_t n = (uint8_t)gc->wt_pos;
	uint8_t np1 = n + 1;
	np1 = fast_mod(np1, 4); // wrap around
	float x1 = lut_lookup(basic_luts[n][wt->harmonic_index], LUT_SIZE, wt->phase);
	float x2 = lut_lookup(basic_luts[np1][wt->harmonic_index], LUT_SIZE, wt->phase);
	wt->phase += wt->stride;
	if (wt->phase > (float)LUT_SIZE) wt->phase -= (float)LUT_SIZE; //wrap around
	return lerp(x1, x2, gc->wt_pos - (float)n);
}

//get n samples from the wavetable
void wt_write_n_samples(float buffer[], wavetable* wt, gen_config* gc, int N) {
	for (int i = 0; i < N; i++) {
		buffer[i] = wt_sample(wt, gc);
	}
}

uint8_t harmonic_indices[] = {1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7}; // floor(log(n/4)), where n is index


//configure in samples/sec and determin correct number of harmonics to avoid aliasing
inline void wt_config_digital_freq(wavetable* wt, float freq) {
	wt->stride = freq * (float)LUT_SIZE;
	wt->base_stride = wt->stride;
	uint16_t harmonics = (uint16_t)(0.5f / freq);
	harmonics = harmonics > (LUT_SIZE >> 1) ? (LUT_SIZE >> 1) : harmonics; //clamp at max allowed by buffer
	uint8_t harmonic_index = harmonics < 4 ? 0 : harmonic_indices[(harmonics >> 2) - 1]; //divide harmonic index by 4, see excel, minus 1 for shifting excel index to 0
	wt->harmonic_index = harmonic_index;
}

//configure in Hz
inline void wt_config_hz(wavetable* wt, float freq) {
	wt_config_digital_freq(wt, freq / FS); //FS recip?
}

//apply fm to wavetable without changin harmonic index, by a change in digital frequency df
inline void wt_apply_fm(wavetable* wt, float df) {
	wt->stride = wt->base_stride + df * (float) LUT_SIZE;
}



