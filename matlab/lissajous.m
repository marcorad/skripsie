
plot_lissajous("0");
hold on;
plot_lissajous("1");
plot_lissajous("2");
plot_lissajous("3");

k = 2.2;
% plot([0, 0], [-k, k], 'LineWidth', 1, 'LineStyle', '-', 'Color', 'black');
% plot([0, -k], [0, k], 'LineWidth', 0.8, 'LineStyle', '--', 'Color', 'black');
% plot([0, k], [0, k], 'LineWidth', 0.8, 'LineStyle', '--', 'Color', 'black');


% xlim([-k/2,k/2]);
% ylim([-k,k]);

xlabel("Side channel");
ylabel("Mid channel");


legend("\delta_w = 0.0", "\delta_w = 0.33", "\delta_w = 0.67", "\delta_w = 1.0");
title("XY-plot of Mid-Side stereo content (\delta_v = 1)");


function plot_lissajous(n)
    t = 0;
    rot = [cos(t) -sin(t); sin(t) cos(t)]; % rotation matrix
    L = importdata("../testfiles/generator/width L " + n + ".txt");
    R = importdata("../testfiles/generator/width R " + n + ".txt");
    X = [L';R'];

    X = rot * X;
    L = X(1, :);
    R = X(2, :);
    %plot(L, R, 'LineWidth', 1.2);
    scatter( L/2-R/2,L/2+R/2,20, '.');
end