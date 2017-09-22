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
%
% Modified by (c) Grzegorz Skołyszewski:
% Added generation for more algorithms and realistic ratio
% AGH University of Science and Technology
% Telecommunications Department
% Krakow, Poland
% 12.06.2017
% skolyszewski.grzegorz@gmail.com
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
close all;

size = 4096;

fs1 = 100;
fc1 = 20;
fs2 = 300;
fc2 = 5;

ratio = 3/2;

realistic_ratio = 0.99994;

t1 = linspace(0, size/fs1-1/fs1, size);
x1 = sin(fc1*t1);
t2 = linspace(0, size/fs2-1/fs2, size);
x2 = sin(fc2*t2);

tq1 = linspace(0,size/fs1 - 1/fs1, size*ratio);
tq2 = linspace(0,size/fs2 - 1/fs2, size*ratio);
tqr1 = linspace(0,size/fs1 - 1/fs1, size*realistic_ratio);
tqr2 = linspace(0,size/fs2 - 1/fs2, size*realistic_ratio);

y1 = resample(x1, 3, 2);
y2 = resample(x2, 3, 2);

y01 = interp1(t1,x1,tq1,'nearest');
y02 = interp1(t2,x2,tq2,'nearest');

y11 = interp1(t1,x1,tq1,'linear');
y12 = interp1(t2,x2,tq2,'linear');

y21 = interp1(t1,x1,tq1,'pchip');
y22 = interp1(t2,x2,tq2,'pchip');

y31 = interp1(t1,x1,tq1,'spline');
y32 = interp1(t2,x2,tq2,'spline');

y01realistic = interp1(t1,x1,tqr1,'nearest');
y02realistic = interp1(t2,x2,tqr2,'nearest');

y11realistic = interp1(t1,x1,tqr1,'linear');
y12realistic = interp1(t2,x2,tqr2,'linear');

y21realistic = interp1(t1,x1,tqr1,'pchip');
y22realistic = interp1(t2,x2,tqr2,'pchip');

y31realistic = interp1(t1,x1,tqr1,'spline');
y32realistic = interp1(t2,x2,tqr2,'spline');

fp = fopen('../data/ut/resampler2_data.txt', 'w');
for i=1:length(x1)
	fprintf(fp, '%1.18f\n', x1(i));
	fprintf(fp, '%1.18f\n', x2(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler2_nn_expected_results.txt', 'w');
for i=1:length(y01)
	fprintf(fp, '%1.18f\n', y01(i));
	fprintf(fp, '%1.18f\n', y02(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler2_linear0_expected_results.txt', 'w');
for i=1:length(y1)
	fprintf(fp, '%1.18f\n', y1(i));
	fprintf(fp, '%1.18f\n', y2(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler2_linear_expected_results.txt', 'w');
for i=1:length(y11)
	fprintf(fp, '%1.18f\n', y11(i));
	fprintf(fp, '%1.18f\n', y12(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler2_pchip_expected_results.txt', 'w');
for i=1:length(y21)
	fprintf(fp, '%1.18f\n', y21(i));
	fprintf(fp, '%1.18f\n', y22(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler2_spline_expected_results.txt', 'w');
for i=1:length(y31)
	fprintf(fp, '%1.18f\n', y31(i));
	fprintf(fp, '%1.18f\n', y32(i));
end
fclose(fp);

% FOR REALISTIC RATIO TESTS

fp = fopen('../data/ut/resampler2_realistic_nn_expected_results.txt', 'w');
for i=1:length(y01realistic)
	fprintf(fp, '%1.18f\n', y01realistic(i));
	fprintf(fp, '%1.18f\n', y02realistic(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler2_realistic_linear_expected_results.txt', 'w');
for i=1:length(y11realistic)
	fprintf(fp, '%1.18f\n', y11realistic(i));
	fprintf(fp, '%1.18f\n', y12realistic(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler2_realistic_pchip_expected_results.txt', 'w');
for i=1:length(y21realistic)
	fprintf(fp, '%1.18f\n', y21realistic(i));
	fprintf(fp, '%1.18f\n', y22realistic(i));
end
fclose(fp);

fp = fopen('../data/ut/resampler2_realistic_spline_expected_results.txt', 'w');
for i=1:length(y31realistic)
	fprintf(fp, '%1.18f\n', y31realistic(i));
	fprintf(fp, '%1.18f\n', y32realistic(i));
end
fclose(fp);
