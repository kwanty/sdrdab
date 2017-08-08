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
% Table of Unequal Error Protection (UEP) - from ETSI EN300401, page 134, Table 31
% -----------------------------------------------------------------------------------------
% First      L1 blocks shall be punctured according to the puncturing index PI1 (described in sec. 11.2.1)
% Next       L2 blocks shall be punctured according to the puncturing index PI2 (described in sec. 11.2.1)
% Next       L3 blocks shall be punctured according to the puncturing index PI3 (described in sec. 11.2.1)
% Remaining  L4 blocks shall be punctured according to the puncturing index PI4 (described in sec. 11.2.1)

function [L1Range, L2Range, L3Range, L4Range, PI1, PI2, PI3, PI4, Padding] = UEPTab(kbps, Protection)

switch kbps
    case 32  
        X = [3 4 17 0 5  3  2  0  0;   % rows in table 31: L1 L2 L3 L4 PI1 PI2 PI3 PI4 NumberOfPaddingBits
             3 3 18 0 11 6  5  0  0;
             3 4 14 3 15 9  6  8  0;
             3 4 14 3 22 13 8  13 0;
             3 5 13 3 24 17 12 17 4];
              
    case 48  
        X = [4 3 26 3 5 4 2 3 0;
             3 4 26 3 9 6 4 6 0;
             3 4 26 3 15 10 6 9 4;
             3 4 26 3 24 14 8 15 0;
             3 5 25 3 24 18 13 18 0];
              
    case 56  
        X = [6 10 23 3 5 4 2 3 0;
             6 10 23 3 9 6 4 5 0;
             6 12 21 3 16 7 6 9 0;
             6 10 23 3 23 13 8 13 8;
             0 0 0 0 0 0 0 0 0];
              
    case 64  
        X = [6 9 31 2 5 3 2 3 0;
             6 9 33 0 11 6 5 0 0;
             6 12 27 3 16 8 6 9 0;
             6 10 29 3 23 13 8 13 8;
             6 11 28 3 24 18 12 18 4];
              
    case 80  
        X = [6 10 41 3 6 3 2 3 0;
             6 10 41 3 11 6 5 6 0;
             6 11 40 3 16 8 6 7 0;
             6 10 41 3 23 13 8 13 8;
             6 10 41 3 24 17 12 18 4];
              
    case 96  
        X = [7 9 53 3 5 4 2 4 0;
             7 10 52 3 9 6 4 6 0;
             6 12 51 3 16 9 6 10 4;
             6 10 53 3 22 12 9 12 0;
             6 13 50 3 24 18 13 19 0];
         
    case 112  
        X = [6 13 50 3 24 18 13 19 0;
             14 17 50 3 5 4 2 5 0;
             11 21 49 3 9 6 4 8 0;
             11 23 47 3 16 8 6 9 0;
             0 0 0 0 0 0 0 0 0];
         
    case 128  
        X = [12 19 62 3 5 3 2 4 0;
             11 21 61 3 11 6 5 7 0;
             11 22 60 3 16 9 6 10 4;
             11 21 61 3 22 12 9 14 0;
             11 20 62 3 24 17 13 19 8];
         
    case 160  
        X = [11 19 87 3 5 4 2 4 0;
             11 23 83 3 11 6 5 9 0;
             11 24 82 3 16 8 6 11 0;
             11 21 85 3 22 11 9 13 0;
             11 22 84 3 24 18 12 19 0];
         
         
    case 192  
        X = [11 20 110 3 6 4 2 5 0;
             11 22 108 3 10 6 4 9 0;
             11 24 106 3 16 10 6 11 0;
             11 20 110 3 22 13 9 13 8;
             11 21 109 3 24 20 13 24 0];
        
    case 224  
        X = [12 22 131 38 8 6 2 6 4;
             12 26 127 3 12 8 4 11 0;
             11 20 134 3 16 10 7 9 0;
             11 22 132 3 24 16 10 5 0;
             11 24 130 3 24 20 12 20 4];
        
    case 256  
        X = [11 24 154 3 6 5 2 5 0;
             11 24 154 3 12 9 5 10 4;
             11 27 151 3 16 10 7 10 0;
             11 22 156 3 24 14 10 13 8;
             11 26 152 3 24 19 14 18 4];
        
    case 320  
        X = [11 26 200 3 8 5 2 6 4
             11 25 201 3 13 9 5 10 8
             0 0 0 0 0 0 0 0 0;
             11 26 200 3 24 17 9 17 0;
             0 0 0 0 0 0 0 0 0];
        
    case 384  
        X = [11 27 247 3 8 6 2 7 0;
             0 0 0 0 0 0 0 0 0 ;
             11 24 250 3 16 9 7 10 4
             0 0 0 0 0 0 0 0 0;
             12 28 245 3 24 20 14 23 8];
        
    otherwise
end

[PI1] = X(6-Protection,5);  % row, column
[PI2] = X(6-Protection,6);
[PI3] = X(6-Protection,7);
[PI4] = X(6-Protection,8);

L1Range = X(6-Protection,1)*128*(PI1+8)/32;
L2Range = X(6-Protection,2)*128*(PI2+8)/32 + L1Range;
L3Range = X(6-Protection,3)*128*(PI3+8)/32 + L2Range;
L4Range = X(6-Protection,4)*128*(PI4+8)/32 + L3Range;

Padding = X(6-Protection,9);
