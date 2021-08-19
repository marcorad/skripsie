filenames = ["sine", "triangle", "sawtooth", "square"];


for k = 1:length(filenames)
  file = append('..\basic waveforms\', filenames(k));
  fprintf(1, 'Now reading %s\n', file);
  fig = figure(k);
  hold on;
  for n = 0:7
    f = file + '\' + string(n) + '.txt';
    wave = importdata(f); 
    plot(wave);
    ylabel('normalised f[n]');
    xlabel('n');
    title(filenames(k));
  end
end