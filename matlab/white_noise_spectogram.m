x = importdata("../testfiles/filter/bp12.txt");
adsr = importdata("../testfiles/filtered adsr.txt");
window = 4096;
overlap = 4000;
fs = 44100;
hold on;
spectrogram(x, hamming(window), overlap, window, fs, 'MinThreshold', -40, 'yaxis', 'power');
colormap summer;
t = (0:(fs-1))/ fs;
plot(t' * 1000,adsr * fs / 1000, 'LineWidth', 1, 'LineStyle', '-', 'Color', 'white');
xlim([50,950]);
ylim([0,20]);




