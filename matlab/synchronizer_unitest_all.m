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

%% Clear environment
clear all
close all

fprintf('\n*************** synchronizer_all generator started ***************\n');

%% Init parameters
SuffixFileName = '';
input_filename = 'Record3_katowice_iq.raw';
nr_test_files = 3;

%% Start script
cd synchronizer_unitest_all/
DAB_ALL_RT
cd ..

%% Init parameters
clear all
SuffixFileName = '_gain42';
input_filename = 'antena-1_dab_229072kHz_fs2048kHz_gain42_1.raw';
nr_test_files = 3;

%% Start script
cd synchronizer_unitest_all/
DAB_ALL_RT
cd ..