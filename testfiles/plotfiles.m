files = importdata("files.txt");
fs = 44100;

for i = 1:size(files)
    f = char(files(i));
    figure(i);
    N = size(y);
    N = N(1);
    y = importdata(f);
    y = y/2^15;
    Y = abs(fft(y));
    n = 0:(N-1); 
    k = n;
    subplot(2,1, 1);
    plot(n/fs,y);
    subplot(2,1, 2);
    plot(k/max(k)*fs,20*log10(Y));
    xlim([0,fs/2])
    ylim([-50,100]);
    title(f); 
end
