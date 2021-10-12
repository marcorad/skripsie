#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <random>

#include "LUT.h"
#include "init_luts.h"
#include "ADSR.h"
#include "IIR.h"
#include "wavetable.h"
#include "generator.h"
#include "play_notes.h"
#include "wav.h"


using namespace std;

//TODO: write code that tests all aspects for report

//white noise generator
void generate_wn(float* buf, int N) {
	unsigned seed = 42; //LOL
	std::default_random_engine generator(seed);
	auto dist = std::normal_distribution<float>(0.0f, 1.0f);

	for (int i = 0; i < N; i++)
	{
		buf[i] = dist(generator);
	}
}

void add_wn(float* wn, int N, float u, float s) {
	srand(time(NULL));
	unsigned seed = rand();
	std::default_random_engine generator(seed);
	auto dist = std::normal_distribution<float>(u, s);

	for (int i = 0; i < N; i++)
	{
		wn[i] += dist(generator);
	}
}

void test_adsr() {
	//adsr with retrigger
}

void test_wavetable(float* buf, int N, float pos) {
	//wavetables at different pos and freq
	float f0 = notes_digital_freq[A0];
	int off = 0;
	int n = N / 8;
	gen_config gc;
	gc.wt_pos = pos;

	wavetable wt;

	for (float f = f0; f <= notes_digital_freq[A8]; f *= 2.0f) //sweep octaves
	{
		wt.phase = 0;
		wt_config_digital_freq(&wt, f);
		for (int i = off;  i < off + n; i++)
		{
			buf[i] = wt_sample(&wt, &gc);
		}
		off += n;
	}

}

void test_filter(float* wn, float* out, float* adsr_out, int N, float a, float d, float s, float r, float freqstart, float freqend, float q, void (*filter_coeff_func)(IIR_coeff*, float, float)) {
	ADSR adsr;
	IIR_prev_values pv = {0,0,0,0};
	IIR_coeff coeff;
	float params[4];
	adsr_config(params, a, d, s, r);
	adsr_trigger_on(&adsr);
	
	for (int i = 0; i < N/2; i++)
	{
		float f0 = freqstart + adsr_sample(&adsr, params) * (freqend - freqstart);
		(*filter_coeff_func)(&coeff, f0, q);
		out[i] = iir_filter_sample(&coeff, &pv, wn[i]);
		adsr_out[i] = f0;
	}

	adsr_trigger_off(&adsr);

	for (int i = N/2; i < N; i++)
	{
		float f0 = freqstart + adsr_sample(&adsr, params) * (freqend - freqstart);
		(*filter_coeff_func)(&coeff, f0, q);
		out[i] = iir_filter_sample(&coeff, &pv, wn[i]);
		adsr_out[i] = f0;
	}
}

void test_waveshape() {
	//waveshaped at dif gains and freq
}

void test_gm() {
	//different tones and fg overload after 8 tones
}

void test_freq_scaling() {
	cout << "Detune factor 125.57: " << get_detune_factor_cents_lut(125.57f) << endl;
}

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


void run_tests() {
	//FILTERS
	int N = 44100;
	float* wn = new float[N];
	generate_wn(wn, N);

	float* out = new float[N];
	float* adsr = new float[N];
	print_to_file(wn, N, "..//testfiles//filter//white.txt");
	test_filter(wn, out, adsr, N, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_hp24_coeff);
	print_to_file(out, N, "..//testfiles//filter//hp24.txt");

	test_filter(wn, out, adsr, N, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_lp24_coeff);
	print_to_file(out, N, "..//testfiles//filter//lp24.txt");

	test_filter(wn, out, adsr, N, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_bp12_coeff);
	print_to_file(out, N, "..//testfiles//filter//bp12.txt");

	test_filter(wn, out, adsr, 44100, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_hp12_coeff);
	print_to_file(out, N, "..//testfiles//filter//hp12.txt");

	test_filter(wn, out, adsr, 44100, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_lp12_coeff);
	print_to_file(out, N, "..//testfiles//filter//lp12.txt");

	print_to_file(adsr, N, "..//testfiles//filter//adsr.txt");
	int i = 0;

	//WAVETABLE
	for (float p = 0.0; p < 4.0f; p += 0.5f)
	{
		test_wavetable(out, N, p);
		print_to_file(out, N, "..//testfiles//wavetable//" + to_string(i) + ".txt");
		i++;
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

	load_notes("..\\midifiles\\fireflies.mid", 6, false);	

	//configure global params
	gen_config_volume_envelope(&gc, 0.03f, 0.25f, 0.8f, 1.0f);
	gen_config_wavetables(&gc, 2.0f, 10.0f, 0.8f, 0.3f);
	gen_config_filter_envelope(&gc, 0.08f, 0.1f, 1.0f, 1000.0f, 5.0f, 1.0f, 2.0f);
	gen_config_filter(&gc, &iir_calc_bp12_coeff);
	gen_config_vibrato(&gc, 10.0f, 7.0f / FS);
	//gen_config_tanh_saturator(&gc, 1.0f); //NO CONFIG REQUIRED HERE
	gen_config_no_saturator(&gc);

	print_iir_coeff(gc.filter_sat_AA);

	//apply global params to generator
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
	
	//write_midi_to_wav(&gm, &gc, "ff1", true);
	//test_freq_scaling();

	run_tests();

	return 0;
}

