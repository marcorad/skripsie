#pragma once
#include "LUT.h"
#include "predefined_luts.h"

#define ATTACK 0
#define DECAY 1
#define SUSTAIN 2
#define RELEASE 3
#define NOT_PLAYING 4

#define ADSR_FINAL_SAMPLE (0x1FF << (K))



//uses overflow to detect new state
//attack always goes to Q_MAX (1 in Q15 notation)
struct ADSR {
	mapped_index phase;
	mapped_index* strides = new mapped_index[0, 0, 0, 0]; //3rd position never used
	Qnum sustain;
	Qnum scale, offset;
	Qnum prev_sample;
	uint8_t state;
	LUT* exp_decay = exp_decay;
};

//MUST CHECK BEFOREHAND IF STATE IS NOT_PLAYING
inline Qnum adsr_sample(ADSR* adsr) {
	if (adsr->state == SUSTAIN) { return adsr->sustain; } //most notes will spend majority of their time here

	mapped_index prev_phase = adsr->phase;
	adsr->phase += adsr->strides[adsr->state];

	if ((adsr->phase <= ADSR_FINAL_SAMPLE) && (prev_phase < adsr->phase)) //detects transition to decay and sustain by detecting wrapping to following sample OR overflow
	{
		Qnum lookup = lut_lookup(adsr->exp_decay, adsr->phase);
		adsr->prev_sample = q_mul(adsr->scale, lookup) + adsr->offset;
	}
	else {
		adsr->state += 1;
		adsr->phase = 0;
		if (adsr->state == DECAY) {
			adsr->scale = adsr->sustain - adsr->prev_sample; 
			adsr->offset = adsr->prev_sample;
		}
		else if (adsr->state == SUSTAIN) {
			adsr->prev_sample = adsr->sustain;
			adsr->scale = Q_MAX;
			adsr->offset = 0;
		}
	}
	
	return adsr->prev_sample;
}


//ADSR times in seconds
//This has maximum Q15 amplitude (Q_MAX)
inline void adsr_init(ADSR* adsr, float attack, float decay, float sustain, float release) {
	//wraps a time of zero to the final sample
	adsr->strides[ATTACK] = attack != 0.0f ? (mapped_index)(1.0f / attack / FS * (float)MAPPED_INDEX_MAX) : ADSR_FINAL_SAMPLE;
	adsr->strides[DECAY] = decay != 0.0f ? (mapped_index)(1.0f / decay / FS * (float)MAPPED_INDEX_MAX) : ADSR_FINAL_SAMPLE;
	adsr->strides[RELEASE] = release != 0.0f ? (mapped_index)(1.0f / release / FS * (float)MAPPED_INDEX_MAX) : ADSR_FINAL_SAMPLE;
	adsr->sustain = (Qnum)((float)Q_MAX * sustain);
	adsr->exp_decay = &exp_decay;
}


//reset ADSR to trigger on a note
inline void adsr_note_on(ADSR* adsr) {
	adsr->state = ATTACK;
	adsr->phase = 0;
	adsr->scale = Q_MAX;
	adsr->offset = 0;
}

//reset ADSR to trigger into release
inline void adsr_off(ADSR* adsr) {
	adsr->state = RELEASE;
	adsr->phase = 0;
	adsr->scale = -adsr->prev_sample; //will always decay from previous sample
	adsr->offset = adsr->prev_sample;
}

void adsr_write_n_samples(ADSR* adsr, Qnum* buf, uint16_t N) {
	for (uint16_t i = 0; i < N; i++) {
		if (adsr->state != NOT_PLAYING) {
			buf[i] = adsr_sample(adsr);
		}
		else {
			buf[i] = 0;
		}
	}
}

