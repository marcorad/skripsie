#pragma once
//GENERAL
#define SINE 0
#define TRIANGLE 1
#define SAWTOOTH 2
#define SQUARE 3


#define PI 3.14159265359f
#define TWO_PI 6.28318530718f
#define PI_INV 0.31830988618f

#define MAX_ADSR_TIME 5.0f //seconds
#define FS 44100.0f //sample rate
#define FS_RECIP 2.2675736961451248e-05f


#define LERP
#define DUTY

#define LUT_SIZE 512 
#define LUT_SIZE_POWER 9
#define EXP_LUT_SIZE 128

//STEREO
#define STEREO_L 0
#define STEREO_R 1

#define TWELFTH_ROOT_TWO 1.05946309436f

#define DIGITAL_FREQ_20KHZ (20000.0f/FS)
#define DIGITAL_FREQ_20HZ (20.0f/FS)
#define ROOT_2_RECIP 0.70710678118f

#define NUM_GENERATORS 8