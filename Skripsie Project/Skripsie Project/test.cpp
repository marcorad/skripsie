#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "init_tables.h"
#include "LUT.h"
#include "predefined_luts.h"
#include "wavetable.h"

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


int main() {
	LUT lut_exp = create_LUT(6);
	int N_luts = 8;//2 3 4 5 6 7 8 9
	LUT* lut_saw = create_n_luts(9); 
	LUT* lut_tri = create_n_luts(9);
	LUT* lut_square = create_n_luts(9);
	LUT* lut_sin = create_n_luts(9);

	LUT** luts = new LUT*[4] { lut_sin , lut_tri , lut_saw , lut_square };
	

	load_basic_tables(lut_sin, lut_tri, lut_saw, lut_square, N_luts);
	load_exp_decay(&lut_exp);

	//write_lut(lut_exp, "exp.txt");
	//write_lut(lut_sin[4], "sin.txt"); 
	//write_lut(lut_saw[4], "saw.txt");
	//write_lut(lut_tri[4], "tri.txt");
	//write_lut(lut_square[4], "square.txt");

	
	int N = 10000;
	Qnum* samples = new Qnum[N];
	wavetable wt;
	wt.luts = luts;
	int note = 0;

	note = 69; //concert A
	wt.stride = note_stride[note];
	wt.table_harmonic_index = harmonic_table_indices[note] - 2;
	generate_n_samples(samples, &wt, N, 2);
	write_samples(samples, N, "concert A.txt");


	note = 100; //E7
	wt.stride = note_stride[note];
	wt.table_harmonic_index = harmonic_table_indices[note] - 2;
	generate_n_samples(samples, &wt, N, 2);
	write_samples(samples, N, "E7.txt");

	note = 15; //A0
	wt.stride = note_stride[note];
	wt.table_harmonic_index = harmonic_table_indices[note] - 2;
	generate_n_samples(samples, &wt, N, 2);
	write_samples(samples, N, "A0.txt");


	write_file_names();
	cout << "DONE";
	return 0;
}