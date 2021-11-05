t = 0;
rot = [cos(t) -sin(t); sin(t) cos(t)]; % rotation matrix
L = importdata("../testfiles/manager/overall L.txt");
R = importdata("../testfiles/manager/overall R.txt");
X = [L';R'];

X = rot * X;
L = X(1, :);
R = X(2, :);
%plot(L, R, 'LineWidth', 1.2);
scatter( L/2-R/2,L/2+R/2,1, '.');
