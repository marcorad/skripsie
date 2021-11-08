#pragma once

#include "gen_manager.h"

#include "note_names.h"
#include "midi/include/MidiFile.h"
#include "defines.h"
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include "wav.h"
#include <string>

#include <queue>



struct note {
	uint8_t note;
	float start_seconds;
	float duration_seconds;
	float vel;
};

std::vector<note> note_queue;


void load_notes(const std::string& file, uint8_t track, bool debug = false) {
	note_queue.clear();
	smf::MidiFile midi;
	//midi.read("..\\midilib\\fireflies.mid");
	midi.read(file);
	//cout << "NUM TRACKS:" << midi.getNumTracks() << endl;

	midi.linkEventPairs();
	midi.linkNotePairs();
	midi.doTimeAnalysis();
	float T = 1.0f / FS * PLAYBACK_BUFFER_SIZE;
	for (int i = 0; i < midi[track].size(); i++) {
		if (!midi[track][i].isNoteOn()) {
			continue;
		}
		auto note_on = midi[track][i];
		note n = { note_on[1], note_on.seconds, midi[track][i].getDurationInSeconds(), note_on[2] };
			note_queue.push_back(n);
			if(debug) std::cout << note_names[(int)n.note] << " ("  <<  (int)n.note  <<  ")" << 
				" @ t=[" << n.start_seconds << ", " << (n.start_seconds + n.duration_seconds) << "] (dt=" 
				<< n.duration_seconds << ")" << std::endl;		
	}
	//sort according to trigger on
	std::sort(note_queue.begin(), note_queue.end(), [](const note& n1, const note& n2){
		return n1.start_seconds < n2.start_seconds;
		});
}

//simulates the PLAYBACK_BUF_SIZE nature of dealing with a queue of note events
void write_midi_to_wav(gen_manager* gm, gen_config* gc, const std::string& name, bool debug = false) {
	using namespace std;

	float dt = (float)PLAYBACK_BUFFER_SIZE / FS;
	float t = note_queue[0].start_seconds - 0.1f; //start time with 100ms space before starting
	float start_t = t;
	float end_t = note_queue.back().start_seconds + note_queue.back().duration_seconds + 1.0f; //end time with 1s space (for possible decays)
	uint32_t N = (uint32_t)( (end_t - t) * FS );

	float T = 1.0f / FS * PLAYBACK_BUFFER_SIZE; // time taken to play one buffer of samples

	float* bufL = new float[N];
	float* bufR = new float[N];

	uint32_t sample_index = 0;

	std::vector<note> playing = {};

	unsigned int i = 0;
	std::vector<note> trigger_off = {};

	while (t <= end_t) {	
		

		//lambda to see if note must be triggered off
		auto l = [&](const note& n) {
			return n.start_seconds + n.duration_seconds <= t ; 
		};

		//copy notes that must be triggered off
		std::copy_if(playing.begin(), playing.end(), std::back_inserter(trigger_off), l);

		//remove them
		playing.erase(std::remove_if(playing.begin(), playing.end(), l), playing.end());
		 
		std::for_each(trigger_off.begin(), trigger_off.end(), [&](const note& n) {
			//TRIGGER OFF
			if (debug) cout << (n.start_seconds - start_t) << " note off: " << note_names[n.note] << endl;
			gm_trigger_note_off(gm, n.note);
			});		
		trigger_off.clear();

		//find trigger on events
		while (i < note_queue.size()) {
			note n = note_queue[i];
			if (n.start_seconds < t) {
				playing.push_back(n);

				//TRIGGER ON
				if (debug) cout << (n.start_seconds - start_t) << " note on: " << note_names[n.note] << endl;
				gm_trigger_note_on(gm, gc, n.note, n.vel);

				i++;
			}
			else {
				break;
			}
		}

		//write samples
		uint32_t sample_size = std::min<uint32_t>(PLAYBACK_BUFFER_SIZE, N - sample_index);
		gm_write_n_samples(gm, gc, bufL + sample_index, bufR + sample_index, sample_size);

		//update time and index
		t += dt;
		sample_index += sample_size;
	}

	//write to file
	write_to_wav(name, bufL, bufR, N, FS);

}