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
% FFT of all OFDM symbols in one TX DAB frame, D-QPSK demodulation
% On output OFDM symbols are in consecutive rows.
% -----------------------------------------------------------------------------------------

function [fft_Array2] = SymbolFFTStreamTZ3e( Data, OFDMOffset, NSymbPerFIC, coef, fi, PlotOn )

global NSymbPerFrame NSampPerFrame NSampPerSymb NSampPerNull NCarrPerSymb Nfft NSampPerGuard PhaseRefSymb sigPhaseRefSymb fs df_fract df_int df FrameStartpoints FrameLengths

df_old = df;

df_fractint  = 0;
df_fractlast = 0;
df_fractcorrect  = 0;

hist_fractnew     = zeros(1,NSymbPerFrame);
hist_fractint     = zeros(1,NSymbPerFrame);
hist_fractcorrect = zeros(1,NSymbPerFrame);
hist_int          = zeros(1,NSymbPerFrame);
hist_df           = zeros(1,NSymbPerFrame);

n1st = 1;                                                      % first OFDM frame number: was 4, possible 1,2,3,4

fft_Array = zeros( [NSymbPerFrame-(n1st-1), NCarrPerSymb] );   % creating empty Array for FFTs of all OFDM symbols minus (n1st-1) initial ones
                                                               % first (n1st-1) OFDM symbols are excluded !!!

% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% THE BEST RESULT WHEN NOISY DATA ARE NOT USED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

if(0) % Frequency offset estimation for each OFDM symbol, mean and correction

for nrOFDMSymb = n1st : NSymbPerFrame                   % taking only subset of all symbols (in MSC [4-76] = [4-NSymbPerFrame])
    
  % disp('OFDM Frame');
    i = (nrOFDMSymb-1) * NSampPerSymb;                  % index of the first sample
    OFDMSymbol = Data(1+i:NSampPerGuard+Nfft+i);        % data samples corresponding to one OFDM symbol
    
          
          % ++++++++++++++++++++++++++++++++++++++
          % Change of fractional offset df_fract=?
            Ncut=25;
            z = sum( conj( Data(i+Ncut+0:i+NSampPerGuard-Ncut-1) ) .* Data(i+Nfft+Ncut+0:i+NSampPerGuard+Nfft-Ncut-1) );
            df_fractnew = angle( z )/(2*pi),
         
          % Fractional unwrappping
            if( (0 <= df_fractlast) & (df_fractlast < 0.5) & (df_fractnew < 0) & (abs(df_fractnew-df_fractlast)>0.5) )
                 df_fractcorrect = df_fractcorrect + 1,
            end
            if( (-0.5 <= df_fractlast) & (df_fractlast <= 0) & (df_fractnew > 0) & (abs(df_fractnew-df_fractlast)>0.5) )
                 df_fractcorrect = df_fractcorrect - 1,
            end
            df_fractintnew  = df_fractnew + df_fractcorrect, % pause
            df_fractlast = df_fractnew;

         % +++++++++++++++++++++++++++++++++++ 
         % Change of integer offset df_int = ?
           M = 5;
           K = NCarrPerSymb/2;
         % S = fft( OFDMSymbol(NSampPerGuard/2+1:NSampPerGuard/2+Nfft) .* exp(-j*2*pi/Nfft*df_fractnew*(NSampPerGuard/2:NSampPerGuard/2+Nfft-1)).'); % ERROR: df_fractnew not compensated
           S = fft( OFDMSymbol(NSampPerGuard/2+1:NSampPerGuard/2+Nfft) .* exp(-j*2*pi/Nfft*df_fractintnew*(NSampPerGuard/2:NSampPerGuard/2+Nfft-1)).'); % ERROR: df_fractnew not compensated
         % S = circshift(PhaseRefSymb,-5); % for test only
           S = [ S(end-K-M+1:end); S(1:K+M+1) ];
           S = real(S.*conj(S));
           Ssum(1) = sum( S(1:2*K+1) );
           for k = 2:2*M+1
               Ssum(k) = Ssum(k-1) - S(k-1) + S(k+2*K); 
           end    
         % if(PlotOn) subplot(111); plot(-M:M,Ssum,'ro'); grid; title('df_int'); pause, end
           [ dummy indx ] = max(Ssum);
           df_intnew = indx - (M+1),
           
           if(0) % Correction of each OFDM frame separately
              % THE BEST RESULT WHEN NO HISTORY IS APPLIED FROM NOISY DATA !!!!  
              % +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
              % Correction of df
                df_new = df + df_fractintnew;
              % df_new = df + df_fractnew + df_intnew;
              % df_new = df + df_fractintnew + df_intnew;
                if( prod(size(df))==0 ) df = df_new;
                else                    df = coef*df + (1-coef)*df_new;
                end

                Data(1+i:NSampPerGuard+Nfft+i) = OFDMSymbol .* exp(-j*2*pi/Nfft*(df_fractintnew)*(i : i+NSampPerGuard+Nfft-1)).';   % fine (fractional) frequency correction
              % Data(1+i:NSampPerGuard+Nfft+i) = OFDMSymbol .* exp(-j*2*pi/Nfft*(df_fractnew+df_intnew)*(i : i+NSampPerGuard+Nfft-1)).';   % fine (fractional) frequency correction
              % Data(1+i:NSampPerGuard+Nfft+i) = OFDMSymbol .* exp(-j*2*pi/Nfft*(df_fractintnew+df_intnew)*(i : i+NSampPerGuard+Nfft-1)).';   % fine (fractional) frequency correction
           
           else % for correction by mean diff value
           
                hist_fractnew( nrOFDMSymb ) = df_fractnew;
                hist_fractint( nrOFDMSymb ) = df_fractintnew;
                hist_int( nrOFDMSymb ) = df_intnew;
                hist_fractcorrect( nrOFDMSymb ) = df_fractcorrect;
                hist_df( nrOFDMSymb ) = df_fractintnew + df_intnew;
           end     
           
    end % of if (frequency offset estimation)

if(1) % do correction by mean diff value
    df_diff = mean( hist_int + hist_fractint ),  % mean
    Data(1 : NSymbPerFrame*NSampPerSymb,1) = Data(1 : NSymbPerFrame*NSampPerSymb,1) .* exp(-j*2*pi/Nfft*df_diff*(0 : NSymbPerFrame*NSampPerSymb-1)).' ;     % correction
    if(1)
       if( prod(size(df))==0 ) df = df + df_diff;
       else                    df = coef*df + (1-coef)*(df+df_diff);
       end
    end
end

%disp('df IN');  df_old,
%disp('df OUT'); df,

if(0)
    figure
    n = 1:NSymbPerFrame;
    subplot(511); stem(n,hist_fractnew); grid; title('FractNew');
    subplot(512); stem(n,hist_fractint); grid; title('FractInt');
    subplot(513); stem(n,hist_fractcorrect); grid; title('Correct');
    subplot(514); stem(n,hist_int); grid; title('Int');
    subplot(515); plot(n,hist_df); grid; title('SUM');
end

end
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


for nrOFDMSymb = n1st : NSymbPerFrame                   % taking only subset of all symbols (in MSC [4-76] = [4-NSymbPerFrame])
    
  % disp('OFDM Frame');
    i = (nrOFDMSymb-1) * NSampPerSymb;                  % index of the first sample
    OFDMSymbol = Data(1+i:NSampPerGuard+Nfft+i);        % data samples corresponding to one OFDM symbol
    OFDMSymbol = OFDMSymbol(NSampPerGuard+1-OFDMOffset : NSampPerGuard+Nfft-OFDMOffset );
  
  % OFDMSymbol = OFDMSymbol - mean(OFDMSymbol);
    
    Y = fft( OFDMSymbol );      % FFT of them
  % subplot(111); stem(abs(Y)), pause
        
    Y = fftshift( Y );          % reordering: negative frequencies to the left
    fft_Array( nrOFDMSymb-(n1st-1), 1 : NCarrPerSymb/2 )              = Y( Nfft/2-NCarrPerSymb/2+1 : Nfft/2 );   % setting FFT array
    fft_Array( nrOFDMSymb-(n1st-1), NCarrPerSymb/2+1 : NCarrPerSymb ) = Y( Nfft/2+2 : Nfft/2+NCarrPerSymb/2+1 ); % take used carriers only

end % of for ( series of FFT )

fft_Array2 = fft_Array( 2:NSymbPerFrame-(n1st-1), : ) .* conj( fft_Array( 1:NSymbPerFrame-(n1st-1)-1, :) );  % phase difference calculation

% fft_Array2 = fft_Array2( (NSymbPerFIC+1)-(n1st-1) : end, : );  % in RT version FIC and MSC are taken into account

if( fi ~= 0 ) fft_Array2 = fft_Array2 * exp( j*fi); end

% df_fract=[]; df_int=[];
    
if( PlotOn )

    figure('Name','Phase difference for all carriers');
    plot(angle(fft_Array2(:,:)),'*');                   % phase difference for all carriers
    pause
    
    figure('Name','Eye-diagrams for all carriers');
    plot(angle(fft_Array2(:,:)));                       % eye diagrams for all carriers
    xlim([1,10])
    pause
% end
% if(1)
    figure('Name','Constellation points for 4 carriers: 1/4, 2/4, 3/4, 4/4 * NCarrPerSymb/2');
    subplot(221) 
    plot(fft_Array2(:,round(1/4*NCarrPerSymb/2)),'*'); title('Carrier #1');   % constellation points "*" (polarplot) for one carrier
    axis equal;
    grid;
    subplot(222) 
    plot(fft_Array2(:,round(2/4*NCarrPerSymb/2)),'*'); title('Carrier #2');   % constellation points "*" (polarplot) for one carrier
    axis equal;
    grid;
    subplot(223) 
    plot(fft_Array2(:,round(3/4*NCarrPerSymb/2)),'*'); title('Carrier #3');   % constellation points "*" (polarplot) for one carrier
    axis equal;
    grid;
    subplot(224) 
    plot(fft_Array2(:,round(4/4*NCarrPerSymb/2)),'*'); title('Carrier #4');   % constellation points "*" (polarplot) for one carrier
    axis equal;
    grid;
    pause
    
    if(0)
    for k=750:800  % max err dla k=788 (dla 229.067MHz) oraz k=788 (dla 229.077MHz)
        k
        subplot(111) 
        plot(fft_Array2(:,round(k)),'*'); title('Carrier #4');   % constellation points "*" (polarplot) for one carrier
        axis equal;
        grid;
        pause
    end
    end
    
end