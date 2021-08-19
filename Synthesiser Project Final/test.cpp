#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "LUT.h"
#include "init_luts.h"
#include "ADSR.h"
#include "IIR.h"
#include "wavetable.h"

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

	ADSR adsr;
	adsr_config(&adsr, 0.05f, 0.05f, 0.5f, 0.05f);
	int size = 10000;
	float* buf = new float[size];
	//adsr_trigger_on(&adsr);
	//adsr_write_n_samples(&adsr, buf, size/2);
	//adsr_trigger_off(&adsr);
	//adsr_write_n_samples(&adsr, buf + size/2, size/2);

	//print_to_file(buf, size, "..\\testfiles\\adsr.txt");

	//cout << cos_lookup(0.1f) << endl;
	//cout << cosf(TWO_PI * 0.1f) << endl;
	//cout << sin_lookup(0.1f) << endl;
	//cout << sinf(TWO_PI * 0.1f) << endl;

	wavetable wt;
	wt_config_hz(&wt, 3.0f, 4000.0f);
	//wt_write_n_samples(buf, &wt, size);

	//print_to_file(buf, size, "..\\testfiles\\4k square.txt");

	IIR iir;
	iir_calc_lp_coeff(&iir, 0.25f, 1.0f/0.7071f);
	print_iir_coeff(iir);

	return 0;
}

