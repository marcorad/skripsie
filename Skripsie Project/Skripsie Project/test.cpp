#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "LUT.h"
#include "predefined_luts.h"
#include "wavetable.h"
#include "notes.h"
#include "IIR.h"
#include "ADSR.h"
#include "voice.h"


using namespace std;

vector<string> files;

void write_file_names() {
	ofstream file("../../testfiles/files.txt", std::ios::out);
	for (string s : files) {
		file << s << endl;
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


void write_lut(LUT& lut, const string& name) {
	ofstream file;
	file.open("../../testfiles/" + name);
	for (int i = 0; i < lut.buf_size; i++) {
		file << lut.buf[i] << "\n";
	}
	file.close();
	files.push_back(name);
}

void write_samples(Qnum samples[], uint32_t N, const char* name) {
	FILE* file = NULL;
	char fname[128];
	snprintf(fname, sizeof(fname), "../../testfiles/%s", name);
	if (fopen_s(&file, fname, "w") == 0) {
		for (int i = 0; i < N; i++) {
			fprintf(file, "%d\n", samples[i]);
		}
		remove(fname);
		fclose(file);
		files.push_back(name);
	}
}




int main() {	
	init_basic_luts(); //ALWAYS DO THIS
			
	uint32_t N = 3*44100;
	//Qnum* samples = new Qnum[N];

	Qnum* L = new Qnum[N];
	Qnum* R = new Qnum[N];

	//mapped_index wtpos = wt_float_to_mapped_index(1,0.5f);


	//IIR filter;

	//iir_calc_lp_coeff(&filter, 1000.0f, 0.50f);


	//ADSR adsr;

	//LUT exp = create_LUT(0);
	//adsr.exp_decay = &exp;
	//load_exp_decay(adsr.exp_decay);

	/*adsr_init(&adsr, 0.01f, 0.1f, 0.8f, 0.05f);
	adsr_note_on(&adsr);
	adsr_write_n_samples(&adsr, samples, N - 2000);
	adsr_off(&adsr);
	adsr_write_n_samples(&adsr, samples + 8000, 2000);*/
	//write_samples(samples, N, "adsr.txt");
	//write_lut(exp, "exp.txt");


	

	//IIR opt;
	//float f0 = 500.0f;
	//float q = 5.1f;

	//iir_calc_lp_coeff_opt(&opt, iir_freq_float_to_int(f0), iir_Q_float_to_int_recip(q));

	//print_filter_opt(opt);

	//generate_n_samples(samples, &basic, N);
	//filter_buffer_opt(&opt, samples, N);

	voice v;
	voice_init_default(&v);

	voice_configure_detune(&v, 0.75f, 0.01f);
	voice_configure_volume_envelope(&v, 0.1f, 0.3f, 0.8f, 0.1f);
	voice_configure_freq_envelope(&v, 1.0f, 0.5f, 0.5f, 0.1f);
	voice_configure_filter_freq_params(&v, 1.0f, 256.0f);
	voice_configure_wt_position(&v, 2, 0.75f);
	voice_configure_filter_q(&v, 0.7071f);

	voice_note_on_relative_filter_freq(&v, A0, 127);


	generate_n_voice_samples(&v, L, R, N);

	write_samples(L, N, "voiceL.txt");
	write_samples(R, N, "voiceR.txt");

	write_file_names();

	cout << "DONE";
	//delete[] L;
	//delete[] R;
	//delete[] basic_luts;
	
	return 0;
}

