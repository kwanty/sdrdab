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


fs = 100;
fc1 = 20;
fc2 = 0.001;
t = 0:1/fs:(1/fs*16384*35);
x1 = sin(fc1*t);
x2 = sin(fc2*t)+0.15;
signal=x1+x2;


fp = fopen('../data/ut/dcremoval_data.txt', 'w');
for i=1:length(signal)
	fprintf(fp, '%1.18f\n', signal(i));
end
fclose(fp);

last_dc_iterator = 1;
dc_memory=zeros(1,5);

for k=0:34
	subframe_mean = mean(signal(k*16384+1:2:(k+1)*16384));
	%subframe_mean
	dc_memory(last_dc_iterator)=subframe_mean;
	if last_dc_iterator<5
		last_dc_iterator=last_dc_iterator+1;
	else 
		last_dc_iterator = 1;
	end
	frame_mean = mean(dc_memory);
	%frame_mean
	signal(k*16384+1:2:(k+1)*16384) = signal(k*16384+1:2:(k+1)*16384) - frame_mean;
	%k
end



fp = fopen('../data/ut/dcremoval_expected_results.txt', 'w');
for i=1:length(signal)
	fprintf(fp, '%1.18f\n', signal(i));
end
fclose(fp);
