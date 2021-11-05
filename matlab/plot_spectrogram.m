x = importdata("../testfiles/manager/overall R.txt");
%plot(x);
window = 1024;
overlap = 1020;
fs = 44100;
hold on;

[s,f,t] = spectrogram(x, blackman(window), overlap, window, fs);
m = max(max(abs(s)));
y = 20*log10(abs(s));
%y(y < -10) = -80;

z = ones(size(t)) * max(max(y));
%plot3(t,0.5 * ones(size(t)), z,  'LineWidth', 1, 'LineStyle', '--', 'Color', 'black');
%plot3(t,10 * ones(size(t)), z,  'LineWidth', 1, 'LineStyle', '--', 'Color', 'black');


surf(t,f/1000,y,'EdgeColor','none');
cb = colorbar;
cb.Label.String = 'Power (dB)';
t = (0:(fs-1))/ fs;

xlim([50,950]/1000);
ylim([0,22.05]);
caxis([-50,30]);
xlabel("Time [s]");
ylabel("Frequency [kHz]");

%legend("Center frequency (f_c)", "Vibrato");
title("Full system test (right channel)");
%title("Hyperbolic tangent waveshaping no AA filter (g=8)");
set(gcf, 'Position',[500,500, 450, 300]);



% zlabel("Power (dB)");
% legend("Filtered white noise", "ADSR-modulated f_c");
% view(-30,60);

