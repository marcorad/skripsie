x = importdata("../testfiles/filter/lp12.txt");
adsr = importdata("../testfiles/filtered adsr.txt");
window = 1024;
overlap = 1020;
fs = 44100;
hold on;

[s,f,t] = spectrogram(x, blackman(window), overlap, window, fs);
y = 20*log10(abs(s));
y = imgaussfilt(y, 15);
surf(t,f/1000,y,'EdgeColor','none');
cb = colorbar;
cb.Label.String = 'Power (db)';
t = (0:(fs-1))/ fs;
z = ones(size(t)) * max(y(:));
xlim([50,950]/1000);
ylim([0,20]);
%zlim([-60,60])
xlabel("Time [s]");
ylabel("Frequency [kHz]");
zlabel("Power (dB)");
plot3(t,adsr * fs / 1000, z,  'LineWidth', 2, 'LineStyle', '--', 'Color', 'black');
legend("Filtered white noise", "ADSR-modulated f_c");
view(-30,60);
title("Modulated LP12-filtered white noise");


