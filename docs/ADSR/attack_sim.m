N = 200;
y = zeros(N+2,0);
na = 100;
a = (1/3)^(1/na);
a = single(a);
n = -2:(N-1);
y(2) = 3/2*(1-1/a);
y(1) = 3/2*(1-1/a/a);


for i = 3:(N+2)
   
    y(i) = ((a+1) * y(i-1) - a * y(i-2));
    
end

plot(n, y);

