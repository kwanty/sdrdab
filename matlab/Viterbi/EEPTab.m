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
% Table of Equal Error Protection (EEP) - from ETSI EN300401, page 135-136, Tables 33-36
% -----------------------------------------------------------------------------------------

function [Audiokbps, PI1, PI2, L1Range, L2Range] = EEPTab (Protection, SubChSize)

if Protection(2) == 'A'             % EEP A Protection
    
    switch Protection(1)
        
        case '1'                    % Protection level 1
            n = SubChSize/12;
            PI1 = 24;
            PI2 = 23;
            L1 = 6*n-3;
            L2 = 3;
            
        case '2'                    % Protection level 2
            n = SubChSize/8;
            if n == 1
                PI1 = 13;
                PI2 = 12;
                L1 = 5;
                L2 = 1;
            else
                PI1 = 14;
                PI2 = 13;
                L1 = 2*n-3;
                L2 = 4*n+3;
            end
            
        case '3'                    % Protection level 3
            n = SubChSize/6;
            PI1 = 8;
            PI2 = 7;
            L1 = 6*n-3;
            L2 = 3;
            
        case '4'                    % Protection level 4
            n = SubChSize/4;
            PI1 = 3;
            PI2 = 2;
            L1 = 4*n-3;
            L2 = 2*n+3;
    end
    Audiokbps = n * 8;
    
else                                % EEP B Protection
    
   switch Protection(1)
       
        case '1'                    % Protection level 1
            n = SubChSize/27;
            PI1 = 10;
            PI2 = 9;
        case '2'                    % Protection level 2
            n = SubChSize/21;
            PI1 = 6;
            PI2 = 5;
        case '3'                    % Protection level 3
            n = SubChSize/18;
            PI1 = 4;
            PI2 = 3;
        case '4'                    % Protection level 4
            n = SubChSize/15;
            PI1 = 2;
            PI2 = 1;
   end
    Audiokbps = n * 32;
    L1 = 24*n-3;
    L2 = 3;
end

L1Range = L1*128*(PI1+8)/32;
L2Range = L2*128*(PI2+8)/32 + L1Range;
