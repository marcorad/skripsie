L = importdata("../testfiles/manager/overall L.txt");
R = importdata("../testfiles/manager/overall R.txt");

t = linspace(0,1, 44100)';

subplot(2, 1, 1);
plot(t, L);
title("Left channel");
xlabel("time [s]");

subplot(2, 1, 2);
plot(t, L);
title("Right channel");
xlabel("time [s]");