x = importdata("../testfiles/adsr/adsr retrigger.txt");
fs = 44100;
t = (0:(fs-1))/fs;
plot(t, x, 'LineWidth', 1.5, 'Color', 'black');
xlabel('time [s]');
title('ADSR envelope with retrigger (t_a=1/8, t_d=1/8, s=1/2, t_r=1/8)');


    
