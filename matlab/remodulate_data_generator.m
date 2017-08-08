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

close all;
clear all;

N = 196608;
fs = 2048000;
s = 0:2*pi/(N-1):2*pi;

data = sin(s) + (sqrt(-1)*sin(s));

%%% HIGH FC
fc = 7164.042;
data_remodulated = data' .* exp(-sqrt(-1)*2*pi*fc/fs*[1:N]');

fp_h = fopen('../data/ut/remodulate_input_data.txt', 'w');

for i = 1:length(data)    
    fprintf(fp_h, '%1.18f\n', real(data(i)));
    fprintf(fp_h, '%1.18f\n', imag(data(i)));
end

fp2_h = fopen('../data/ut/remodulate_output_data.txt', 'w');

for i = 1:length(data_remodulated)
    fprintf(fp2_h, '%1.18f\n', real(data_remodulated(i)));
    fprintf(fp2_h, '%1.18f\n', imag(-data_remodulated(i)));
end

%%% LOW FC
fc = 357.982;
data_remodulated = data' .* exp(-sqrt(-1)*2*pi*fc/fs*[1:N]');

fp_l = fopen('../data/ut/remodulate_input_data1.txt', 'w');

for i = 1:length(data)    
    fprintf(fp_l, '%1.18f\n', real(data(i)));
    fprintf(fp_l, '%1.18f\n', imag(data(i)));
end

fp2_l = fopen('../data/ut/remodulate_output_data1.txt', 'w');

for i = 1:length(data_remodulated)
    fprintf(fp2_l, '%1.18f\n', real(data_remodulated(i)));
    fprintf(fp2_l, '%1.18f\n', imag(-data_remodulated(i)));
end

%%% ZERO FC
fc = 0;
data_remodulated = data' .* exp(-sqrt(-1)*2*pi*fc/fs*[1:N]');

fp_z = fopen('../data/ut/remodulate_input_data2.txt', 'w');

for i = 1:length(data)    
    fprintf(fp_z, '%1.18f\n', real(data(i)));
    fprintf(fp_z, '%1.18f\n', imag(data(i)));
end

fp2_z = fopen('../data/ut/remodulate_output_data2.txt', 'w');

for i = 1:length(data_remodulated)
    fprintf(fp2_z, '%1.18f\n', real(data_remodulated(i)));
    fprintf(fp2_z, '%1.18f\n', imag(-data_remodulated(i)));
end

%%% LOW NEGATIVE FC
fc = -244.065;
data_remodulated = data' .* exp(-sqrt(-1)*2*pi*fc/fs*[1:N]');

fp_ln = fopen('../data/ut/remodulate_input_data3.txt', 'w');

for i = 1:length(data)    
    fprintf(fp_ln, '%1.18f\n', real(data(i)));
    fprintf(fp_ln, '%1.18f\n', imag(data(i)));
end

fp2_ln = fopen('../data/ut/remodulate_output_data3.txt', 'w');

for i = 1:length(data_remodulated)
    fprintf(fp2_ln, '%1.18f\n', real(data_remodulated(i)));
    fprintf(fp2_ln, '%1.18f\n', imag(-data_remodulated(i)));
end

%%% HIGH NEGATIVE FC
fc = -6965.253;
data_remodulated = data' .* exp(-sqrt(-1)*2*pi*fc/fs*[1:N]');

fp_hn = fopen('../data/ut/remodulate_input_data4.txt', 'w');

for i = 1:length(data)    
    fprintf(fp_hn, '%1.18f\n', real(data(i)));
    fprintf(fp_hn, '%1.18f\n', imag(data(i)));
end

fp2_hn = fopen('../data/ut/remodulate_output_data4.txt', 'w');

for i = 1:length(data_remodulated)
    fprintf(fp2_hn, '%1.18f\n', real(data_remodulated(i)));
    fprintf(fp2_hn, '%1.18f\n', imag(-data_remodulated(i)));
end