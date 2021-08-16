#pragma once
#include "LUT.h"
#include "wavetable.h"

LUT *lut_saw, *lut_tri, *lut_square, *lut_sin;

void init_basic_luts() {
	lut_saw = create_luts();
	lut_tri = create_luts();
	lut_square = create_luts();
	lut_sin = create_luts();
}


