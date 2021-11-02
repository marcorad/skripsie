x = zeros(44100, 8);
window = 5512;
overlap = 0;
fs = 44100;
f0 = 27.5;
period = floor(44100/f0);

start = 1;

for f = 1:1

    for i = 0:7
        x(1:44100, i+1) = importdata("../testfiles/wavetable/" + i + ".txt");
    end

    figure();
    hold on;
    set(gcf,'position',[10,10,500,500])

    for i = 1:8
        %subplot(4, 2, i);      
        pos = (i-1)/2;
        plot3(0:(period), pos * ones(period+1), x(start:(start+period), i), 'LineWidth', 1.4); 
        
       
    end
    %patch([0, period, period, 0], [0, 0, 3.5, 3.5], [0, 0, 0, 0], [0, 0, 0]);
        alpha(0.05);
    %suptitle("Wavetable position sweep (f_0 = " + f0 + " Hz)");
    title("Inter-wavetable interpolation (f_0 = " + f0 + " Hz)");
    xlim([0, period]);
    xlabel("n");
    ylabel("wavetable position");
    view(45,30);
    f0 = f0 * 2;
    period = floor(44100/f0);
    start = start + floor(44100/8);
end
% hold on;
% spectrogram(x(:, 1), hamming(window), overlap, window, fs, 'MinThreshold', -80, 'yaxis', 'power');
% colormap summer;
% xlim([50,950]);
% ylim([0,4]);




