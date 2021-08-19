#pragma once
#include "LUT.h"
#include "defines.h"
#include "ADSR.h"
#include "wavetable.h"
#include "IIR.h"

#include <math.h>


//generates the notes with defined modular structure
struct generator {
	wavetable wt_left; //left detuned
	wavetable wt_right; //right detuned
	wavetable wt_center; // main freq
	IIR filter_left; //left filter
	IIR filter_right; //right filter
	ADSR envelope_volume; //volume
	ADSR envelope_filter; //cut-off	
	float detune = 0; //detune amount
	float detune_volume = 0; //detune volume
	float filter_envelope_start_relative = 0.5f; //starting point relative to translated freq 
	float filter_envelope_amplitude = 0.0; //amplitude of cut-off modulation
	float velocity; //note volume
	float filter_Q_recip = ROOT_2_RECIP; //Q^-1 of filter
	float freq_start = 20000.0f, freq_end = 20000.0f; //actual frequency start and end ADSR paramters
	float detune_rel = 0.0f;
	float base_freq; //the frequency without any frequency shifting applied
	float translated_freq; // accounts for frequency shifts
};

inline void voice_init_default(voice* v) {
	v->wt_left = wt_basic();
	v->wt_right = wt_basic();
	v->wt_center = wt_basic();
	v->detune = 0;
	v->detune_volume = 0;
	v->filter_envelope_start = Q_20KHZ;
	v->filter_envelope_amplitude = 0;
	v->filter_Q_recip = Q_ROOT2_RECIP;
	adsr_init(&v->envelope_volume, 0.001f, 0.0f, 1.0f, 0.001f);
	adsr_init(&v->envelope_filter, 0.001f, 0.0f, 1.0f, 0.001f);
	v->freq_start = 20000.0f;
	v->freq_end = 20000.0f;
	iir_reset(&v->filter_left);
	iir_reset(&v->filter_right);
}

//sample the voice
//CHECK IF THE VOLUME ENVELOPE IS DONE AND RESET THE FILTERS AFTERWARDS
inline void voice_sample(voice* v, Qnum* buf_L, Qnum* buf_R) {
	//get samples
	Qnum sc = wt_sample(&v->wt_center) >> 1; // make some headroom
	Qnum sl = wt_sample(&v->wt_left) >> 1;
	Qnum sr = wt_sample(&v->wt_right) >> 1;
	//blend detuned samples
	Qnum L = sc + q_mul(v->detune_volume, sl);
	Qnum R = sc + q_mul(v->detune_volume, sr);
	//find envelope values
	Qnum f0 = v->filter_envelope_start + q_mul(adsr_sample(&v->envelope_filter), v->filter_envelope_amplitude);
	Qnum volume = q_mul(adsr_sample(&v->envelope_volume), v->velocity);
	//calculate filter coefficients
	iir_calc_lp_coeff_opt(&v->filter_left, (uint16_t) f0, v->filter_Q_recip); //TODO: filter type
	iir_copy_params(&v->filter_left, &v->filter_right);
	//filter values
	L = filter_opt(&v->filter_left, L);
	R = filter_opt(&v->filter_left, R);
	//apply volume envelope
	L = q_mul(L, volume);
	R = q_mul(R, volume);
	*buf_L = L; // L;
	*buf_R = R;
}

//note on. vel must be to midi standards in the range 0-127
inline void voice_note_on(voice* v, uint8_t note, uint8_t vel) {
	v->detune = (int32_t)((float)note_stride[note] * v->detune_rel);
	wt_note_detune(&v->wt_left, note, -v->detune);
	wt_note_detune(&v->wt_right, note, v->detune);
	wt_note(&v->wt_center, note);
	adsr_note_on(&v->envelope_volume);
	adsr_note_on(&v->envelope_filter);
	v->velocity = (Qnum)vel << 8; //could velocity maybe also be exponential?
}

//note on, but also setting filter params to be relative to the note. vel must be to midi standards in the range 0-127
//the freq start and stop is normalised to the note, i.e. freq_start=1 is directly on the note
inline void voice_note_on_relative_filter_freq(voice* v, uint8_t note, uint8_t vel) {
	voice_note_on(v, note, vel);
	Qnum qstart =  (Qnum)(v->freq_start*(float)(note_stride[note] >> 16));
	Qnum qamp = (Qnum)(fminf(v->freq_end*(float)(note_stride[note] >> 16), (float)(Q_MAX - 1))) - qstart; //clamp the stop frequency
	v->filter_envelope_amplitude = qamp;
	v->filter_envelope_start = qstart;
}


//sets relative frequencies
inline void voice_configure_filter_freq_params(voice* v, float freq_start, float freq_end) {
	v->freq_start = freq_start;
	v->freq_end = freq_end;
}

inline void voice_configure_volume_envelope(voice* v, float a, float d, float s, float r) {
	adsr_init(&v->envelope_volume, a, d, s, r);
}

inline void voice_configure_freq_envelope(voice* v, float a, float d, float s, float r) {
	adsr_init(&v->envelope_filter, a, d, s, r);
}

inline void voice_configure_filter_q(voice* v, float q) {
	v->filter_Q_recip = iir_Q_float_to_int_recip(q);
}

inline void voice_configure_detune(voice* v, float vol, float detune_rel) {
	v->detune_rel = detune_rel; 
	v->detune_volume = (Qnum)((float)Q_MAX * vol);
}

inline void generate_n_voice_samples(voice* v, Qnum* buf_l, Qnum* buf_r, uint32_t N) {
	for (uint32_t i = 0; i < N; i++) {
		voice_sample(v, buf_l + i, buf_r + i);
	}
}

inline void voice_configure_duty_cycle(voice* v, float dc) {
	wt_set_dutycyle(&v->wt_center, wt_duty_cycle_from_float(dc));
	wt_set_dutycyle(&v->wt_left, wt_duty_cycle_from_float(dc));
	wt_set_dutycyle(&v->wt_right, wt_duty_cycle_from_float(dc));
}

inline void voice_configure_wt_position(voice* v, mapped_index n, float offset) {
	mapped_index pos = wt_pos_float_to_mapped_index(n, offset);
	v->wt_center.pos = pos;
	v->wt_left.pos = pos;
	v->wt_right.pos = pos;
}






