N = 50;
y = zeros(N+2,0);
sf = 0.5;
nr = 30;
r = (1/3)^(1/nr);
n = -2:(N-1);
y(2) =((1+r) * sf - sf*(3*r-1)/2)/r;
y(1) = ((1+r) * y(2) - sf)/r;


for i = 3:(N+2)
   
    y(i) = ((r+1) * y(i-1) - r * y(i-2));
    
end

stem(n, y);

