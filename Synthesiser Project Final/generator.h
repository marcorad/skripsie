#pragma once
#include "LUT.h"
#include "defines.h"
#include "ADSR.h"
#include "wavetable.h"
#include "IIR.h"
#include "notes.h"

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

	//TO BE IMPLEMENTED
	float vibrato_amount = 0.0f; //in cents
	float vibrato_freq = 0.0f; //in digital freq
	float vibrato_factor = 0.0f; //in digital freq
};

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
}


//generates the notes with defined modular structure
struct generator {
	wavetable wt_left; //left detuned
	wavetable wt_right; //right detuned
	wavetable wt_center; // main freq
	IIR filter_left; //left filter
	IIR filter_right; //right filter
	ADSR envelope_volume; //volume
	ADSR envelope_filter_cutoff; //cut-off	
	float filter_freq_start = DIGITAL_FREQ_20KHZ, filter_freq_end = DIGITAL_FREQ_20KHZ; //actual frequency start and end ADSR paramters	
	float base_freq; //the frequency without any frequency shifting applied
	float translated_freq; // accounts for frequency shifts (vibrato ??)
	float velocity; //note volume
	float filter_envelope_amplitude = 0.0; //amplitude of cut-off modulation
};

inline uint8_t gen_is_playing(generator* g) {
	return g->envelope_volume.phase != NOT_PLAYING;
}

inline void voice_config_default(generator* g, gen_config* gc) {
	gc->detune = 0.0f;
	gc->detune_volume = 0.0f;
	g->filter_envelope_amplitude = 0.0f;
	gc->filter_Q = ROOT_2_RECIP;
	adsr_config(&g->envelope_volume, 0.001f, 0.0f, 1.0f, 0.001f);
	adsr_config(&g->envelope_filter_cutoff, 0.001f, 0.0f, 1.0f, 0.001f);
	g->filter_freq_start = DIGITAL_FREQ_20KHZ;
	g->filter_freq_end = DIGITAL_FREQ_20KHZ;
	iir_reset(&g->filter_left);
	iir_reset(&g->filter_right);
}

//sample the voice
//CHECK IF THE VOLUME ENVELOPE IS DONE AND RESET THE FILTERS AFTERWARDS
inline void gen_sample(generator* g, gen_config* gc, float* buf_L, float* buf_R) {
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
	iir_calc_lp_coeff(&g->filter_left,  f0, gc->filter_Q); //TODO: filter type
	iir_copy_coeff(&g->filter_left, &g->filter_right);

	//apply volume envelope
	//this is done before so that we can partially mitigate the transient response
	L = L * volume;
	R = R * volume;

	//filter values
	L = iir_filter_sample(&g->filter_left, L);
	R = iir_filter_sample(&g->filter_right, R);
	
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

inline void gen_config_wavetables(gen_config* gc, float pos, float detune, float detune_volume) {	
	gc->detune = detune;
	gc->detune_volume = detune_volume;
	gc->wt_pos = pos;
	float df = get_detune_factor(gc->detune);
	gc->detune_factor_up = df;
	gc->detune_factor_down = 1 / df;
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






