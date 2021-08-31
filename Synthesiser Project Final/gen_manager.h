#pragma once

#include "defines.h"
#include "generator.h"

generator* note_played_hash_table[128]; //keep track of which note is played, for O(1) time on on-off triggers.

struct gen_manager {
	generator generators[NUM_GENERATORS];
	generator* available[NUM_GENERATORS]; 
	int8_t available_head = NUM_GENERATORS;
	generator* in_use[NUM_GENERATORS]; 
	uint8_t in_use_head = 0;
};

inline void gm_init(gen_manager* gm) {
	//make all available
	for (int i = 0; i < NUM_GENERATORS; i++)
	{
		gm->available[i] = &gm->generators[i];
	}
	gm->available_head = NUM_GENERATORS;
	gm->in_use_head = 0;
	//none are in use
	for (int i = 0; i < NUM_GENERATORS; i++)
	{
		gm->in_use[i] = nullptr;
	}
	//none are playing notes
	for (int i = 0; i < 128; i++) {
		note_played_hash_table[i] = nullptr;
	}
}

inline void gm_add_to_in_use(gen_manager* gm, generator* g) {
	gm->in_use[gm->in_use_head] = g;
	gm->in_use_head++;
}

inline generator* gm_pop_off_back_from_in_use(gen_manager* gm) {
	generator* g = gm->in_use[0];
	//shift queue
	for (int i = 0; i < gm->in_use_head; i++)
	{
		gm->in_use[i] = gm->in_use[i + 1];
	}
	gm->in_use_head--;
	return g;
}

inline void gm_add_to_available(gen_manager* gm, generator* g) {
	gm->available[gm->available_head] = g;
	gm->available_head++;
}

inline generator* gm_pop_off_front_from_available(gen_manager* gm) {
	gm->available_head--;
	return gm->available[gm->available_head];	
}

//get an available or gen, or the oldest playing gen if none are available
inline generator* gm_get_gen(gen_manager* gm) {
	generator* g;
	if (!gm->available_head == 0) {
		//pop off front of available
		g = gm_pop_off_front_from_available(gm);
	}
	else {//none are available (this should be a rare case of key-mashing)
		g = gm_pop_off_back_from_in_use(gm);		
	}

	gm_add_to_in_use(gm, g);	

	return g;
}

//add all non-playing gens to the available list in O(N) time.
inline void gm_make_not_playing_available(gen_manager* gm) {	
	int count = 0;
	for (int i = 0; i < gm->in_use_head; i++)
	{
		generator* g = gm->in_use[i];
		if (!gen_is_playing(g)) {
			count++;
			gm_add_to_available(gm, g);
		}
		else {
			gm->in_use[i - count] = g;
		}
	}
	gm->in_use_head -= count;
}

inline generator* gm_get_gen_playing_note(uint8_t midi_note) {
	return note_played_hash_table[midi_note];
}

inline void gm_set_gen_playing_note(generator* g, uint8_t midi_note) {
	note_played_hash_table[midi_note] = g;
}

float L_temp[PLAYBACK_BUFFER_SIZE];
float R_temp[PLAYBACK_BUFFER_SIZE];

inline void gm_write_n_samples(gen_manager* gm, gen_config* gc, float bufL[], float bufR[], uint32_t n) {

	for (int i = 0; i < PLAYBACK_BUFFER_SIZE; i++) //init to zero for addition later
	{
		bufL[i] = 0.0f;
		bufR[i] = 0.0f;
	}
	for (int i = 0; i < gm->in_use_head; i++) //for all active voices
	{
		gen_write_n_samples(gm->in_use[i], gc, L_temp, R_temp, n); //write to temp
		for (int i = 0; i < PLAYBACK_BUFFER_SIZE; i++) //for all samples in each voice
		{
			bufL[i] += 0.125f * L_temp[i]; //1/8th for 8 voices to ensure headroom
			bufR[i] += 0.125f * R_temp[i];
		}
	}	
}

inline void gm_trigger_note_on(gen_manager* gm, gen_config* gc, uint8_t note, uint8_t vel){
	generator* g = gm_get_gen_playing_note(note);
	if (g == nullptr) g = gm_get_gen(gm); //prevents any weirdness in retriggers of notes before a note off
	gen_note_on(g);
	gen_freq(g, gc, notes_digital_freq[note], vel);
	gm_set_gen_playing_note(g, note);
}

inline void gm_trigger_note_off(gen_manager* gm, uint8_t note) {
	generator* g = gm_get_gen_playing_note(note);
	gm_set_gen_playing_note(nullptr, note);
	if (g != nullptr) gen_note_off(g); //prevents any weirdness in note off triggers if it's not actually on
}

inline void gm_apply_volume_envelope_config(gen_manager* gm, gen_config* gc) {
	for (int i = 0; i < NUM_GENERATORS; i++) {
		gen_apply_volume_envelope_config(&gm->generators[i], gc);
	}
}

inline void gm_apply_filter_envelope_config(gen_manager* gm, gen_config* gc) {
	for (int i = 0; i < NUM_GENERATORS; i++) {
		gen_apply_filter_envelope_config(&gm->generators[i], gc);
	}
}

inline void gm_apply_wavetable_config(gen_manager* gm, gen_config* gc) {
	for (int i = 0; i < NUM_GENERATORS; i++) {
		gen_apply_wavetable_config(&gm->generators[i], gc);
	}
}


