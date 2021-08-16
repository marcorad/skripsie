#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "LUT.h"
#include "init_luts.h"

using namespace std;

void print_to_file(float arr[], int size, const string& name) {
	ofstream f;
	f.open(name);
	for (int i = 0; i < size; i++) {
		f << arr[i] << endl;
	}	
	f.close();
}

void print_basic_waveforms(){
	string names[] = { "sine", "triangle", "sawtooth", "square" };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 8; j++) {
			print_to_file(basic_luts[i][j], LUT_SIZE, "basic waveforms\\" + names[i] + "\\" + to_string(j) + ".txt");
		}
	}
}


int main() {	
	init_basic_luts();
	print_basic_waveforms();
	return 0;
}

