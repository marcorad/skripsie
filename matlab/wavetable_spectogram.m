x = zeros(44100, 8);
window = 5512;
overlap = 0;
fs = 44100;
f0 = 27.5;
period = floor(44100/f0);

start = 1;

for f = 1:8

    for i = 0:7
        x(1:44100, i+1) = importdata("../testfiles/wavetable/" + i + ".txt");
    end

    figure();
    set(gcf,'position',[10,10,500,500])

    for i = 1:8
        subplot(4, 2, i);      
        plot(0:(period), x(start:(start+period), i));    
        title("Position = " + ((i-1)/2));
        xlim([0, period]);
    end
    suptitle("Wavetable position sweep (f_0 = " + f0 + " Hz)");

    f0 = f0 * 2;
    period = floor(44100/f0);
    start = start + floor(44100/8);
end
% hold on;
% spectrogram(x(:, 1), hamming(window), overlap, window, fs, 'MinThreshold', -80, 'yaxis', 'power');
% colormap summer;
% xlim([50,950]);
% ylim([0,4]);




