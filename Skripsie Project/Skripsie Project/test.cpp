#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "LUT.h"
#include "predefined_luts.h"
#include "wavetable.h"
#include "notes.h"

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

//n'th LUT and a position from [0,1)
mapped_index get_wt_pos(mapped_index n, float pos) {
	mapped_index offset = (mapped_index)((float)(1 << (B_lut - 2)) * pos);
	return offset + (n << (B_lut - 2));
}


int main() {
	//LUT lut_exp = create_LUT(6);
	//int N_luts = 8;//2 3 4 5 6 7 8 9
	//LUT* lut_saw = create_luts(); 
	//LUT* lut_tri = create_luts();
	//LUT* lut_square = create_luts();
	//LUT* lut_sin = create_luts();

	//LUT** luts = new LUT*[4] { lut_sin , lut_tri , lut_saw , lut_square };
	//

	//load_basic_luts(lut_sin, lut_tri, lut_saw, lut_square, N_luts);
	//load_exp_decay(&lut_exp);

	//write_lut(lut_exp, "exp.txt");
	//write_lut(lut_sin[7], "sin.txt"); 
	//write_lut(lut_tri[2], "tri.txt");
	//write_lut(lut_square[7], "square.txt");

	//
	
	//wavetable wt;
	//wt.luts = luts;
	//int note = 0;
	//int wave = 0;

	//note = 69; //concert A
	//wt.stride = note_stride[note];
	//wt.table_harmonic_index = harmonic_table_indices[note];
	//generate_n_samples(samples, &wt, N, wave);
	//write_samples(samples, N, "concert A.txt");


	//note = 100; //E7
	//wt.stride = note_stride[note];
	//wt.table_harmonic_index = harmonic_table_indices[note];
	//generate_n_samples(samples, &wt, N, wave);
	//write_samples(samples, N, "E7.txt");

	//note = 15; //A0
	//wt.stride = note_stride[note];
	//wt.table_harmonic_index = harmonic_table_indices[note];
	//generate_n_samples(samples, &wt, N, wave);
	//write_samples(samples, N, "A0.txt");

	wavetable basic = load_basic_wavetable();


	//write_lut(basic.luts[SAWTOOTH][7], "saw.txt");
	//write_lut(basic.luts[TRIANGLE][7], "tri.txt");

	
	int N = 10000;
	Qnum* samples = new Qnum[N];

	mapped_index wtpos = get_wt_pos(3,0.0f);
	wt_note(&basic, E3, wtpos);
	wt_set_dutycyle(&basic, HALF_MAPPED_INDEX_MAX  + (HALF_MAPPED_INDEX_MAX >> 1));
	generate_n_samples(samples, &basic, N);
	write_samples(samples, N, "concert A.txt");


	write_file_names();
	cout << "DONE";
	return 0;
}

