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
%
% -----------------------------------------------------------------------------------------
% Frame start detection
% -----------------------------------------------------------------------------------------

function [ FrameStartpoints ] = DetFrameStart( Signal, PlotOn )

TOC_Old = toc;

forgetfactor = 3;                            % forgeting factor for Signal Moving Average Filter

ABS_RunSum = zeros( 1, length(Signal) );
FrameStartpoints = zeros(1,1000);
Delay = 2^6;
Skip = 2^13;

% Running summation
  ABS_Signal = abs(Signal);
  for i=1:length(Signal)-1
      ABS_RunSum(i+1) = ABS_RunSum(i) - ABS_RunSum(i)/(2^forgetfactor) + ABS_Signal(i);  % Moving Average Filter
  end
  if PlotOn == 1                                                          % Plot RunSumABS and signal amplitude
     figure('Name','ABS_Signal and ABS_RunSum');
     plot( 1 : length(Signal), ABS_Signal, 1+Delay : length(Signal)+Delay, ABS_RunSum);
     legend('ABS_Signal','ABS_RunSum')
     hold on;
  end
  fprintf('ABS_RunSum calculation %3i sec \n',round(toc-TOC_Old));
  TOC_Old = toc;

% Comparison: running summation vs. abs(signal)
  index = 0;
  for i = 1+Delay : 1 : length(Signal)-1
     if (index < i)                                                       % Detection condition
         if (ABS_RunSum(i-Delay) < ABS_Signal(i))                         % Framestart search
             if PlotOn == 1                                               %
                plot(i,ABS_Signal(i),'*','color','r');                    % Detection markers
             end                                                          %
             FrameStartpoints(1) = i;                                     % Store position
             FrameStartpoints = circshift( FrameStartpoints, [0 -1] );    %
             index = i+Skip;                                              % Jump forward (in oder to avoid double detection)
         end                                                              %
     end
  end

fprintf('Framestart calculation %3i sec \n', round(toc-TOC_Old));

y= find( FrameStartpoints > 1000 );
FrameStartpoints = FrameStartpoints( min(y) : length(FrameStartpoints) ); % Startpoint selection

if PlotOn == 1
    hold off;
end
