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
close all


x=randn(1,1024)+j*randn(1,1024);

fp = fopen('../data/ut/remodulator_input.txt', 'w');
for i=1:length(x)
	fprintf(fp, '%1.18f\n', real(x(i)));
	fprintf(fp, '%1.18f\n', imag(x(i)));
end
fclose(fp);

fs = 2.048e6;
fc_offset=30e3;

for i=1:length(x)
	z2(i) = x(i)* exp( j*2*pi*fc_offset/fs*(i-1) );
end

fp = fopen('../data/ut/remodulator_expected_results.txt', 'w');
for i=1:length(z2)
	fprintf(fp, '%1.18f\n', real(z2(i)));
	fprintf(fp, '%1.18f\n', imag(z2(i)));
end
fclose(fp);
