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
#include "play_notes.h"
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

void print_iir_coeff(IIR_coeff& f) {
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
	init_notes_digital_freq_buffer();
	
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

	gen_manager gm;
	gen_config gc;

	gm_init(&gm);

	load_notes("..\\midilib\\fireflies.mid", 6, false);	

	//configure global params
	gen_config_volume_envelope(&gc, 0.01f, 0.25f, 1.0f, 0.2f);
	gen_config_wavetables(&gc, 3.0f, 1200.0f, 0.25f);
	gen_config_filter_envelope(&gc, 0.08f, 0.1f, 1.0f, 20.0f, 20.0f, 5.0f, 2.0f);
	//gen_config_filter(&gc, &iir_calc_lp12_coeff);
	gen_config_vibrato(&gc, 10.0f, 7.0f / FS);
	gen_config_tanh_saturator(&gc, 2.0f); //NO CONFIG REQUIRED HERE
	//gen_config_no_saturator(&gc);

	print_iir_coeff(gc.filter_sat_AA);

	//apply global params to generator
	gm_apply_volume_envelope_config(&gm, &gc);
	gm_apply_filter_envelope_config(&gm, &gc);
	gm_apply_wavetable_config(&gm, &gc);
	gm_apply_vibrato_config(&gm, &gc);

	////set generator freq (ALWAYS AFTER CONFIG)
	//gen_freq(&g, &gc, 220.0f / FS, 32);
	//gen_note_on(&g);
	//gen_write_n_samples(&g, &gc, L + 0 * size / 3, R + 0 * size / 3, size / 3);
	//gen_freq(&g, &gc, 440.0f / FS, 64);
	//gen_note_on(&g);
	//gen_write_n_samples(&g, &gc, L + 1 * size / 3, R + 1 * size / 3, size / 3);
	//gen_freq(&g, &gc, 110.0f / FS, 48);
	//gen_note_on(&g);
	//gen_write_n_samples(&g, &gc, L + 2 * size / 3, R + 2 * size / 3, size / 3);

	//print_to_file(L, size, "..\\testfiles\\220 genL.txt");

	//write_to_wav(string("test"), L, R, size, (int)FS);
	
	write_midi_to_wav(&gm, &gc, "fireflies 6 (2)", true);

	return 0;
}

