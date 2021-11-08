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

//chirp generator (in digital frequency)
void generate_linear_chirp_frequencies(float* buf, int N, float start, float end) {
	for (int i = 0; i < N; i++)
	{
		float x = (float)i / (float)N;
		float f = start + (end-start) * x;
		buf[i] = f;
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

void test_adsr(float* buf, int N) {
	ADSR adsr;
	int off = 0;
	int k = N / 8;
	float t = k / FS;
	float params[4];
	adsr_config(params, t, t, 0.5f, t);

	for (int i = 0; i < k / 3; i++)
	{
		buf[i] = 0.0f;
	}
	off += k / 3;
	adsr_trigger_on(&adsr);

	for (int i = off; i < off + k + k/2; i++)
	{
		buf[i] = adsr_sample(&adsr, params);
	}
	adsr_trigger_on(&adsr);
	off += k + k / 2;
	for (int i = off; i < off + 3 * k; i++)
	{
		buf[i] = adsr_sample(&adsr, params);
	}
	off += 3* k;
	adsr_trigger_off(&adsr);
	for (int i = off; i < off + N - off; i++)
	{
		buf[i] = adsr_sample(&adsr, params);
	}
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

void test_vibrato(float* buf, int N) {
	float f0 = 10000.0f/FS;
	float cents = 500.0f;
	generator g;
	gen_config gc;
	gen_config_default(&gc);

	gen_config_vibrato(&gc, cents, 2.0f / FS);

	gen_apply_vibrato_config(&g, &gc);
	gen_freq(&g, &gc, f0, 127);
	gen_note_on(&g);



	for (int i = 0; i < N; i++)
	{
		float temp;
		gen_sample(&g, &gc, buf + i, &temp);
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

void test_waveshape(float* buf_tanh, float* buf_sin, int N, float* freq, float g_tanh, float g_sin, bool use_aa = true) {
	generator g;
	gen_config gc;
	gen_config_default(&gc);
	gen_apply_vibrato_config(&g, &gc);	
	gen_note_on(&g);
	gen_config_tanh_saturator(&gc, g_tanh);
	gen_config_filter_envelope(&gc, 0.1f, 0.1f, 1.0f, 0.1f, 512.0f, 512.0f, 0.7071f);
	gen_config_wavetables(&gc, 0.0f, 0.0f, 0.0f, 0.0f);
	if(!use_aa)
		iir_calc_lp12_coeff(&gc.filter_sat_AA, DIGITAL_FREQ_20KHZ, 1.0f);
	for (int i = 0; i < N; i++)
	{
		float temp;
		gen_freq(&g, &gc, freq[i], 127);		
		gen_sample(&g, &gc, buf_tanh + i, &temp);
	}

	//TODO IMPLEMENT SIN 
	gen_config_sine_saturator(&gc, g_sin);
	gen_reset_AA_pv(&g); //reset pv from previous step
	if (!use_aa)
		iir_calc_lp12_coeff(&gc.filter_sat_AA, DIGITAL_FREQ_20KHZ, 1.0f);
	for (int i = 0; i < N; i++)
	{
		//WHERE IS THE NOISE COMING FROM?!?!?!?!?!
		float temp;
		gen_freq(&g, &gc, freq[i], 127);
		gen_sample(&g, &gc, buf_sin + i, &temp);
	}
}

void test_wavetable_sweep(float* buf, int N, float* freq, float pos) {
	wavetable wt;
	gen_config gc;

	gc.wt_pos = pos;

	for (int i = 0; i < N; i++)
	{
		wt_config_digital_freq(&wt, freq[i]);
		buf[i] = wt_sample(&wt, &gc);
	}
}

void test_stereo_width(float* bufL, float* bufR, int N, float width) {
	float f0 = 100.0f / FS;
	float cents = 1200.0f;
	generator g;
	gen_config gc;
	gen_config_default(&gc);
	gen_apply_vibrato_config(&g, &gc);
	gen_config_wavetables(&gc, 0.0f, cents, 1.0f, width);
	gen_freq(&g, &gc, f0, 127);
	gen_note_on(&g);
	gen_write_n_samples(&g, &gc, bufL, bufR, N);
}

void test_gm_overload(float buf[], int N) {

	gen_manager gm;
	gen_config gc;
	gen_config_default(&gc);
	gm_init(&gm);
	gc.filter_coeff_func = &iir_calc_no_coeff;
	gen_config_wavetables(&gc, 0.0f, 0.0f, 0.0f, 0.0f);
	
	float* temp = new float[N];

	float freq[16];
	for (int i = 0; i < 8; i++)
	{
		freq[i] = (1000.0f + i * 2000.0f) / FS; //1k, 3k, ..., 15k
		freq[i+8] = (2000.0f + i * 2000.0f) / FS; //2k, 4k, ..., 16k
	}

	int L = N / 16;
	int end = L * 16;
	int note = 0;
	for (int off = 0; off < end; off += L)
	{
		gm_trigger_note_on_freq(&gm, &gc, note, 127, freq[note]);		
		gm_write_n_samples(&gm, &gc, buf + off, temp + off, L);
		note++;
	}

	//fill remaining with 0
	for (int i = 0; i < N-end; i++)
	{
		buf[end + i] = 0.0f;
	}	
}



void test_gm_operation(float buf[], int N) {

	gen_manager gm;
	gen_config gc;
	gen_config_default(&gc);
	gm_init(&gm);
	gc.filter_coeff_func = &iir_calc_no_coeff;
	gen_config_wavetables(&gc, 0.0f, 0.0f, 0.0f, 0.0f);

	float* temp = new float[N];


	float freq[8];
	for (int i = 0; i < 8; i++)
	{
		freq[i] = (500.0f + i * 2500.0f) / FS; //0k5, 3k, 5k5, ..., 18k
	}

	int L = N / 16;
	int end = L * 16;
	int note = 0;
	int count = 0;
	//trigger all on, then trigger all off by LIFO
	for (int off = 0; off < end; off += L)
	{
		if (count < 8)
		{
			gm_trigger_note_on_freq(&gm, &gc, note, 127, freq[note]);
			note++;
		}
		else {
			gm_trigger_note_off(&gm, note);
			note--;
		}

		gm_write_n_samples(&gm, &gc, buf + off, temp + off, L);
		count++;		
	}

	//fill remaining with 0
	for (int i = 0; i < N - end; i++)
	{
		buf[end + i] = 0.0f;
	}
}

void test_overall_system(float bufL[], float bufR[], int N) {
	gen_manager gm;
	gen_config gc;
	gen_config_default(&gc);
	gm_init(&gm);
	gen_config_wavetables(&gc, 2.5f, 100.0f, 0.25f, 1.0f);
	gen_config_filter(&gc, &iir_calc_lp24_coeff);
	gen_config_filter_envelope(&gc, 0.3f, 0.1f, 0.5f, 0.2f, 1.0f, 50.0f, 7.0f);
	gen_config_tanh_saturator(&gc, 2.0f);
	gen_config_vibrato(&gc, 50.0f, 5.0f / FS);
	gm_apply_vibrato_config(&gm, &gc);
	gen_config_volume_envelope(&gc, 0.01f, 0.2f, 0.5f, 0.2f);

	float f1 = 500.0f / FS, f2 = 10000.0f / FS ;

	int n = N / 16;
	int n1 = 4 * n;
	int n2 = 8 * n;
	int n3 = 12 * n;
	int n4 = 16 * n;

	gm_trigger_note_on_freq(&gm, &gc, 0, 127, f1);
	int begin = 0;
	int size = n1;
	gm_write_n_samples(&gm, &gc, bufL + begin, bufR + begin, size);

	begin = n1;
	size = n1;
	gm_trigger_note_on_freq(&gm, &gc, 1, 127, f2);
	gm_write_n_samples(&gm, &gc, bufL + begin, bufR + begin, size);

	begin = n2;
	size = n3 - n2;
	gm_trigger_note_off(&gm, 0);
	gm_write_n_samples(&gm, &gc, bufL + begin, bufR + begin, size);

	begin = n3;
	size = n4 - n3;
	gm_trigger_note_off(&gm, 1);
	gm_write_n_samples(&gm, &gc, bufL + begin, bufR + begin, size);

	for (int i = n4; i < N; i++)
	{
		bufL[i] = 0.0f;
		bufR[i] = 0.0f;
	}	
}

void test_freq_scaling() {
	cout << "Detune factor 125.57: " << get_detune_factor_cents_lut(125.57f) << endl;
}

void print_to_file(float arr[], int size, const string& name) {
	ofstream f;
	f.open(name);
	f.precision(10);
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
	
	int N = 44100;
	float* wn = new float[N];
	generate_wn(wn, N);

	float* chirp_freq = new float[N];
	generate_linear_chirp_frequencies(chirp_freq, N, DIGITAL_FREQ_20HZ, DIGITAL_FREQ_20KHZ*0.5f); //20 to 10k

	float* out = new float[N];
	float* out2 = new float[N];
	float* adsr = new float[N];

	//FILTERS
	cout << "_____________________" << endl;
	cout << "STARTING FILTER TESTS" << endl;
	cout << "_____________________" << endl;
	
	cout << "HP24" << endl;
	print_to_file(wn, N, "..//testfiles//filter//white.txt");
	test_filter(wn, out, adsr, N, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_hp24_coeff);
	print_to_file(out, N, "..//testfiles//filter//hp24.txt");
	cout << "LP24" << endl;
	test_filter(wn, out, adsr, N, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_lp24_coeff);
	print_to_file(out, N, "..//testfiles//filter//lp24.txt");
	cout << "BP12" << endl;
	test_filter(wn, out, adsr, N, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_bp12_coeff);
	print_to_file(out, N, "..//testfiles//filter//bp12.txt");
	cout << "HP12" << endl;
	test_filter(wn, out, adsr, 44100, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_hp12_coeff);
	print_to_file(out, N, "..//testfiles//filter//hp12.txt");
	cout << "LP12" << endl;
	test_filter(wn, out, adsr, 44100, 0.25f, 0.1f, 0.5f, 0.2f, 1000.0f / FS, 20000.0f / FS, 5.0f, &iir_calc_lp12_coeff);
	print_to_file(out, N, "..//testfiles//filter//lp12.txt");
	cout << "WRITING ENVELOPE" << endl;
	print_to_file(adsr, N, "..//testfiles//filter//adsr.txt");
	int i = 0;

	cout << endl;
	cout << "_____________________" << endl;
	cout << "STARTING WAVETABLE TESTS" << endl;
	cout << "_____________________" << endl;
	//WAVETABLE
	for (float p = 0.0; p < 4.0f; p += 0.5f)
	{
		cout << "POS = " << p << endl;
		test_wavetable(out, N, p);
		print_to_file(out, N, "..//testfiles//wavetable//" + to_string(i) + ".txt");
		i++;
	}

	cout << "SQUARE CHIRP" << endl;
	test_wavetable_sweep(out, N, chirp_freq, 3.0f);
	print_to_file(out, N, "..//testfiles//wavetable//square chirp.txt");

	cout <<  endl;
	cout << "_____________________" << endl;
	cout << "STARTING GENERATOR TESTS" << endl;
	cout << "_____________________" << endl;
	//GENERATOR
	cout << "VIBRATO" << endl;
	test_vibrato(out, N);
	print_to_file(out, N, "..//testfiles//generator//vibrato.txt");

	float w[] = { 0.0f, 0.33f, 0.67f, 1.0f };

	for (int i = 0; i < 4; i++)
	{
		cout << "WIDTH = " << w[i] << endl;
		test_stereo_width(out, out2, N, w[i]);
		print_to_file(out, N, "..//testfiles//generator//width L " + to_string(i) + ".txt");
		print_to_file(out2, N, "..//testfiles//generator//width R " + to_string(i) + ".txt");
	}

	cout << endl;
	cout << "_____________________" << endl;
	cout << "STARTING WAVESHAPER TESTS" << endl;
	cout << "_____________________" << endl;
	

	//WAVESHAPE
	for (int i = 0; i < 5; i++)
	{		
		float ip1 = (float)i + 1.0f;
		float g_sin = PI / 2.0f * ((float)i + 1.0f);
		float g_tanh = ip1 * ip1 * 2.0f;

		cout << "G_TANH = " << g_tanh << " G_SIN = " << g_sin << endl;

		test_waveshape(out, out2, N, chirp_freq, g_tanh, g_sin, true);
		print_to_file(out, N, "..//testfiles//waveshape//tanh " + to_string(i) + ".txt");
		print_to_file(out2, N, "..//testfiles//waveshape//sin " + to_string(i) + ".txt");

		test_waveshape(out, out2, N, chirp_freq, g_tanh, g_sin, false);
		print_to_file(out, N, "..//testfiles//waveshape//tanh " + to_string(i) + " no aa.txt");
		print_to_file(out2, N, "..//testfiles//waveshape//sin " + to_string(i) + " no aa.txt");
	}

	cout << endl;
	cout << "_____________________" << endl;
	cout << "STARTING ADSR TESTS" << endl;
	cout << "_____________________" << endl;
	cout << "ADSR WITH RETRIGGER" << endl;
	test_adsr(out, N);
	print_to_file(out, N, "..//testfiles//adsr//adsr retrigger.txt");

	cout << endl;
	cout << "_____________________" << endl;
	cout << "STARTING MANAGER TESTS" << endl;
	cout << "_____________________" << endl;
	cout << "TESTING MANAGER OPERATION" << endl;
	test_gm_operation(out, N);
	print_to_file(out, N, "..//testfiles//manager//operation.txt");	

	cout << "TESTING MANAGER OVERLOAD" << endl;
	test_gm_overload(out, N);
	print_to_file(out, N, "..//testfiles//manager//overload.txt");

	cout << "TESTING OVERALL SYSTEM" << endl;
	test_overall_system(out, out2, N);
	print_to_file(out, N, "..//testfiles//manager//overall L.txt");
	print_to_file(out2, N, "..//testfiles//manager//overall R.txt");
	
}


int main() {	

	cout << "INITIALISING" << endl;
	init_basic_luts();
	init_notes_digital_freq_buffer();

	int size = FS*3;
	int off = 3000;
	float* L = new float[size];
	float* R = new float[size];

	gen_manager gm;
	gen_config gc;

	gm_init(&gm);

	cout << "LOADING MOONLIGHT SONATA" << endl;
	load_notes("..\\midifiles\\moonlight sonata.mid", 0, true);	

	cout << endl << "_______________" << endl;
	cout << "CONFIGURATION 1" << endl;
	gen_config_volume_envelope(&gc, 0.05f, 0.05f, 0.8f, 0.06f);
	gen_config_wavetables(&gc, 2.0f, 10.0f, 0.8f, 0.3f);
	gen_config_filter_envelope(&gc, 0.1f, 0.1f, 1.0f, 1000.0f, 3.0f, 500.0f, 0.7f);
	gen_config_filter(&gc, &iir_calc_lp12_coeff);
	gen_config_vibrato(&gc, 15.0f, 7.0f / FS);
	gen_config_tanh_saturator(&gc, 1.0f); 
	gm_apply_vibrato_config(&gm, &gc);		
	cout << "WRITING TO WAV" << endl;
	//write_midi_to_wav(&gm, &gc, "moonlight 1", false);
	cout << "_______________" << endl;

	cout << "CONFIGURATION 2" << endl;
	gen_config_volume_envelope(&gc, 0.05f, 0.2f, 0.5f, 0.01f);
	gen_config_wavetables(&gc, 2.0f, 0.0f, 0.8f, 0.8f);
	gen_config_filter_envelope(&gc, 0.15f, 0.1f, 1.0f, 1000.0f, 100.0f, 3.0f, 2.0f);
	gen_config_filter(&gc, &iir_calc_lp24_coeff);
	gen_config_vibrato(&gc, 0.0f, 7.0f / FS);
	gen_config_sine_saturator(&gc, 2 * PI);
	gm_apply_vibrato_config(&gm, &gc);
	cout << "WRITING TO WAV" << endl;
	//write_midi_to_wav(&gm, &gc, "moonlight 2", false);
	cout << "_______________" << endl;

	cout << "CONFIGURATION 3" << endl;
	gen_config_volume_envelope(&gc, 0.08f, 0.2f, 0.75f, 0.08f);
	gen_config_wavetables(&gc, 2.5f, 1200.0f, 0.8f, 0.2f);
	gen_config_filter_envelope(&gc, 0.15f, 0.2f, 1.0f, 1000.0f, 100.0f, 10.0f, 0.7071f);
	gen_config_filter(&gc, &iir_calc_lp24_coeff);
	gen_config_vibrato(&gc, 10.0f, 7.0f / FS);
	gen_config_tanh_saturator(&gc, 5.0f);
	gm_apply_vibrato_config(&gm, &gc);
	cout << "WRITING TO WAV" << endl;
	//write_midi_to_wav(&gm, &gc, "moonlight 3", false);
	cout << "_______________" << endl;

	cout << "CONFIGURATION 4" << endl;
	gen_config_volume_envelope(&gc, 0.08f, 0.2f, 0.75f, 0.08f);
	gen_config_wavetables(&gc, 3.2f, 20.0f, 0.8f, 0.5f);
	gen_config_filter_envelope(&gc, 0.2f, 0.2f, 1.0f, 1000.0f, 1.0f, 3.0f, 3.0f);
	gen_config_filter(&gc, &iir_calc_hp24_coeff);
	gen_config_vibrato(&gc, 0.0f, 7.0f / FS);
	gen_config_no_saturator(&gc);
	gm_apply_vibrato_config(&gm, &gc);
	cout << "WRITING TO WAV" << endl;
	//write_midi_to_wav(&gm, &gc, "moonlight 4", false);
	cout << "_______________" << endl;

	cout << "CONFIGURATION 5" << endl;
	gen_config_volume_envelope(&gc, 0.005f, 0.1f, 0.0f, 0.08f);
	gen_config_wavetables(&gc, 2.9f, 0.0f, 0.0f, 0.25f);
	gen_config_filter_envelope(&gc, 0.1f, 0.1f, 1.0f, 1000.0f, 700.0f, 7.0f, 2.0f);
	gen_config_filter(&gc, &iir_calc_lp24_coeff);
	gen_config_vibrato(&gc, 0.0f, 7.0f / FS);
	gen_config_no_saturator(&gc);
	gm_apply_vibrato_config(&gm, &gc);
	cout << "WRITING TO WAV" << endl;
	write_midi_to_wav(&gm, &gc, "moonlight 5", false);
	cout << "_______________" << endl;
	

	//run_tests();
	return 0;
}

