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

function [Check] = CRC16(Data)

CRC = [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1];            % Initialization word
Polynomial = [0 0 0 1 0 0 0 0 0 0 1 0 0 0 0];       % [MSB ... LSB]

% size(Data)
Data(end-15:end) = xor( Data(end-15:end),CRC );     % complement of 16 last data

for i = 1:length(Data)                              % CRC calculation
    if( xor(CRC(1),Data(i)) == 1)
        CRC = [ xor( Polynomial,CRC(2:end) ) 1];
    else
        CRC = [ CRC(2:end) 0 ];
    end
end

if sum(CRC) == 0                                    % CRC verification
    Check = 1;
else
    Check = 0;
end