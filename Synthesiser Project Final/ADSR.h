#pragma once
#include "LUT.h"
#include "init_luts.h"

#define ATTACK 0
#define DECAY 1
#define SUSTAIN 2
#define RELEASE 3
#define NOT_PLAYING 4


struct ADSR {
	float phase = 0.0f;
	float scale, offset;
	float prev_sample = 0.0f;
	uint8_t state;
};

//MUST CHECK BEFOREHAND IF STATE IS NOT_PLAYING
inline float adsr_sample(ADSR* adsr, float params[]) {
	if (adsr->state == SUSTAIN) { //expecting notes to spend most time in sustain
		return params[SUSTAIN]; 
	}	

	if ((adsr->phase < (float)EXP_LUT_SIZE - 1.0f)) //next phase from buffer wrap
	{
		float lookup = lut_lookup_no_wrap(lut_exp, adsr->phase);
		adsr->prev_sample = adsr->scale * lookup + adsr->offset;
	}
	else {
		adsr->state += 1;
		adsr->phase = 0.0f;
		if (adsr->state == DECAY) {
			adsr->scale = params[SUSTAIN] - adsr->prev_sample; 
			adsr->offset = adsr->prev_sample;
		}
		else if (adsr->state == SUSTAIN) {
			adsr->prev_sample = params[SUSTAIN];
			return params[SUSTAIN];
		}
	}

	if (adsr->state != NOT_PLAYING) {
		adsr->phase += params[adsr->state];
		return adsr->prev_sample;
	}
	else {
		adsr->prev_sample = 0.0f;		
	}

	return adsr->prev_sample;
}


//ADSR times in seconds
inline void adsr_config(float params[], float attack, float decay, float sustain, float release) {
	float min = 1.0f / 0.001f / FS * (float)EXP_LUT_SIZE;
	//strides
	params[ATTACK] = attack != 0.0f ? (1.0f / attack / FS * (float)EXP_LUT_SIZE) : min; //minimum 1 ms
	params[DECAY] = decay != 0.0f ? (1.0f / decay / FS * (float)EXP_LUT_SIZE) : min;
	params[RELEASE] = release != 0.0f ? (1.0f / release / FS * (float)EXP_LUT_SIZE) : min;
	//sustain level
	params[SUSTAIN] = sustain;
}


//reset ADSR to trigger on a note
inline void adsr_trigger_on(ADSR* adsr) {
	adsr->state = ATTACK;
	adsr->phase = 0.0f;
	adsr->scale = 1.0f - adsr->prev_sample; //attack from previous sample (for retrigger purposes)
	adsr->offset = adsr->state != NOT_PLAYING ? adsr->prev_sample : 0.0f;
}

//reset ADSR to trigger into release
inline void adsr_trigger_off(ADSR* adsr) {
	adsr->state = RELEASE;
	adsr->phase = 0.0f;
	adsr->scale = -adsr->prev_sample; //will always decay from previous sample
	adsr->offset = adsr->prev_sample;
}

void adsr_write_n_samples(ADSR* adsr, float buf[], uint32_t size, float adsr_params[]) {
	for (uint32_t i = 0; i < size; i++) {
		buf[i] = adsr_sample(adsr, adsr_params);
	}
}

inline void adsr_reset(ADSR* adsr) {
	adsr->phase = 0.0f;
	adsr->prev_sample = 0.0f;
}


