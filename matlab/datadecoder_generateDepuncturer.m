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

function datadecoder_generateDepuncturer

% GENERATING MSC DATA (DAB)
block1 = randi(2.99e11,4,836); pi1 = 11;
block2 = randi(2.99e11,4,1176); pi2 = 6;
block3 = randi(2.99e11,4,3172); pi3 = 8;
block4 = randi(2.99e11,4,180); pi4 = 7;
block5 = randi(2.99e11,4,12); pi5 = 8;
pad = randi(1e4,1,864);
%

DataWithoutPad = [block1 block2 block3 block4 block5];
DataWithPad = [DataWithoutPad(1,1:end) pad DataWithoutPad(2, 1:end) pad DataWithoutPad(3, 1:end) pad DataWithoutPad(4, 1:end) ];
DepuncturedData = [depuncturing(block1,pi1) depuncturing(block2,pi2) depuncturing(block3,pi3) depuncturing(block4,pi4) depuncturing(block5, pi5)];

fp = fopen('../data/ut/depuncturer_data_in_msc_11_6_8_7_8_dab.txt','w');

for i=1:length(DataWithPad)
   fprintf( fp, '%f\n', DataWithPad(i) );
end
fclose( fp );

fp = fopen('../data/ut/depuncturer_data_out_msc_11_6_8_7_8_dab.txt','w');

for i=1:4
    for l=1:length(DepuncturedData)
        fprintf( fp, '%f\n', DepuncturedData(i,l) );
    end
end
fclose( fp );

% GENERATING MSC DATA (DAB+)
block1 = randi(2.99e11,4,5184); pi1 = 8;
block2 = randi(2.99e11,4,180); pi2 = 7;
block3 = randi(2.99e11,4,12); pi3 = 8;
pad = randi(1e4,1,768);
%

DataWithoutPad = [block1 block2 block3];
DataWithPad = [DataWithoutPad(1,1:end) pad DataWithoutPad(2, 1:end) pad DataWithoutPad(3, 1:end) pad DataWithoutPad(4, 1:end)];
DepuncturedData = [depuncturing(block1,pi1) depuncturing(block2,pi2) depuncturing(block3,pi3)];

fp = fopen('../data/ut/depuncturer_data_in_msc_8_7_8_dabp.txt','w');
for i=1:length(DataWithPad)
   fprintf( fp, '%f\n', DataWithPad(i) );
end
fclose( fp );

fp = fopen('../data/ut/depuncturer_data_out_msc_8_7_8_dabp.txt','w');
for i=1:4
    for l=1:length(DepuncturedData)
        fprintf( fp, '%f\n', DepuncturedData(i,l) );
    end
end
fclose( fp );

% GENERATING FIC DATA
block1 = randi(2.99e11,4,2016); pi1 = 16;
block2 = randi(2.99e11,4,276); pi2 = 15;
block3 = randi(2.99e11,4,12); pi3 = 8;
%

DataWithoutPad = [block1 block2 block3];
DepuncturedData = [depuncturing(block1,pi1) depuncturing(block2,pi2) depuncturing(block3,pi3)];

fp = fopen('../data/ut/depuncturer_data_in_fic_16_15_8.txt','w');

for i=1:4
    for l=1:length(DataWithoutPad)
        fprintf( fp, '%f\n', DataWithoutPad(i,l) );
    end
end
fclose( fp );

fp = fopen('../data/ut/depuncturer_data_out_fic_16_15_8.txt','w');

for i=1:4
    for l=1:length(DepuncturedData)
        fprintf( fp, '%f\n', DepuncturedData(i,l) );
    end
end
fclose( fp );

end