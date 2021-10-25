x = importdata("../testfiles/waveshape/sin 4.txt");
plot(x);
window = 512;
overlap = 510;
fs = 44100;
hold on;

[s,f,t] = spectrogram(x, blackman(window), overlap, window, fs);
m = max(max(abs(s)));
y = 20*log10(abs(s)/m);
surf(t,f/1000,y,'EdgeColor','none');
cb = colorbar;
cb.Label.String = 'Power (dB)';
t = (0:(fs-1))/ fs;
z = ones(size(t)) * max(y(:));
xlim([50,950]/1000);
ylim([0,20]);
caxis([-80,10]);
xlabel("Time [s]");
ylabel("Frequency [kHz]");


title("Hyperbolic tangent waveshaping with AA filter (g=32)");
%title("Hyperbolic tangent waveshaping no AA filter (g=32)");
set(gcf, 'Position',[500,500, 450, 300]);

%plot3(t,10 * ones(size(t)), z,  'LineWidth', 1, 'LineStyle', '--', 'Color', 'black');
%legend("Vibrato", "Center frequency (f_0)");

% zlabel("Power (dB)");
% legend("Filtered white noise", "ADSR-modulated f_c");
% view(-30,60);

