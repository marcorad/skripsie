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
	float params[4] = {0.0f, 0.0f, 0.0f, 0.0f}; //ADSR, for efficiency, this is duplicated
	float scale, offset;
	float prev_sample = 0.0f;
	uint8_t state;
};

//MUST CHECK BEFOREHAND IF STATE IS NOT_PLAYING
inline float adsr_sample(ADSR* adsr) {
	if (adsr->state == SUSTAIN) { //this is first since it presumes that most of the note will be in sustain. This is obviously not always true.
		return adsr->params[SUSTAIN]; 
	}

	if (adsr->state != NOT_PLAYING) {
		adsr->phase += adsr->params[adsr->state];
	}

	if ((adsr->phase < (float)EXP_LUT_SIZE - 1.0f)) //detects transition to decay and sustain by detecting buffer overflow
	{
		float lookup = lut_lookup(lut_exp, EXP_LUT_SIZE, adsr->phase);
		adsr->prev_sample = adsr->scale * lookup + adsr->offset;
	}
	else {
		adsr->state += 1;
		adsr->phase = 0.0f;
		if (adsr->state == DECAY) {
			adsr->scale = adsr->params[SUSTAIN] - adsr->prev_sample; 
			adsr->offset = adsr->prev_sample;
		}
		else if (adsr->state == SUSTAIN) {
			adsr->prev_sample = adsr->params[SUSTAIN];
			adsr->scale = 1.0f;
			adsr->offset = 0.0f;
		}
	}
	
	return adsr->state != NOT_PLAYING ? adsr->prev_sample : 0.0f;
}


//ADSR times in seconds
inline void adsr_config(ADSR* adsr, float attack, float decay, float sustain, float release) {
	adsr->params[ATTACK] = attack != 0.0f ? (1.0f / attack / FS * (float)EXP_LUT_SIZE) : 0.001f; //minimum 1 ms
	adsr->params[DECAY] = decay != 0.0f ? (1.0f / decay / FS * (float)EXP_LUT_SIZE) : 0.001f;
	adsr->params[RELEASE] = release != 0.0f ? (1.0f / release / FS * (float)EXP_LUT_SIZE) : 0.001f;
	adsr->params[SUSTAIN] = sustain;
}


//reset ADSR to trigger on a note
inline void adsr_trigger_on(ADSR* adsr) {
	adsr->state = ATTACK;
	adsr->phase = 0.0f;
	adsr->scale = 1.0f - adsr->prev_sample; //for retrigger purposes, will always attack from previous sample in full attack time. This could also decrease attack time accordingly??
	adsr->offset = adsr->state != NOT_PLAYING ? adsr->prev_sample : 0.0f;
}

//reset ADSR to trigger into release
inline void adsr_trigger_off(ADSR* adsr) {
	adsr->state = RELEASE;
	adsr->phase = 0.0f;
	adsr->scale = -adsr->prev_sample; //will always decay from previous sample
	adsr->offset = adsr->prev_sample;
}

void adsr_write_n_samples(ADSR* adsr, float buf[], uint32_t size) {
	for (uint32_t i = 0; i < size; i++) {
		buf[i] = adsr_sample(adsr);
	}
}


