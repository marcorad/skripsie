#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "LUT.h"
#include "predefined_luts.h"
#include "wavetable.h"
#include "notes.h"
#include "IIR.h"
#include "ADSR.h"

using namespace std;

vector<string> files;

void write_file_names() {
	ofstream file;
	file.open("../../testfiles/files.txt");
	for (string s : files) {
		file << s << "\n";
	}
	file.close();
}

void print_filter_slow(IIR& filter) {
	cout << "[ " << filter.n0 << ", 2*" << filter.n1 << ", " << filter.n2 << " ] , ";
	cout << "[ " << (1<<Q) << ", -2*" << filter.d1 << ", -" << filter.d2 << " ]\n";
}

void print_filter_opt(IIR& filter) {
	cout << "[ " << filter.n0 << ", " << filter.n1 << ", " << filter.n2 << " ] , ";
	cout << "[ " << (1 << Q_IIR) << ", -" << filter.d1 << ", -" << filter.d2 << " ]\n";
}


void write_lut(LUT& lut, string name) {
	ofstream file;
	file.open("../../testfiles/" + name);
	for (int i = 0; i < lut.buf_size; i++) {
		file << lut.buf[i] << "\n";
	}
	file.close();
	files.push_back(name);
}

void write_samples(Qnum* samples, int N, string name) {
	ofstream file;
	file.open("../../testfiles/" + name);
	for (int i = 0; i < N; i++) {
		file << samples[i] << "\n";
	}
	file.close();
	files.push_back(name);
}




int main() {	

	wavetable basic = load_basic_wavetable();
		
	int N = 10000;
	Qnum* samples = new Qnum[N];

	mapped_index wtpos = wt_float_to_mapped_index(3,0.0f);
	wt_note(&basic, A4, wtpos);
	wt_set_dutycyle(&basic, wt_duty_cycle_from_float(0.5f));


	//IIR filter;

	//iir_calc_lp_coeff(&filter, 1000.0f, 0.50f);


	ADSR adsr;

	LUT exp = create_LUT(0);
	adsr.exp_decay = &exp;
	load_exp_decay(adsr.exp_decay);

	adsr_init(&adsr, 0.001f, 0.1f, 0.8f, 0.05f);
	adsr_note_on(&adsr, 0xFF);
	adsr_write_n_samples(&adsr, samples, N - 2000);
	adsr_off(&adsr);
	adsr_write_n_samples(&adsr, samples + 8000, 2000);
	write_samples(samples, N, "adsr.txt");
	write_lut(exp, "exp.txt");


	

	IIR slow;
	IIR opt;
	float f0 = 5000.0f;
	float q = 2.0f;

	iir_calc_lp_coeff(&slow, f0, q);
	iir_calc_lp_coeff_opt(&opt, iir_freq_float_to_int(f0), iir_Q_float_to_int_recip(q));

	cout << "SLOW\n";
	print_filter_slow(slow);
	cout << "OPTIMISED\n";
	print_filter_opt(opt);

	generate_n_samples(samples, &basic, N);
	filter_buffer(&slow, samples, N);
	write_samples(samples, N, "slow filter.txt");

	generate_n_samples(samples, &basic, N);
	filter_buffer_opt(&opt, samples, N);
	write_samples(samples, N, "fast filter.txt");

	write_file_names();

	cout << "DONE";
	return 0;
}

