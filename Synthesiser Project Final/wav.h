#pragma once
/*wav_read_write.cpp
Author: K. L. Srinivas
Problem statement :
1. opens a wav file for reading; opens another wav file for writing
2. detects the samp. rate and #samples from the header of input wav

Next, we wish to read in sequentially blocks of data from the wav file
for processing one block ("frame") at a time.

In the loop (until last frame):
 3. reads in the data (16-bit integers) in blocks of BUFSIZE samples
 4. dummy processing()
 5. writes out BUFSIZE output samples in the output wav file
end loop
*/

// This program is tested on linux machine with g++ compiler.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include <fstream>



// WAVE PCM soundfile format (you can find more in http://soundfile.sapp.org/doc/WaveFormat/ )
typedef struct header_file
{
	char chunk_id[4] = { 'R','I','F','F'};
	int chunk_size; //TO BE FILLED IN:  36 + SubChunk2Size
	char format[4] = { 'W','A','V','E' };
	char subchunk1_id[4] = { 'f','m','t',' ' };
	int subchunk1_size = 16; //16 for PCM
	short int audio_format = 1; // PCM = 1
	short int num_channels = 2; //stereo
	int sample_rate;			//TO BE FILLED IN: sample_rate denotes the sampling rate.
	int byte_rate; //TO BE FILLED IN: SampleRate * NumChannels * BitsPerSample/8
	short int block_align = 2 * 16 / 8; //NumChannels * BitsPerSample/8
	short int bits_per_sample = 16; //bits per sample
	char subchunk2_id[4] = { 'd','a','t','a' };
	int subchunk2_size;			//TO BE FILLED IN: NumSamples * NumChannels * BitsPerSample/8
} header;

typedef struct header_file* header_p;

void write_to_wav(const std::string& name, float bufL[], float bufR[], int num_samples, int fs) {

	using namespace std;
	header head;
	head.sample_rate = fs;
	head.byte_rate = fs * 2 * 16 / 8;
	head.subchunk2_size = num_samples * 2 * 16 / 8;
	head.chunk_size = 36 + head.subchunk2_size;

	ofstream f;
	f.open("..\\wav\\" + name + ".wav", ios::binary);
	f.write(reinterpret_cast<char*> (&head), sizeof(header));


	for (int i = 0; i < num_samples; i++) {
		short int s = (int)(bufL[i] * (float)INT16_MAX);
		f.write(reinterpret_cast<char*> (&s), sizeof(short int));
		s = (int)(bufR[i] * (float)INT16_MAX);
		f.write(reinterpret_cast<char*> (&s), sizeof(short int));
	}

	f.close();

}