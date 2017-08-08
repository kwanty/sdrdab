%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Biela Ernest
% ernest.biela@gmail.com
% 30.06.2015
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function generatetimedeinterleaverdata
fprintf('\n*************** timedeinterleaver generator started ***************\n\n');
fprintf('\n*************** MODE 1 ***************\n\n');
SCS = 72;
sub_ch_size = SCS
Delay = 4*SCS;                                              % Example: SubChSize=84, Delay=336
DLR = zeros( 16, 19*Delay );
FS = 2304;
fic_size = FS
%TiDeIntSubChDataOut
% GENERATING MSC DATA (MODE_1)

block1 = randi(2.99e11,4,SCS*64);
block2 = randi(2.99e11,4,SCS*64);
block3 = randi(2.99e11,4,SCS*64);
block4 = randi(2.99e11,4,SCS*64);
block5 = randi(2.99e11,4,SCS*64);
%pad = randi(1e4,1,864);
pad = zeros(1,1536);
FIC = zeros(1,FS);
PS = length(pad);
pading_size = PS
%
TiDeIntSubChDataOut = zeros(20, SCS*64); 
DataWithPad = zeros( 5, (((SCS*64)+PS)*4)+FS);
DataWithoutPad = [block1; block2; block3; block4; block5];
for i=1:5
    DataWithPad(i,:) = [DataWithoutPad(1+(i-1)*4,:) pad DataWithoutPad(2+(i-1)*4,:) pad DataWithoutPad(3+(i-1)*4,:) pad DataWithoutPad(4+(i-1)*4,:) pad FIC];
end
DataToInputFile = [ zeros(1, 4* ( (PS*4) + (4*SCS*64) + FS) ) DataWithPad(1,:) DataWithPad(2,:) DataWithPad(3,:) DataWithPad(4,:) DataWithPad(5,:)];


%length(DataToInputFile)
%length(DataWithPad)*5
[TiDeIntSubChDataOut(1:4,:) , DLR] = TimeDeinterleavingStream(block1, SCS, DLR, 0);
[TiDeIntSubChDataOut(5:8,:) , DLR] = TimeDeinterleavingStream(block2, SCS, DLR, 0);
[TiDeIntSubChDataOut(9:12,:) , DLR] = TimeDeinterleavingStream(block3, SCS, DLR, 0);
[TiDeIntSubChDataOut(13:16,:) , DLR] = TimeDeinterleavingStream(block4, SCS, DLR, 0);
[TiDeIntSubChDataOut(17:20,:) , DLR] = TimeDeinterleavingStream(block5, SCS, DLR, 0);

%OutputDataWithPad = [ TiDeIntSubChDataOut(1,:) pad TiDeIntSubChDataOut(2,:) pad TiDeIntSubChDataOut(3,:) pad TiDeIntSubChDataOut(4,:) pad FIC ];
%OutputDataWithPad = [ TiDeIntSubChDataOut(5,:) pad TiDeIntSubChDataOut(6,:) pad TiDeIntSubChDataOut(7,:) pad TiDeIntSubChDataOut(8,:) pad FIC ];
OutputDataWithPad = [ TiDeIntSubChDataOut(17,:) pad TiDeIntSubChDataOut(18,:) pad TiDeIntSubChDataOut(19,:) pad TiDeIntSubChDataOut(20,:) pad FIC ];
input_size = length(DataToInputFile)
output_size = length(OutputDataWithPad)
fp = fopen('../data/ut/timedeinterleaver_data_in_mode_1.txt','w');
for i=1:length(DataToInputFile)
   fprintf( fp, '%f\n', DataToInputFile(i) );
end
fclose( fp );

fp = fopen('../data/ut/timedeinterleaver_data_out_mode_1.txt','w');

for i=1:length(OutputDataWithPad)
    fprintf( fp, '%f\n', OutputDataWithPad(i) );
end
fclose( fp );

clear;
close all;
fprintf('\n*************** MODE 2 & 3 ***************\n\n');
NCIF = 1;
SCS = 72;
PS = 1536;
sub_ch_size = SCS
Delay = 4*SCS;                                              % Example: SubChSize=84, Delay=336
DLR = zeros( 16, 19*Delay );
FS = 2304;
fic_size = FS
pading_size = PS
%TiDeIntSubChDataOut
% GENERATING MSC DATA (MODE_II)

block = randi(2.99e11,17,SCS*64);
%pad = randi(1e4,1,864);
pad = zeros(1,PS);
FIC = zeros(1,FS);
%
TiDeIntSubChDataOut = zeros(20, SCS*64); 
DataWithPad = zeros( 17, (((SCS*64)+PS)*NCIF)+FS);
DataWithoutPad = block;
for i=1:17
    DataWithPad(i,:) = [DataWithoutPad(i,:) pad FIC];
end
%DataToInputFile_mode_2 = [ zeros(1, 33 *  (PS + (SCS*64) + FS)  ) ];
DataToInputFile_mode_2 = [ zeros(1, 4* ( (PS*4) + (4*SCS*64) + (4*FS) ) ) DataWithPad(1,:) DataWithPad(2,:) DataWithPad(3,:) DataWithPad(4,:) ...
                                                                   DataWithPad(5,:) DataWithPad(6,:) DataWithPad(7,:) DataWithPad(8,:) ...
                                                                   DataWithPad(9,:) DataWithPad(10,:) DataWithPad(11,:) DataWithPad(12,:) ...
                                                                   DataWithPad(13,:) DataWithPad(14,:) DataWithPad(15,:) DataWithPad(16,:) DataWithPad(17,:)];


%length(DataToInputFile)
%length(DataWithPad)*5
for i=1:17
    [TiDeIntSubChDataOut(i,:) , DLR] = TimeDeinterleavingStream(block(i,:), SCS, DLR, 0);
end

%OutputDataWithPad = [ TiDeIntSubChDataOut(1,:) pad TiDeIntSubChDataOut(2,:) pad TiDeIntSubChDataOut(3,:) pad TiDeIntSubChDataOut(4,:) pad FIC ];
%OutputDataWithPad = [ TiDeIntSubChDataOut(5,:) pad TiDeIntSubChDataOut(6,:) pad TiDeIntSubChDataOut(7,:) pad TiDeIntSubChDataOut(8,:) pad FIC ];
OutputDataWithPad = [ TiDeIntSubChDataOut(17,:) pad FIC ];
input_size = length(DataToInputFile_mode_2)
output_size = length(OutputDataWithPad)
fp = fopen('../data/ut/timedeinterleaver_data_in_mode_2&3.txt','w');
for i=1:length(DataToInputFile_mode_2)
   fprintf( fp, '%f\n', DataToInputFile_mode_2(i) );
end
fclose( fp );

fp = fopen('../data/ut/timedeinterleaver_data_out_mode_2&3.txt','w');

for i=1:length(OutputDataWithPad)
    fprintf( fp, '%f\n', OutputDataWithPad(i) );
end
fclose( fp );



fprintf('\n*************** timedeinterleaver generator ended ***************\n\n');

end
