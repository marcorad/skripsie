files = importdata("files.txt");
fs = 44100;

for i = 1:size(files)
    f = char(files(i));
    figure(i);
    y = importdata(f);
    y = y/2^16 - 0.5;
    n = 0:(size(y)-1);
    subplot(2,1, 1);
    plot(n/fs,y);
    subplot(2,1, 2);
    semilogx(n/max(n)*fs,20*log(abs(fft(y))));
    xlim([0,20000])
    title(f);
end
