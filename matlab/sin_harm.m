x = linspace(0, 200*pi, 100000);
y = sin(x);
a = pi/2;
g = 64;

H = zeros(g, 1); %stores the max harmonic that's 60dB below fund
for n = 1:g

A = a * n / g * 8; % from pi/32 to 4pi
Y = abs(fft(sin(A * y)));
[m, fund] = max(Y);
fund = 101; %100 + 1 due to MATLAB indexing
Y = 20 * log10 (Y / Y(101));
hold on;
plot(Y);

count = 0;

for i = 2:350
k = (fund-1) * i + 1;

h = Y(k);

if h > -40.2
    count = count + 1;
    H(n) = i;
end

end

"count A=" + string(A) + " : " + string(count)

    
end

ax = figure();
n = 1:g;
H = 1 ./ H;
stem(n/64 * 8 * pi/2, H, '.');
hold on;
xlabel("g");
ylabel("h_f^{-1}");
title("Final harmonic above -40 dB as a function of gain");
set(gca, 'XTick', 0:pi:4*pi);
set(gca, 'XTickLabel', {'0','\pi','2\pi','3\pi','4\pi'});
set(gcf, 'Position', [100,100,450,350]);

myfit = fittype('a + b^n',...
'dependent',{'y'},'independent',{'n'},...
'coefficients',{'a','b'});
%fit(n',H,myfit)
%plot( n,   0.01197  + 0.2208 .^ n );