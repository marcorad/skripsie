#pragma once

#include "defines.h"
#include "generator.h"

struct gen_manager {
	generator generators[NUM_GENERATORS];
	generator* available[NUM_GENERATORS];
	uint8_t available_head = 0;
	generator* in_use[NUM_GENERATORS];
	uint8_t in_use_head = 0;
};