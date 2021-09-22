x = linspace(0, 200*pi, 100000);
y = sin(x);
a = 1;
g = 64;

H = zeros(g, 1); %stores the max harmonic that's 60dB below fund
for n = 1:g

A = a * n;
Y = abs(fft(tanh(A * y)));
[m, fund] = max(Y);
Y = 20 * log10 (Y / m);
%plot(Y);

count = 0;

for i = 2:256
k = (fund-1) * i + 1;

h = Y(k);

if h > -60
    count = count + 1;
    H(A) = i;
end

end

"count A=" + string(A) + " : " + string(count)

    
end

n = 1:g;
H = 1 ./ H;
stem(n, H);
hold on;
xlabel("gain");
ylabel("harmonic");

myfit = fittype('a + b^n',...
'dependent',{'y'},'independent',{'n'},...
'coefficients',{'a','b'});
%fit(n',H,myfit)
%plot( n,   0.01197  + 0.2208 .^ n );