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

function [ fft_Array2 ] = SymbolFFT_TZ2( Signal, FrameNr, FrameStartpoints, NSampPerFrame, NSampPerNull, NSymbPerFrame, NSampPerSymb, NSampPerGuard, Nfft, NCarrPerSymb, Offset, coef, fi, PlotOn)

global df df_int df_fract

Data = Signal( FrameStartpoints( FrameNr ) : FrameStartpoints( FrameNr+1 ) -1 );

% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ADC SAMPLING CORRECTION  + DF CORRECTION - START ++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  if(1) % 
      NSampPerFrameADC = FrameStartpoints(FrameNr+1) - FrameStartpoints(FrameNr),  % should be equal NSampPerFrame
      NSampPerFrameDIF =  NSampPerFrameADC - NSampPerFrame,
      if(NSampPerFrameDIF ~= 0)
           step = NSampPerFrameADC/NSampPerFrame,
           if( step ~= 0 )
               s(1:NSampPerFrame,1)=interp1( [0:1:NSampPerFrameADC-1]', Data(1:NSampPerFrameADC), [0:step:NSampPerFrameADC-1]', 'spline' );
               Data(1:NSymbPerFrame*NSampPerSymb,1)=s(1:NSymbPerFrame*NSampPerSymb,1);
           end
      end
  end
  
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ADC SAMPLING CORRECTION + DF CORRECTION - STOP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

  Data = Data(1 : NSymbPerFrame*NSampPerSymb,1) .* exp(-j*2*pi/Nfft*df*(0 : NSymbPerFrame*NSampPerSymb-1)).' ;

  df_fractint  = 0;
  df_fractlast = 0;
  df_fractcorrect  = 0;  

fft_Array = zeros( [NSymbPerFrame, NCarrPerSymb] );       % creating empty Array for FFTs of all OFDM symbols

for nrOFDMSymb = 0 : NSymbPerFrame-1                      % taking only subset of all symbols (in MSC [4-76] = [4-NSymbPerFrame])
      step = nrOFDMSymb * NSampPerSymb;                   % index of the first sample
      
      OFDMSymbol = Data(1+step:NSampPerGuard+Nfft+step);  % data samples corresponding to one OFDM symbol
    
    % +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    % FREQUENCY OFFSET estimation for each OFDM symbol
      if(1)
         % New FRACTIONAL frequency offset: df_intnew = ? 
           Ncut=5;
           z = sum( conj( OFDMSymbol(1+Ncut:NSampPerGuard-Ncut) ) .* OFDMSymbol(1+Nfft+Ncut:NSampPerGuard+Nfft-Ncut) );
           df_fractnew = angle( z )/(2*pi),
           if(1) % Fractional unwrappping
               if( (0 <= df_fractlast) & (df_fractlast < 0.5) & (df_fractnew < 0) & (abs(df_fractnew-df_fractlast)>0.5) )
                    df_fractcorrect = df_fractcorrect + 1,
               end
               if( (-0.5 <= df_fractlast) & (df_fractlast <= 0) & (df_fractnew > 0) & (abs(df_fractnew-df_fractlast)>0.5) )
                    df_fractcorrect = df_fractcorrect - 1,
               end
               df_fractint = df_fractnew + df_fractcorrect; % pause    
               df_fractlast = df_fractnew;
           end

         % New INTEGER frequency offset. df_intnew = ?
           if(1)
              M = 25;
              K = NCarrPerSymb/2;
              S = fft( OFDMSymbol(NSampPerGuard/2:NSampPerGuard/2+Nfft-1) );
            % S = circshift(PhaseRefSymb,-5); % for test only
              S = [ S(end-K-M+1:end); S(1:K+M+1) ];
              S = real(S.*conj(S));
              Ssum(1) = sum( S(1:2*K+1) );
              for k = 2:2*M+1
                  Ssum(k) = Ssum(k-1) - S(k-1) + S(k+2*K); 
              end    
            % if(PlotOn) subplot(111); plot(-M:M,Ssum,'ro'); grid; title('df_inf'); pause, end
              [ dummy indx ] = max(Ssum);
              df_intnew = indx - (M+1),
           else
              df_intnew = df_int;
           end    

         % THE BEST RESULT WHEN NO HISTORY IS APPLIED FROM NOISY DATA !!!!  
         % +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
         % Correction of df
           df_new = df + df_fractint;
         % df_new = df + df_fractnew + df_intnew;
         % df_new = df + df_fractint + df_intnew;
           if( prod(size(df))==0 ) df = df_new;
           else                    df = coef*df + (1-coef)*df_new;
           end
           
           OFDMSymbol = OFDMSymbol(1 : NSampPerGuard+Nfft,1 ) .* exp(-j*2*pi/Nfft*(df_fractint)*(i : i+NSampPerGuard+Nfft-1)).';   % fine (fractional) frequency correction
         % OFDMSymbol = OFDMSymbol(1 : NSampPerGuard+Nfft ) .* exp(-j*2*pi/Nfft*(df_fractnew+df_intnew)*(i : i+NSampPerGuard+Nfft-1)).';   % fine (fractional) frequency correction
         % OFDMSymbol = OFDMSymbol(1 : NSampPerGuard+Nfft ) .* exp(-j*2*pi/Nfft*(df_fractint+df_intnew)*(i : i+NSampPerGuard+Nfft-1)).';   % fine (fractional) frequency correction
         
         OFDMSymbol = OFDMSymbol( NSampPerGuard-Offset : NSampPerGuard + Nfft -1 - Offset);
         
      else
          
         OFDMSymbol = OFDMSymbol( NSampPerGuard-Offset : NSampPerGuard + Nfft -1 - Offset);
         
      end     
    % +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
      Y = fft( OFDMSymbol );      % FFT of them
    % subplot(111); stem(abs(Y)), pause
    
      Y = fftshift( Y ); % reordering: negative frequencies to the left
      fft_Array( nrOFDMSymb+1, 1 : NCarrPerSymb/2 )              = Y( Nfft/2-NCarrPerSymb/2+1 : Nfft/2 );   % setting FFT array
      fft_Array( nrOFDMSymb+1, NCarrPerSymb/2+1 : NCarrPerSymb ) = Y( Nfft/2+2 : Nfft/2+NCarrPerSymb/2+1 ); % take used carriers only
end

fft_Array2 = fft_Array( 2:NSymbPerFrame-1, : ) .* conj( fft_Array( 1:NSymbPerFrame-2, :) );  % phase difference calculation
if( fi ~= 0 ) fft_Array2 = fft_Array2 * exp( j*fi); end

if( PlotOn )
    if(1)  
      figure('Name','Phase difference for all carriers');
      plot(angle(fft_Array2(:,:)),'*');                   % phase difference for all carriers
      figure('Name','Eye-diagrams for all carriers');
      plot(angle(fft_Array2(:,:)));                       % eye diagrams for all carriers
      xlim([1,15])
    end
    figure('Name','Constellation points');
    subplot(221) 
    plot(fft_Array2(:,100),'*'); title('Carrier #1');   % constellation points "*" (polarplot) for one carrier
    axis equal;
    grid;
    subplot(222) 
    plot(fft_Array2(:,200),'*'); title('Carrier #2');   % constellation points "*" (polarplot) for one carrier
    axis equal;
    grid;
    subplot(223) 
    plot(fft_Array2(:,300),'*'); title('Carrier #3');   % constellation points "*" (polarplot) for one carrier
    axis equal;
    grid;
    subplot(224) 
    plot(fft_Array2(:,400),'*'); title('Carrier #4');   % constellation points "*" (polarplot) for one carrier
    axis equal;
    grid;
    disp(' '); disp(' LOOK AT FIGURES! PRESS ANY KEY ...'); pause
end    
