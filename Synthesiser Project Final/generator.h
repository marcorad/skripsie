#pragma once
#include "LUT.h"
#include "defines.h"
#include "ADSR.h"
#include "wavetable.h"
#include "IIR.h"
#include "notes.h"
#include "waveshape.h"

#include <math.h>
#include "util.h"

//stores freq gen config to avoid any data duplication
struct gen_config {
	float detune = 0.0; //detune amount
	float detune_volume = 0; //detune volume
	float filter_freq_start_relative = 1.0f, filter_freq_end_relative = 1.0f;
	float filter_Q = ROOT_2_RECIP; //Q of filter
	float wt_pos = 0.0f;
	float vol_A = 0.01f, vol_D = 0.01f, vol_S = 1.0f, vol_R = 0.01f;
	float filt_A = 0.01f, filt_D = 0.01f, filt_S = 1.0f, filt_R = 0.01f;
	float detune_factor_up = 1.0f, detune_factor_down = 1.0f;

	//VIBRATO TO BE IMPLEMENTED
	float vibrato_freq = 0.0f; //in digital freq
	float vibrato_factor = 0.0f; //in digital freq


	//saturation
	float gain = 1.0f;
	float (*waveshape)(float, float) = &waveshape_none; //function to waveshape
	IIR_coeff filter_sat_AA;

	//FNC POINTER TO DETERMINE HOW TO CALC COEFF
	void (*filter_coeff_func)(IIR_coeff*, float, float) = &iir_calc_lp24_coeff;
	//FNC POINTER TO DETERMINE HOW TO FILTER
	//float (*filter_func)(IIR*, float) = &iir_filter_sample;
};

inline void gen_config_no_saturator(gen_config* gc) {
	gc->gain = 1.0f;
	gc->waveshape = &waveshape_none;
	iir_calc_lp12_coeff(&gc->filter_sat_AA, 0.25f, 1.0f);
}

inline void gen_config_default(gen_config* gc) {
	gc->detune = 0.0f; //detune in cents
	gc->detune_factor_up = 1.0f;
	gc->detune_factor_down = 1.0f;
	gc->detune_volume = 0.0f;
	gc->filter_freq_start_relative = 512.0f;
	gc->filter_freq_end_relative = 512.0f;
	gc->filter_Q = ROOT_2_RECIP;
	gc->filt_A = 0.01f;
	gc->filt_D = 0.01f;
	gc->filt_S = 1.0f;
	gc->vol_R = 0.01f;
	gc->vol_A = 0.01f;
	gc->vol_D = 0.01f;
	gc->vol_S = 1.0f;
	gc->vol_R = 0.01f;
	gc->vibrato_factor = 0.0f;
	gc->vibrato_freq = 0.0f;
	gen_config_no_saturator(gc);
}


//generates the notes with defined modular structure
struct generator {
	wavetable wt_left; //left detuned
	wavetable wt_right; //right detuned
	wavetable wt_center; // main freq

	wavetable wt_vibrato; //vibrato wavetable
	float vibrato_amp; //amplitude of vibrato

	IIR_coeff filter_coeff; //filter coeff
	IIR_prev_values filter_left_pv; //left filter prev values
	IIR_prev_values filter_right_pv; //right filter prev values

	ADSR envelope_volume; //volume TODO: remove adsr parameters to be global

	ADSR envelope_filter_cutoff; //cut-off	TODO: remove adsr parameters to be global
	float filter_freq_start = DIGITAL_FREQ_20KHZ, filter_freq_end = DIGITAL_FREQ_20KHZ; //actual frequency start and end ADSR paramters	
	float base_freq; //the frequency without any frequency shifting applied
	float translated_freq; // accounts for frequency shifts (vibrato ??)
	float velocity; //note volume
	float filter_envelope_amplitude = 0.0; //amplitude of cut-off modulation

	//AA saturation filters, REPLACE ONLY WITH STORAGE OF PREV SAMPLES
	IIR_prev_values filter_sat_pv_L;
	IIR_prev_values filter_sat_pv_R;
};

inline uint8_t gen_is_playing(generator* g) {
	return g->envelope_volume.state != NOT_PLAYING;
}

inline void voice_config_default(generator* g, gen_config* gc) {
	gc->detune = 0.0f;
	gc->detune_volume = 0.0f;
	g->filter_envelope_amplitude = 0.0f;
	gc->filter_Q = ROOT_2_RECIP;
	gc->filter_coeff_func = &iir_calc_lp24_coeff;
	adsr_config(&g->envelope_volume, 0.001f, 0.0f, 1.0f, 0.001f);
	adsr_config(&g->envelope_filter_cutoff, 0.001f, 0.0f, 1.0f, 0.001f);
	g->filter_freq_start = DIGITAL_FREQ_20KHZ;
	g->filter_freq_end = DIGITAL_FREQ_20KHZ;
	iir_reset(&g->filter_left_pv);
	iir_reset(&g->filter_right_pv);
}

inline void gen_vibrato(generator* g, gen_config* gc) {
	float osc = (wt_sample(&g->wt_vibrato));// sinusoid between [0,1] , which is the same as on a guitar (cannot bend down)
	float ampl = (gc->vibrato_factor - 1.0f) * g->base_freq;
	float vib = osc * ampl;
	//apply it to the wavetables without changing harmonic index
	wt_apply_fm(&g->wt_left, vib);
	wt_apply_fm(&g->wt_right, vib);
	wt_apply_fm(&g->wt_center, vib);
}

inline float gen_saturate_sample(IIR_prev_values* pv, gen_config* gc, float x) {
	float ylp = iir_filter_sample(&gc->filter_sat_AA, pv, x);
	float yws = (*gc->waveshape)(ylp, gc->gain);
	return (x - ylp) + yws;
}

//sample the voice
//CHECK IF THE VOLUME ENVELOPE IS DONE AND RESET THE FILTERS AFTERWARDS
inline void gen_sample(generator* g, gen_config* gc, float* buf_L, float* buf_R) {
	//apply vibrato
	gen_vibrato(g, gc);

	//get samples
	float sc = wt_sample(&g->wt_center);
	float sl = wt_sample(&g->wt_left);
	float sr = wt_sample(&g->wt_right);

	//blend detuned samples
	float L = sc + gc->detune_volume * sl;
	float R = sc + gc->detune_volume * sr;

	//find envelope values
	float f0 = g->filter_freq_start + adsr_sample(&g->envelope_filter_cutoff) * g->filter_envelope_amplitude;
	float volume = adsr_sample(&g->envelope_volume) * g->velocity;

	//calculate filter coefficients
	(*(gc->filter_coeff_func))(&g->filter_coeff, f0, gc->filter_Q); //TODO: filter type

	//apply volume envelope
	//this is done before so that we can partially mitigate the transient response
	L = L * volume;
	R = R * volume;

	//saturate
	L = gen_saturate_sample(&g->filter_sat_pv_L, gc, L);
	R = gen_saturate_sample(&g->filter_sat_pv_R, gc, R);

	//filter values
	L = iir_filter_sample(&g->filter_coeff, &g->filter_left_pv, L);
	R = iir_filter_sample(&g->filter_coeff, &g->filter_right_pv, R);
	
	
	*buf_L = L;
	*buf_R = R;
}

inline void gen_calc_filter_envelope_freq(generator* g, gen_config* gc) {
	g->filter_freq_start = clamp(gc->filter_freq_start_relative * g->base_freq, DIGITAL_FREQ_20HZ, DIGITAL_FREQ_20KHZ); //check if compiler precomputes THESE!
	g->filter_freq_end = clamp(gc->filter_freq_end_relative * g->base_freq, DIGITAL_FREQ_20HZ, DIGITAL_FREQ_20KHZ); //clamp to audible range
	g->filter_envelope_amplitude = g->filter_freq_end - g->filter_freq_start;
}

//note on. vel must be to midi standards in the range 0-127
//ALWAYS DO CONFIGURATION AND APPLY IT BEFORE THIS
inline void gen_freq(generator* g, gen_config* gc, float freq, uint8_t vel) {
	//set velocity
	float v = (float)vel / 127.0f;
	g->velocity = v;
	g->base_freq = freq;

	//tune wavetables
	
	wt_config_digital_freq(&g->wt_center, freq);
	wt_config_digital_freq(&g->wt_left, freq * gc->detune_factor_up);
	wt_config_digital_freq(&g->wt_right, freq * gc->detune_factor_down);

	//trigger envelopes
	adsr_trigger_on(&g->envelope_volume);
	adsr_trigger_on(&g->envelope_filter_cutoff);	

	//configure start and end filter frequencies
	gen_calc_filter_envelope_freq(g, gc); //this is also done on configuration of filter, incase it is currently playing
}

inline void gen_apply_volume_envelope_config(generator* g, gen_config* gc) {
	adsr_config(&g->envelope_volume, gc->vol_A, gc->vol_D, gc->vol_S, gc->vol_R);
}

inline void gen_apply_filter_envelope_config(generator* g, gen_config* gc) {
	adsr_config(&g->envelope_filter_cutoff, gc->filt_A, gc->filt_D, gc->filt_S, gc->filt_R);
}

inline void gen_apply_wavetable_config(generator* g, gen_config* gc) {
	g->wt_center.pos = gc->wt_pos;
	g->wt_left.pos = gc->wt_pos;
	g->wt_right.pos = gc->wt_pos;
}

inline void gen_apply_saturator_config(generator* g, gen_config* gc) {
	
}

inline void gen_config_wavetables(gen_config* gc, float pos, float detune, float detune_volume) {	
	gc->detune = detune;
	gc->detune_volume = detune_volume;
	gc->wt_pos = pos;
	float df = get_detune_factor(gc->detune);
	gc->detune_factor_up = df;
	gc->detune_factor_down = 1 / df;
}


inline void gen_config_filter(gen_config* gc, void (*coeff)(IIR_coeff*, float, float)) {
	gc->filter_coeff_func = coeff;
}


inline void gen_config_volume_envelope(gen_config* gc, float a, float d, float s, float r) {
	gc->vol_A = a;
	gc->vol_D = d;
	gc->vol_S = s;
	gc->vol_R = r;
}

//configure the adsr filter parameters with frequencies relative to the fundamental
inline void gen_config_filter_envelope(gen_config* gc, float a, float d, float s, float r, float relative_freq_start, float relative_freq_end, float q) {	
	gc->filter_freq_start_relative = relative_freq_start;
	gc->filter_freq_end_relative = relative_freq_end;
	gc->filter_Q = q;
	gc->filt_A = a;
	gc->filt_D = d;
	gc->filt_S = s;
	gc->filt_R = r;
}

inline void gen_note_on(generator* g) {
	adsr_trigger_on(&g->envelope_volume);
	adsr_trigger_on(&g->envelope_filter_cutoff);
	g->wt_vibrato.phase = 0.0f; //make sure the note starts on the correct freq
}

inline void gen_note_off(generator* g) {
	adsr_trigger_off(&g->envelope_volume);
	adsr_trigger_off(&g->envelope_filter_cutoff);
}

inline void gen_write_n_samples(generator* g, gen_config* gc, float buf_L[], float buf_R[], uint32_t n) {
	for (int i = 0; i < n; i++)
	{
		gen_sample(g, gc, buf_L + i, buf_R + i);
	}
}

//vibrato config, with freq in cycles/sample
inline void gen_config_vibrato(gen_config* gc, float amount_cents, float freq) {
	gc->vibrato_factor = get_detune_factor(amount_cents);
	gc->vibrato_freq = freq;
}

inline void gen_apply_vibrato_config(generator* g, gen_config* gc) {
	wt_config_digital_freq(&g->wt_vibrato, gc->vibrato_freq);
	g->wt_vibrato.pos = 0.0f;
}

inline void gen_config_tanh_saturator(gen_config* gc, float g) {
	gc->gain = g;
	gc->waveshape = &waveshape_tanh;
	float freq;
	if (g > 63.0f) { //a point of diminishing returns, and also to save memory
		freq = tanh_filter_cutoff[63];
	}
	else {
		freq = lut_lookup(tanh_filter_cutoff, 64, g);
	}
	iir_calc_lp12_coeff(&gc->filter_sat_AA, 0.5f * freq, 1.0f); //Q does not matter here
}








