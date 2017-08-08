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
% ----------------------------------------------------------------------------------------------
% Frequency de-interleaving  (Standard Pages 157-161)
% Function works for DAB modes I, II, III and IV
% However examplary numbers in comments are given for mode=1
% ----------------------------------------------------------------------------------------------

function [DeintFFTFrame] = FreqDeInterleaving( InterleaverTab, fft_Frame, Nfft )

% Calculate de-interleaving table from the interleaving one having Nfft rows: take only 2 columns: n and k
  DeinterleaverTab = sortrows( InterleaverTab(:, 4:5), 2 );  % sort row 4 (n) and 5 (k) according to column 2 (pi(i))
  DeinterleaverTab = DeinterleaverTab( Nfft/4+1:end,: );     % remove initial 512 nulls/spaces [0,..,511] - not used

% Calculate k-number of the carrier - append as a new column 3: in order 1,2,3,..., 1536
  DeinterleaverTab(:,3) = [ DeinterleaverTab( 1 : Nfft/8*3, 2 )+1 ;...        % [ De(1:768,2)+1;
                            DeinterleaverTab( Nfft/8*3+1 : Nfft/8*6, 2) ] ... %   De(769:1536,2)
                            + Nfft/8*3;                                       %   + 768 ]
% Calculate n-number of the QPSK symbol - append as a new column 4: not in order (interleaved) 1,2,3,..., 1536
  DeinterleaverTab(:,4) = DeinterleaverTab(:,1)+1;                          
% DeinterleaverTab(1:5,3:4), DeinterleaverTab(end-4:end,3:4), pause % Test only

  DeintFFTFrame = zeros(size(fft_Frame));
  for r = 1 : length( DeinterleaverTab(:,4) )                               
      DeintFFTFrame( :, DeinterleaverTab(r,4) ) = fft_Frame(:,r);  % Frequency de-interleaving
  end
  