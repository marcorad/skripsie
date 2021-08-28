y = importdata('..\testfiles\220 genL.txt');
plot_waveform_fft(y);


%L = 2;
%M = 20;
%n = 0:50000;
%g = sin(n/512*2*pi);
%plot_waveform_fft(g);
%xlim([0,1000]);
%hm = mfilt.linearinterp(L);
%freqz(hm);
%g = filter(hm ,g);
%plot_waveform_fft(g);
%g = downsample(g, M);
%plot_waveform_fft(g);


function plot_waveform_fft(y)
    figure();
    fs = 44100;
    n = 0:(length(y)-1);
    Y = abs(fft(y));
    Y = Y(1:length(y)/2);
    [m, fund] = max(Y); 
    fund = fund  * fs / length(y);
    f = 0:(length(y)/2-1);
    f = f * fs / length(y);
    subplot(2, 1, 1);
    plot(n / fs, y);
    ylabel("y[t]");
    %ylim([-2,2]);
    xlabel("t [sec]");
    %xlim([0, 1/fund * 2]); %5 periods
    subplot(2, 1, 2);
    semilogx(f, 20 * log10(Y/m)); %normalised at fundamental
    xlim([20,fs/2]);
    ylim([-100, 10]) %noise floor 100dB
    xlabel("f [Hz]");
    ylabel("Y[f]");
    soundsc(y, fs, 24);
end