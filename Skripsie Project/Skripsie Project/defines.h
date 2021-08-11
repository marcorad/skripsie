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


#define LERP
#define DUTY

//lut defs
#define B_lut 32 //precision of mapped index
#define mapped_index uint32_t
#define index uint16_t
#define MAPPED_INDEX_MAX 0xFFFFFFFF
#define HALF_MAPPED_INDEX_MAX 0x7FFFFFFF

#define b 9 //buffer size power
#define LUT_SIZE 1<<b //512
#define K (B_lut-b)


//lut Q number defs
#define Q 15
#define Qnum int16_t
#define Qnum_buf int32_t
#define Q_MAX 0x7FFF
#define HALF_Q_MAX (Q_MAX >> 1)
#define Q_MUL_ROUND   (1 << (Q - 1))

//IIR
#define Q_IIR 14
#define Q_IIR_ONE 0x3FFF
#define TRIG_BUFFER_POWER 8
#define TRIG_BUFFER_SIZE 256

//STEREO
#define STEREO_L 0
#define STEREO_R 1