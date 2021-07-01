N = 50;
y = zeros(N+2,0);
sf = 0.5;
nd = 30;
d = ((0.01 * sf / (1-sf)))^(1/nd);
n = -2:(N-1);
y(2) =(d - (d-1) * sf) / d;
y(1) = ((1+d) * y(2) - 1)/d;


for i = 3:(N+2)
   
    y(i) = ((d+1) * y(i-1) - d * y(i-2));
    
end

stem(n, y);

