

L=3;
f = linspace(0,1,1000);
y = h(L, f);
b = zeros(length(f));
hold on;
title("The frequency reponse of linear interpolation (L=" + string(L) + ")");
plot(f, y);
plot(f, abs(sawtooth(2*pi*L*f)));
legend({"H(f)","Y(f)_{\uparrow " + string(L) + "}"} );
xlabel("f [cycles/sample]")

function y = h(L, f)    
    y = sin(L*pi*f).^2 ./ L ./ sin(pi*f).^2;
end