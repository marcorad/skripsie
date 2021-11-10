
file = "4";
desc = " (vintage detune) - waveform";

x = [0.956, 0.9565];
y = [-0.13, 0.13];

lr = importdata("../wav/moonlight " + file + ".wav").data;
s = size(lr(:, 1));
t = linspace(0, 1, s(1))';

subplot(2,1, 1);
plot(t, lr(:, 1));
title("Left channel");
xlim(x); ylim(y);

subplot(2,1, 2);
plot(t, lr(:, 2));
title("Right channel");
xlabel("time [s]");
xlim(x); ylim(y);

suptitle("Audio for file " + file + desc);

set(gcf, 'Position',[500,500, 450, 300]);




