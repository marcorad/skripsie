fs = 44100;
G = 2*pi;
G_prime = max(1/G, 1);
f0 = 0.5 * 0.142857142857143; %multiply by emperical calcs in tanh_harm.m
w = 2*pi*f0;
sinw = sin(w);
cosw = cos(w);
T = 120;
Q = 1/sqrt(2);

%LPF12
a0 = (1 - cosw + sinw);
a1 = 2 * ( 1 - cosw);
a2 = 1 - cosw - sinw;

b0 = 1 - cosw;
b2 = b0;
b1 = 2 * b0;

%LPF24
% b0 = Q*(1-cosw);
% b1 = 2 * b0;
% b2 = b0;
% 
% a0 = 2*Q + sinw;
% a1 = -4*Q*cosw;
% a2 = 2*Q - sinw;


N = 10000;

freqz([b0,b1,b2], [a0,a1,a2], N, fs);