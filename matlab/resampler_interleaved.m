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

fs1 = 100;
fc1 = 20;
fs2 = 300;
fc2 = 5;

t1 = linspace(0, size/fs1-1/fs1, size);
x1 = sin(fc1*t1);
t2 = linspace(0, size/fs2-1/fs2, size);
x2 = sin(fc2*t2);

y1 = resample(x1, 3, 2);
y2 = resample(x2, 3, 2);

fp = fopen('../data/ut/resampler_interleaved_data.txt', 'w');
for i=1:length(x1)
	fprintf(fp, '%1.18f\n', x1(i));
	fprintf(fp, '%1.18f\n', x2(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler_interleaved_expected_results.txt', 'w');
for i=1:length(y1)
	fprintf(fp, '%1.18f\n', y1(i));
	fprintf(fp, '%1.18f\n', y2(i));
end
fclose(fp);
