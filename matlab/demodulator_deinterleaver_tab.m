function demodulator_deinterleaver_tab

    clear;
    close all;
    
    fprintf('\n*************** deinterleaver_tab generator started ***************\n\n');
    
    nfft_values = [2048 512 256 1024];
    mode_names = {'mode_1' 'mode_2' 'mode_3' 'mode_4'};
    
    for k = 1: length(nfft_values)
    
        Nfft = nfft_values(k);
        deinterleaver_tab = BuildFreqDeInterleaverTab(FreqInterleavingTab(Nfft), Nfft);

        fp = fopen(['../data/ut/demodulator_deinterleaver_tab_' mode_names{k} '.txt'], 'w' );
        for i = 1: size(deinterleaver_tab, 1)
            
            fprintf(fp, '%1.18f\n', deinterleaver_tab(i, 4));
        end
        fclose(fp);
    end
end

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
% Building frequency de-interleaving table (Standard Pages 157-161)
% Function works for DAB modes I, II, III and IV
% However examplary numbers in comments are given for mode=1
% ----------------------------------------------------------------------------------------------

function [ DeinterleaverTab ] = BuildFreqDeInterleaverTab( InterleaverTab, Nfft )

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
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Masterthesis
% Zurcher Hochschule fur Angewandte Wissenschaften
% Zentrum fur Signalverarbeitung und Nachrichtentechnik
% (c) Michael Hoin
% 12.4.2011 ZSN
% info.zsn@zhaw.ch
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
end