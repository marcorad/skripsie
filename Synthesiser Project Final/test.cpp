#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "LUT.h"
#include "init_luts.h"
#include "ADSR.h"
#include "IIR.h"
#include "wavetable.h"
#include "generator.h"
#include "midi/include/MidiFile.h"

#include "wav.h"

using namespace std;

void print_to_file(float arr[], int size, const string& name) {
	ofstream f;
	f.open(name);
	for (int i = 0; i < size; i++) {
		f << arr[i] << endl;
	}	
	f.close();
}

void print_iir_coeff(IIR& f) {
	cout << "[" << f.n0 << ", " << f.n1 << ", " << f.n2 << "], [ 1, -" << f.d1 << ", -" << f.d2 << "]" << endl;
}

void print_basic_waveforms(){
	string names[] = { "sine", "triangle", "sawtooth", "square" };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 8; j++) {
			print_to_file(basic_luts[i][j], LUT_SIZE, "..\\basic waveforms\\" + names[i] + "\\" + to_string(j) + ".txt");
		}
	}
}


int main() {	
	init_basic_luts();
	//print_basic_waveforms();

	//ADSR adsr;
	//adsr_config(&adsr, 0.05f, 0.05f, 0.5f, 0.05f);
	int size = FS*3;
	int off = 3000;
	float* L = new float[size];
	float* R = new float[size];
	//adsr_trigger_on(&adsr);
	//adsr_write_n_samples(&adsr, buf, size/2 - off);
	//adsr_trigger_on(&adsr);
	//adsr_write_n_samples(&adsr, buf + size/2 - off, off);
	//adsr_trigger_off(&adsr);
	//adsr_write_n_samples(&adsr, buf + size/2, size/2);

	//print_to_file(buf, size, "..\\testfiles\\adsr.txt");

	//cout << cos_lookup(0.1f) << endl;
	//cout << cosf(TWO_PI * 0.1f) << endl;
	//cout << sin_lookup(0.1f) << endl;
	//cout << sinf(TWO_PI * 0.1f) << endl;

	//wavetable wt;
	//wt_config_hz(&wt, 4000.0f);
	
	//wt_write_n_samples(buf, &wt, size);

	//print_to_file(buf, size, "..\\testfiles\\4k square.txt");

	//IIR iir;
	//iir_calc_hp_coeff(&iir, 0.25f, 10.f);
	//print_iir_coeff(iir);
	//iir_calc_lp_coeff(&iir, 0.25f, 10.f);
	//print_iir_coeff(iir);
	//iir_calc_bp_coeff(&iir, 0.25f, 10.f);
	//print_iir_coeff(iir);

	generator g;
	gen_config gc;

	smf::MidiFile midi;
	midi.read("..\\midilib\\fireflies.mid");
	cout << "NUM TRACKS:" << midi.getNumTracks() << endl;
	midi.linkNotePairs();
	midi.linkEventPairs();
	midi.doTimeAnalysis();
	int track = 6;
	for (int i = 0; i < midi[track].size(); i++) {
		if (!midi[track][i].isNoteOn()) {
			continue;
		}
		double d = midi[track][i].getDurationInSeconds();
		d = midi[track][i].seconds;
		cout << 	d	<< '\t' << (int)midi[0][i][1]			<< endl;
	}


	//configure global params
	gen_config_volume_envelope(&gc, 0.01f, 0.3f, 0.0f, 0.05f);
	gen_config_wavetables(&gc, 3.5f, 10.0f, 0.5f);
	gen_config_filter_envelope(&gc, 0.01f, 0.5f, 0.5f, 0.1f, 0.5f, 10.0f, 0.7071f);

	//apply global params to generator
	gen_apply_volume_envelope_config(&g, &gc);
	gen_apply_filter_envelope_config(&g, &gc);
	gen_apply_wavetable_config(&g, &gc);

	//set generator freq (ALWAYS AFTER CONFIG)
	gen_freq(&g, &gc, 220.0f / FS, 32);
	gen_note_on(&g);
	gen_write_n_samples(&g, &gc, L + 0 * size / 3, R + 0 * size / 3, size / 3);
	gen_freq(&g, &gc, 440.0f / FS, 64);
	gen_note_on(&g);
	gen_write_n_samples(&g, &gc, L + 1 * size / 3, R + 1 * size / 3, size / 3);
	gen_freq(&g, &gc, 110.0f / FS, 48);
	gen_note_on(&g);
	gen_write_n_samples(&g, &gc, L + 2 * size / 3, R + 2 * size / 3, size / 3);

	print_to_file(L, size, "..\\testfiles\\220 genL.txt");

	write_to_wav(string("test"), L, R, size, (int)FS);

	return 0;
}

