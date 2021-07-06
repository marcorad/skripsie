#pragma once
#include "LUT.h"
#include "defines.h"
#include "ADSR.h"
#include "wavetable.h"
#include "IIR.h"

struct voice {
	wavetable wt_left;
	wavetable wt_right;
	wavetable wt_center;
	IIR filter_left;
	IIR filter_left;
	ADSR envelope_amplitude;
	ADSR envelope_filter;
	mapped_index detune;
};
