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
#include "gen_config.h"


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
	gc->vibrato_factor = 0.0f;
	gc->vibrato_freq = 0.0f;
	//TODO: ADSR
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

	//the currently playing note
	uint8_t midi_note;
};

inline uint8_t gen_is_playing(generator* g) {
	return g->envelope_volume.state != NOT_PLAYING;
}


inline void gen_vibrato(generator* g, gen_config* gc) {
	float osc = (wt_sample_no_interpolation(&g->wt_vibrato, 0));
	float ampl = (gc->vibrato_factor - 1.0f) * g->base_freq;
	float vib = osc * ampl;
	//apply it to the wavetables without changing harmonic index
	wt_apply_fm(&g->wt_left, vib);
	wt_apply_fm(&g->wt_right, vib);
	wt_apply_fm(&g->wt_center, vib);
}

inline float gen_waveshape_sample(IIR_prev_values* pv, gen_config* gc, float x) {
	float ylp = iir_filter_sample(&gc->filter_sat_AA, pv, x);
	float yws = (*gc->waveshape)(ylp, gc->gain);
	return (x - ylp) + yws;
}

//sample the voice
//CHECK IF THE VOLUME ENVELOPE IS DONE AND RESET THE FILTERS AFTERWARDS
inline void gen_sample(generator* g, gen_config* gc, float* buf_L, float* buf_R) {
	//apply vibrato using FM
	gen_vibrato(g, gc);

	//sample L, R, C channels
	float sc = wt_sample(&g->wt_center, gc);
	float sl = wt_sample(&g->wt_left, gc);
	float sr = wt_sample(&g->wt_right, gc);

	//blend detuned samples
	float width = 1.0f - gc->detune_width; //invert to get volume in other channel
	//detune_width of 1 seperates sr and sl into L and R channels
	//detune_width of 0 is mono
	float L = sc + gc->detune_volume * (sl + width * sr);
	float R = sc + gc->detune_volume * (sr + width * sl);

	//find f0 using envelope
	float f0 = g->filter_freq_start + adsr_sample(&g->envelope_filter_cutoff, gc->filt_adsr_params) * g->filter_envelope_amplitude;
	f0 = clamp(f0, DIGITAL_FREQ_20HZ, DIGITAL_FREQ_20KHZ); //limit to audible range (otherwise there are clicks)

	//calculate filter coefficients
	(*(gc->filter_coeff_func))(&g->filter_coeff, f0, gc->filter_Q);

	//waveshape L and R
	L = gen_waveshape_sample(&g->filter_sat_pv_L, gc, L);
	R = gen_waveshape_sample(&g->filter_sat_pv_R, gc, R);

	//apply volume and envelope
	//this is done before filtering so that we can partially mitigate the transient response, since filtering is linear
	float volume = adsr_sample(&g->envelope_volume, gc->vol_adsr_params) * g->velocity;
	L = L * volume;
	R = R * volume;	

	//filter channels
	L = iir_filter_sample(&g->filter_coeff, &g->filter_left_pv, L);
	R = iir_filter_sample(&g->filter_coeff, &g->filter_right_pv, R);	
	
	//output
	*buf_L = L;
	*buf_R = R;
}

inline void gen_calc_filter_envelope_freq(generator* g, gen_config* gc) {
	g->filter_freq_start = gc->filter_freq_start_relative * g->base_freq;
	g->filter_freq_end = gc->filter_freq_end_relative * g->base_freq;
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



inline void gen_config_wavetables(gen_config* gc, float pos, float detune, float detune_volume, float detune_width) {	
	gc->detune = detune;
	gc->detune_volume = detune_volume;
	gc->detune_width = detune_width;
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
	adsr_config(gc->vol_adsr_params, a, d, s, r);
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
	adsr_config(gc->filt_adsr_params, a, d, s, r);
}

inline void gen_note_on(generator* g) {

	adsr_reset(&g->envelope_filter_cutoff); //reset filter in case of retrigger

	adsr_trigger_on(&g->envelope_volume);
	adsr_trigger_on(&g->envelope_filter_cutoff); //possible retrigger
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








