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
%  Tables 44-47 for frequency interleaving in Modes I-IV (Standard Page 157-161)
% -----------------------------------------------------------------------------------------

function [ FreqIntTab ] = FreqInterleavingTab( Nfft )
% input:  Nfft
% output: Frequency interleaving tables 44, 45, 46 and 47 for Modes I, II, III and IV 

FreqIntTab        = -1*ones( Nfft, 5 );                % init all
FreqIntTab(1,1:2) =  zeros(1,2);                       % first row, columns 1:2                   
FreqIntTab(1,5)   = -Nfft/2-1;                         % first row, column 5

FreqIntTab(:,1) = 0 : Nfft-1;                          % column 1 for variable (i)
counter = 0;                                           % counter for (n)
for m = 2 : Nfft
    FreqIntTab(m,2) =  mod( 13*FreqIntTab(m-1,2)+Nfft/4-1, Nfft);  % column 2 for variable (pi); modulo(Nfft)
    if FreqIntTab(m,2) > Nfft/8-1 && FreqIntTab(m,2) < Nfft/8*7+1 && Nfft/2 ~= FreqIntTab(m,2)
        FreqIntTab(m,3) = FreqIntTab(m,2);             % column 3 for variable (dn)
        FreqIntTab(m,4) = counter;                     % column 4 for variable (n)
        counter = counter+1;                           % increment
    end
    FreqIntTab(m,5) = FreqIntTab(m,3) - Nfft/2;        % column 5 for variable (k)
end
if(0) % Verification
   format short g
   for m=1:Nfft
       m
       FreqIntTab(m,1),FreqIntTab(m,2),FreqIntTab(m,3),FreqIntTab(m,4),FreqIntTab(m,5),
       pause
   end    
end    