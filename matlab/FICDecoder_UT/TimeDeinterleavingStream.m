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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% -----------------------------------------------------------------------------------------
% Time interleaving
% -----------------------------------------------------------------------------------------
function [TiDeIntSubChDataOut, DataNextRound] = TimeDeInterleavingStream( NewSubChData, SubChSize, DataLastRound, DebugOn )
% ETSI pp. 137-142, table 37
%
% NewSubChData on input:        rows=NCIFsPerFrame, cols=64*SubChSize
% after ()':                    rows=64*SubChSize,  cols=NCIFsPerFrame
% after reshaing into 16 rows:  rows=16,            cols=(4*SubChSize)*NCIFsPerFrame

% For NCIFs=4 and SubChSize=84
% On input (4, 5376):           rows=4              cols=64*84=5376
% Then     (16,1344):           rows=16             cols=(4*SubChSize)*4=1344

  NewTiIntSubChData = reshape(NewSubChData',16,[]);   % dim: 16x1344   % transp. then reshape into 16 rows

  NCIFs = length( NewSubChData(:,1) );                % 4 for Mode 1   % NCIFsPerFrame
  Delay = 4*SubChSize;                                % 336=4*84       % delay
  
                                                                               % tab. 37
% Example        1+336*15=5041 : 336*(15+4)=6384      rows=16, cols=1344       % time de-interleaving:                                                        
  DataLastRound(1,  1+Delay*15 : Delay*(15 +NCIFs)) = NewTiIntSubChData(1,:);  % in row 1
  DataLastRound(2,  1+Delay*7  : Delay*( 7 +NCIFs)) = NewTiIntSubChData(2,:);  % in row 2
  DataLastRound(3,  1+Delay*11 : Delay*(11 +NCIFs)) = NewTiIntSubChData(3,:);  % in row 3
  DataLastRound(4,  1+Delay*3  : Delay*( 3 +NCIFs)) = NewTiIntSubChData(4,:);  % in row 4
  DataLastRound(5,  1+Delay*13 : Delay*(13 +NCIFs)) = NewTiIntSubChData(5,:);  % in row 5
  DataLastRound(6,  1+Delay*5  : Delay*( 5 +NCIFs)) = NewTiIntSubChData(6,:);  % in row 6
  DataLastRound(7,  1+Delay*9  : Delay*( 9 +NCIFs)) = NewTiIntSubChData(7,:);  % in row 7
  DataLastRound(8,  1+Delay*1  : Delay*( 1 +NCIFs)) = NewTiIntSubChData(8,:);  % in row 8
  DataLastRound(9,  1+Delay*14 : Delay*(14 +NCIFs)) = NewTiIntSubChData(9,:);  % in row 9
  DataLastRound(10, 1+Delay*6  : Delay*( 6 +NCIFs)) = NewTiIntSubChData(10,:); % in row 10
  DataLastRound(11, 1+Delay*10 : Delay*(10 +NCIFs)) = NewTiIntSubChData(11,:); % in row 11
  DataLastRound(12, 1+Delay*2  : Delay*( 2 +NCIFs)) = NewTiIntSubChData(12,:); % in row 12
  DataLastRound(13, 1+Delay*12 : Delay*(12 +NCIFs)) = NewTiIntSubChData(13,:); % in row 13
  DataLastRound(14, 1+Delay*4  : Delay*( 4 +NCIFs)) = NewTiIntSubChData(14,:); % in row 14
  DataLastRound(15, 1+Delay*8  : Delay*( 8 +NCIFs)) = NewTiIntSubChData(15,:); % in row 15
  DataLastRound(16, 1+Delay*0  : Delay*( 0 +NCIFs)) = NewTiIntSubChData(16,:); % in row 16
% Example      1             : 1334

if( DebugOn )
    disp('##### INSIDE function: TimeDeinterleaving');
    ERR_Delay = Delay-4*SubChSize,                                              % Delay = 4*SubChSize
    ERR_NewTiIntSubChData = size( NewTiIntSubChData ) - [16,Delay*NCIFs],       % 16 x Delay*NCIFs;                1344 
    ERR_DataLastRound = size( DataLastRound ) - [16,Delay*(16+NCIFs-1)], pause  % 16 x 16*Delay + (NCIFs-1)*Delay; 16 x 6384
end

% Circulat left rotation of rows -NCIFs*Delay = -4*312 = -1344 positions
  DataNextRound = circshift( DataLastRound, [0 -NCIFs*Delay] );

% Reshape column-wise first 64*SubChSize*NCIFs elements into 4 rows
  TiDeIntSubChDataOut = reshape( DataLastRound(1:64*SubChSize*NCIFs),[],NCIFs )'; % NCIFsPerFrame x SubChSize; 4CIFs*5376bits=21504


