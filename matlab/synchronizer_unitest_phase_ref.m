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

fprintf('\n\n*************** PhaseReference generator started ***************\n\n');

%% Start script
cd synchronizer_unitest_all/
global NSampPerGuard;
for mode = 1:4
    switch mode
        case 1
            NSampPerGuard = 504;
        case 2
            NSampPerGuard = 126;
        case 3
            NSampPerGuard = 63;
        case 4
            NSampPerGuard = 252;
    end
    [PhaseRefSymb, sigPhaseRefSymb] = PhaseRefSymbGen( mode, 0 );
    
    fp = fopen( sprintf('../../data/ut/synchronizer_phase_ref_symb_output%d.txt', mode), 'w' );
    for l=1:length(sigPhaseRefSymb)
        fprintf( fp, '%1.18f\n%1.18f\n', real(sigPhaseRefSymb(l)), imag(sigPhaseRefSymb(l)) );
    end
    fclose( fp );
    
end
cd ..
