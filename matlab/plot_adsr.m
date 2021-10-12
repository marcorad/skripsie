N=250;
P = [0.67, 0.8, 0.95]
adsr(P, N);

function adsr(P, N)
    n = 0:(N - 1);
    y = zeros(1,4*N);
    L = strings(1, length(P));
    for i = 1:length(P)
        p = P(i);
        K = 1/p;
        R = (1-p)^(1/(N-1));
        s = 1; e = N;
        y(s:e) = K*(1-power(R, n));
        s = e + 1; e = e + N;
        y(s:e) = 1 - 0.5*K*(1-power(R, n));
        s = e + 1; e = e + N;
        y(s:e) = 0.5 * ones(1, N);
        s = e + 1; e = e + N;
        y(s:e) = (1 - K*(1-power(R, n))) * 0.5;
        hold on;
        plot(0:(4*N-1), y);
        xlim([0, 1000]);
        ylim([0,1]);
        L(i) = "p = " + p;
    end
    xlabel("n");
    ylabel("ADSR[n]");
    title("ADSR envelope");
    legend(L);
    
    
end

    
