files = importdata("files.txt");
fs = 44100;
noisefloor = -6.02 * 12;


for i = 1:size(files)
    f = char(files(i));
    figure(i);
   
    y = importdata(f);
    N = size(y);
    N = N(1);
    y = y/2^15;
    if i == 1
       s = zeros(N,2); 
    end
    s(:,i) = y;
    Y = abs(fft(y));
    n = 0:(N-1); 
    k = n;
    subplot(2,1, 1);
    plot(n/fs,y);
    subplot(2,1, 2);
    semilogx(k/max(k)*fs,20*log10(Y/N*2));
    xlim([0,fs/2])
    ylim([noisefloor,0]);
    title(f);   
    spec = spectrogram(y);

    spectrogram(y,kaiser(10000,17), 512, 2048, 44100)
end
sound(s, fs);
