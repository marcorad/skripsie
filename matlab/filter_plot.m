Q = 3;
f0 = 5000; %Hz
fs = 44100;
w = 2*pi*f0/fs;
% same for all biquads
sinw = sin(w);
cosw = cos(w);
a0 = 2*Q + sinw;
a1 = -4*Q*cosw;
a2 = 2*Q - sinw;
den = [a0,a1,a2];

figure();
hold on;

% biquad dependent
%HP24
b1 = -2 * Q *(1+cosw);
b0 = -0.5*b1;
b2 = b0;
num = [b0,b1,b2];
[H, F] = freqz(num, den, 1000, fs);
semilogx(F/1000, 20*log10(abs(H)));

%LP24
b0 = Q*(1-cosw);
b1 = 2 * b0;
b2 = b0;
num = [b0,b1,b2];
[H, F] = freqz(num, den, 1000, fs);
semilogx(F/1000, 20*log10(abs(H)));

%BP12
b0 = Q*sinw;
b1 = 0;
b2 = -Q*sinw;
num = [b0,b1,b2];
[H, F] = freqz(num, den, 1000, fs);
semilogx(F/1000, 20*log10(abs(H)));

legend('HP24','LP24','BP12');
%xlim([100,10000]);
ylim([-60, 10])
xlabel('Frequency [kHz]');
ylabel('20log_{10}|H(f)|')
title('Digitised 2-pole filters (f_0 = ' + string(f0/1000) + ' kHz, Q = ' + string(Q) + ')');

figure();
hold on;



a0 = 1 - cosw + sinw;
a1 = 2 * ( 1 - cosw);
a2 = 1 - cosw - sinw;
den = [a0,a1,a2];

%HP12
b0 = sinw;
b1 = 0;
b2 = -b0;
num = [b0,b1,b2];
[H, F] = freqz(num, den, 1000, fs);
semilogx(F/1000, 20*log10(abs(H)));

%LP12
b0 = 1 - cosw;
b1 = 2 * b0;
b2 = b0;
num = [b0,b1,b2];
[H, F] = freqz(num, den, 1000, fs);
semilogx(F/1000, 20*log10(abs(H)));

legend('HP12','LP12');
ylim([-60, 10])
xlabel('Frequency [kHz]');
ylabel('20log_{10}|H(f)|')
title('Digitised 1-pole filters (f_0 = ' + string(f0/1000) + ' kHz)');

