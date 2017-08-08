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
% De-puncturing (FIC and MSC)
% -----------------------------------------------------------------------------------------

function [depunctured] = depuncturing( data, PI )

v = PIDet( PI );   % v=values 0/1 (removed/left)         % taking puncturing 1x32-element vector from the PIDet function
NOriginalBits = length(data(1,:))* 32/sum(v);            % bit number after depuncturing (sent out) in each row
depunctured = zeros( length(data(:,1)), NOriginalBits ); % initialization of an output array with 0s

dataSourceCounter = 1;    % index in the input punctured "data" matrix
dataTargetCounter = 1;    % index in the output "depunctured" matrix
vCounter = 1;             % bit counter (changed modulo from 1 to 32)
for h = 1 : length(depunctured(1,:))  % data depuncturing - correct each bit in each row (from each CIF)
    if v(vCounter) == 1                                               % retained value, present in the stream
        depunctured(:,dataTargetCounter) = data(:,dataSourceCounter); % copy value from source to target (input to output)
        dataSourceCounter = dataSourceCounter + 1;                    % increment source counter
    end                                                               %
    dataTargetCounter = dataTargetCounter + 1;                        % increment target counter (left zero in the output matrix) 
    vCounter = vCounter + 1;                                          % increment puncturing counter
    if vCounter == 33                                                 % check puncturing counter overflow
       vCounter = 1;                                                  % re-initialization of puncturing counter
    end                                                               %
end

