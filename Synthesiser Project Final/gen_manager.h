#pragma once

#include "defines.h"
#include "generator.h"

struct gen_manager {
	generator generators[NUM_GENERATORS];
	generator* available[NUM_GENERATORS]; 
	int8_t available_head = NUM_GENERATORS;
	generator* in_use[NUM_GENERATORS]; 
	uint8_t in_use_head = 0;
};

inline void gm_add_to_in_use(gen_manager* gm, generator* g) {
	gm->in_use[gm->in_use_head] = g;
	gm->in_use_head = gm->in_use_head + 1;
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
	if (!gm->in_use_head == 0) {
		//pop off front of available
		g = gm_pop_off_front_from_available(gm);
	}
	else {//none are available (this should be a rare case of key-mashing)
		g = gm_pop_off_back_from_in_use(gm);		
	}

	gm_add_to_in_use(gm, g);	

	return g;
}

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


