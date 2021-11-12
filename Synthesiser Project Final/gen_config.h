#pragma once

#include "defines.h"
#include "waveshape.h"
#include "IIR.h"


//stores freq gen config to avoid any data duplication
struct gen_config {
	float detune = 0.0; //detune amount (cents)
	float detune_width = 0.0f; //stereo width
	float detune_volume = 0; //detune volume
	float filter_freq_start_relative = 1.0f, filter_freq_end_relative = 1.0f; //relative start and end cutoffs
	float filter_Q = ROOT_2_RECIP; //Q of filter
	float wt_pos = 0.0f;
	//times in sec, for convenience
	float vol_A = 0.01f, vol_D = 0.01f, vol_S = 1.0f, vol_R = 0.01f;
	float filt_A = 0.01f, filt_D = 0.01f, filt_S = 1.0f, filt_R = 0.01f;
	//store stride params
	float vol_adsr_params[4];
	float filt_adsr_params[4];
	//detune freq scaling factors
	float detune_factor_up = 1.0f, detune_factor_down = 1.0f;
	//VIBRATO
	float vibrato_freq = 0.0f; //in digital freq
	float vibrato_factor = 0.0f; //in cents
	//saturation
	float gain = 1.0f;
	float (*waveshape)(float, float) = &waveshape_none; //function to waveshape
	IIR_coeff filter_sat_AA;
	//FNC POINTER TO DETERMINE HOW TO CALC COEFF
	void (*filter_coeff_func)(IIR_coeff*, float, float) = &iir_calc_lp24_coeff;
};