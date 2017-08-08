%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Masterthesis
% Zurcher Hochschule fur Angewandte Wissenschaften
% Zentrum fur Signalverarbeitung und Nachrichtentechnik
% (c) Michael Hoin
% 12.4.2011 ZSN
% info.zsn@zhaw.ch
%
% Code modified/extended (to DAB+ and Modes 2,3,4) by:
% (c) Tomasz Zielinski
% AGH University of Science and Technology
% Telecommunications Department
% Krakow, Poland
% 15.03.2015
% tzielin@agh.edu.pl
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
close all;

size = 4096;
fs = 100;
fc = 20;

t = linspace(0, size/fs-1/fs, size);
x = sin(fc*t);

y = resample(x, 3, 2);

fp = fopen('../data/ut/resampler_linear_data.txt', 'w');
for i=1:length(x)
	fprintf(fp, '%1.18f\n', x(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler_linear_expected_results.txt', 'w');
for i=1:length(y)
	fprintf(fp, '%1.18f\n', y(i));
end
fclose(fp);
