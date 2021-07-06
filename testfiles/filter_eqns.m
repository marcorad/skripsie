syms Q s zr w0 fs;
Hlps = w0^2/(s^2 + w0*s/Q + w0^2);
Hhps = w0^2*s^2/(s^2 + w0*s/Q + w0^2);
K = 2 * fs * (1/zr-1)/(1/zr+1);

Hlpz = collect(simplifyFraction(subs(Hlps, s, K)), zr)
Hhpz = collect(simplifyFraction(subs(Hhps, s, K)), zr)
