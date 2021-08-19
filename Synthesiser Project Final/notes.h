#pragma once
#include "defines.h"
#include "includes.h"
#include <math.h>

#define A0		21
#define Bb0		22
#define B0		23
#define C1		24
#define Db1		25
#define D1		26
#define Eb1		27
#define E1		28
#define F1		29
#define Gb1		30
#define G1		31
#define Ab1		32
#define A1		33
#define Bb1		34
#define B1		35
#define C2		36
#define Db2		37
#define D2		38
#define Eb2		39
#define E2		40
#define F2		41
#define Gb2		42
#define G2		43
#define Ab2		44
#define A2		45
#define Bb2		46
#define B2		47
#define C3		48
#define Db3		49
#define D3		50
#define Eb3		51
#define E3		52
#define F3		53
#define Gb3		54
#define G3		55
#define Ab3		56
#define A3		57
#define Bb3		58
#define B3		59
#define C4		60
#define Db4		61
#define D4		62
#define Eb4		63
#define E4		64
#define F4		65
#define Gb4		66
#define G4		67
#define Ab4		68
#define A4		69
#define Bb4		70
#define B4		71
#define C5		72
#define Db5		73
#define D5		74
#define Eb5		75
#define E5		76
#define F5		77
#define Gb5		78
#define G5		79
#define Ab5		80
#define A5		81
#define Bb5		82
#define B5		83
#define C6		84
#define Db6		85
#define D6		86
#define Eb6		87
#define E6		88
#define F6		89
#define Gb6		90
#define G6		91
#define Ab6		92
#define A6		93
#define Bb6		94
#define B6		95
#define C7		96
#define Db7		97
#define D7		98
#define Eb7		99
#define E7		100
#define F7		101
#define Gb7		102
#define G7		103
#define Ab7		104
#define A7		105
#define Bb7		106
#define B7		107
#define C8		108
#define Db8		109
#define D8		110
#define Eb8		111
#define E8		112
#define F8		113
#define Gb8		114
#define G8		115
#define Ab8		116
#define A8		117
#define Bb8		118
#define B8		119
#define C9		120
#define Db9		121
#define D9		122
#define Eb9		123
#define E9		124
#define F9		125
#define Gb9		126
#define G9		127

float notes_freq_hz[] =
{ 8.18f,8.66f,9.18f,9.72f,10.3f,10.91f,
11.56f,12.25f,12.98f,13.75f,14.57f,15.43f,
16.35f,17.32f,18.35f,19.45f,20.6f,21.83f,
23.12f,24.5f,25.96f,27.5f,29.14f,30.87f,32.7f,
34.65f,36.71f,38.89f,41.2f,43.65f,46.25f,49.0f,
51.91f,55.0f,58.27f,61.74f,65.41f,69.3f,73.42f,77.78f,82.41f,87.31f,92.5f,
98.0f,103.83f,110.0f,116.54f,123.47f,130.81f,138.59f,146.83f,155.56f,164.81f,
174.61f,185.f,196.0f,207.65f,220.0f,233.08f,246.94f,261.63f,277.18f,293.66f,
311.13f,329.63f,349.23f,369.99f,392.0f,415.3f,440.0f,466.16f,493.88f,523.25f,554.37f,
587.33f,622.25f,659.26f,698.46f,739.99f,783.99f,830.61f,880.0f,932.33f,987.77f,1046.5f,
1108.73f,1174.66f,1244.51f,1318.51f,1396.91f,1479.98f,1567.98f,1661.22f,1760.0f,1864.66f,
1975.53f,2093.0f,2217.46f,2349.32f,2489.02f,2637.02f,2793.83f,2959.96f,3135.96f,3322.44f,
3520.0f,3729.31f,3951.07f,4186.01f,4434.92f,4698.64f,4978.03f,5274.04f,5587.65f,5919.91f,
6271.93f,6644.88f,7040.0f,7458.62f,7902.13f,8372.02f,8869.84f,9397.27f,9956.06f,10548.08f,
11175.3f,11839.82f,12543.85f };

float notes_digital_freq[128];

void init_notes_digital_freq_buffer() {
	for (int i = 0; i < 128; i++) {
		notes_digital_freq[i] = notes_freq_hz[i] / FS; //calc the buffer to allow for changes in FS
	}
}

inline float get_cents_detune(float orig_freq, float cents) {
	return orig_freq * powf(TWELFTH_ROOT_TWO, cents / 100.0f); // div by 100 to achieve 2^(1/1200)
}

//ref frequency for the note, the range in semi tones and the midi 14bit value
float get_pitch_bend_digital_freq(float orig_freq, float range, uint16_t midi_pb) {
	float pb_norm = 2.0f * (float)midi_pb / (float)0x3FFF - 1.0f; //max 14 bits, normalised to [-1,1]
	return orig_freq * powf(TWELFTH_ROOT_TWO, pb_norm * range);
}
