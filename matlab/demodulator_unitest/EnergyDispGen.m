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
% Energy Dispersal Sequense Generation - ETSI pp. 128-129
% -----------------------------------------------------------------------------------------
function [PRBS] = EnergyDispGen(size)
Vector = [1 1 1 1 1 1 1 1 1];               % initialization word/register
PRBS= zeros(1,size);
CounterPRBS = 1;
for i = 0 : size-1
    NewBit =  xor(Vector(5),Vector(9));     % calculate a new bit
    Vector = [NewBit Vector(1:end-1)];      % store it in a work register
    PRBS(CounterPRBS) = NewBit;             % write the bit into output sequense
    CounterPRBS = CounterPRBS + 1;          % incease the counter
end