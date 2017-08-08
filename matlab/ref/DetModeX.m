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

% -----------------------------------------------------------------------------------------
% Mode detection
% -----------------------------------------------------------------------------------------

function [ Mode, NSampPerFrame, NSampPerNull, NSymbPerFrame, NSampPerSymb, NSampPerGuard, Nfft, NCarrPerSymb, FreqIntTabSize, Offset, NCIFsPerFrame, NSymbPerFIC ] = DetModeX( Signal, FrameStartpoints )

% Mode      I       II     III    IV             % from ETSI EN300401, page 145, Table 38
  Tab38 = [ 76,     76,    153,   76;            % 1, L, the number of OFDM symbols per one TX DAB frame (without NULL symbol)
            1536,   384,   192,   768;           % 2, K, the number of transmitted carriers
            196608, 49152, 49152, 98304;         % 3, Tf, the transmission frame duration (in samples)
            2656,   664,   345,   1328;          % 4, TNULL, the Null symbol duration (in samples)
            2552,   638,   319,   1276;          % 5, Ts, the duration of OFDM symbols of indices: l=1,2,3,...,L
            2048,   512,   256,   1024;          % 6, Tu, the inverse of the carrier spacing
            504,    126,   63,    252;           % 7, delta, the duration of guard interval 
            4,      1,     1,     2;             % 8, ADDED, number of CIFs per one DAB frame
            3,      3,     8,     3; ];          % 9, ADDED, number of OFDM frames per FIC

n1 = FrameStartpoints(end-1);  % start point of the last-1 DAB frame    
n2 = FrameStartpoints(end);    % start point of the last   DAB frame

Samples = n2 - n1;             % DAB frame duration in samples

if( Samples > 3*49152 )         Mode=1;
elseif( Samples > 1.5*49152 )   Mode=4;    
else                                            
    if( mean(abs(Signal(n2-664 : n2-345-1))) > 3*mean(abs(Signal(n2-345 : n2-1))) )           
        Mode=3;                 % !!! in Modes 2 and 3
    else                        % !!! number of samples is the same = 49152!
        Mode=2;                 % !!! TNULL should be used for proper choice!
    end                         % !!! NSampPerNull = 664 (Mode 2) and 345 (Mode 3)
end                                                       

                                   % For Mode=1
NSymbPerFrame = Tab38(1,Mode);     % 76          % L, the number of OFDM symbols per one DAB frame (without NULL symbol)                        
NCarrPerSymb  = Tab38(2,Mode);     % 1536        % K, number of transmitted carriers
NSampPerFrame = Tab38(3,Mode);     % 196608      % Tf, the transmission frame duration (in samples)
NSampPerNull  = Tab38(4,Mode);     % 2656        % TNULL, the Null symbol duration (in samples)
NSampPerSymb  = Tab38(5,Mode);     % 2552        % Ts, the duration of OFDM symbols of indices: l=1,2,3,...,L
Nfft          = Tab38(6,Mode);     % 2048        % Tu, the inverse of the carrier spacing
NSampPerGuard = Tab38(7,Mode);     % 504         % delta, the duration of guard interval
NCIFsPerFrame = Tab38(8,Mode);     % 4           % ADDED, number of CIFs per one DAB frame
NSymbPerFIC   = Tab38(9,Mode);     % 3           % ADDED, number of OFDM frames per FIC

FreqIntTabSize = Nfft;
NSampPerGuard  = NSampPerSymb - Nfft;
NSampPerFrame  = NSymbPerFrame*NSampPerSymb + NSampPerNull;
Offset         = NSampPerGuard/2;          % 100 = very good, 0 = too fast?, above 500 bad (see Guard Time)
