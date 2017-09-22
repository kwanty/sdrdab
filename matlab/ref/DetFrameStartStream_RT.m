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
% Frame start detection
% First OFDM symbol in each TX frame = NULL signal (equal zero) + TTI (optional, different from zero)
% This signal should have very small.  
% -----------------------------------------------------------------------------------------

function [ SignalOut, OldData ] = DetFrameStartStream_RT( Signal, ReadSize,...
                                  CorrectTimeSynchro, CorrectDeltaFreq, CorrectADC,...
                                  coef, PlotOn )

global NSymbPerFrame NSampPerFrame NSampPerSymb NSampPerNull NCarrPerSymb Nfft NSampPerGuard PhaseRefSymb sigPhaseRefSymb fs df_fract df_int df
global DABFrameFoundNr FrameStartpoints FrameLengths NSampPerFrameADC

persistent isum

if( prod(size(DABFrameFoundNr))==0 ) isum = 0; DABFrameFoundNr = 0; end

ABS_Signal = abs(Signal);

%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
if( length(Signal) > 2*NSampPerFrame )  % continue only if Signal is long enough
%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
  disp(' NEW PhaseRef ###################################################################################');

  if( DABFrameFoundNr == 0 )   NSampPerSynchro = NSampPerFrame + NSampPerNull;
  else                         NSampPerSynchro = 2*NSampPerNull;
  end

  nullFFT = zeros(1, Nfft);
  load('matrixForDecodeNULLv2.mat');
  SUM = zeros(1,NSampPerSynchro);
  SUM(1) = sum( ABS_Signal(1:NSampPerNull) );
  for n = 2 : NSampPerSynchro; 
      SUM(n) = SUM(n-1) - ABS_Signal(n-1) + ABS_Signal(NSampPerNull+n-1);
  end
  [dummy imin] = min(SUM);
  i = imin+NSampPerNull;

       nullFFT = fft(Signal(i-Nfft-Nfft/8:i-1-Nfft/8));
       if( PlotOn ) % Show detected first sample of the Phase Reference OFDM Symbol 
           i,
           figure('Name','REAL Signal');
           plot(i,0,'ro',1:length(ABS_Signal),real(Signal),'b-'); title('REAL Signal'); pause
           
           figure('Name','REAL & IMAG Signal');
           subplot(211); plot(i,0,'ro',1:length(ABS_Signal),real(Signal),'b-'); title('REAL Signal');
           subplot(212); plot(i,0,'ro',1:length(ABS_Signal),imag(Signal),'b-'); title('IMAG Signal'); pause
           figure('Name','Spectrum ABS of NULL');
           plot(abs(fft(Signal(i-Nfft-Nfft/8:i-1-Nfft/8)))); title('SPECTRUM ABS of NULL signal'); pause
           figure('Name','SYNCHRO ABS via NULL & RunSum');
           plot(i,0,'ro',1:length(ABS_Signal),ABS_Signal,'b-'); title('SYNCHRO ABS via NULL signal and RunSum'); pause
       
           offs = 0; % for test only

         % TIME: compare first OFDM symbol with Phase Reference Symbol in TIME DOMAIN
           sreal = Signal(i+offs : i+offs+NSampPerSymb-1);  % Symbol with Cyclic Prefix
           sreal = sreal/max(abs(sreal));
           sref = ifft( PhaseRefSymb ); sref = [ sref(end-NSampPerGuard+1:end); sref ]; % Ref with Cyclic Prefix
           sref = sref/max(abs(sref));
           figure('Name','BEFORE DF - Signals');
           subplot(211); plot( 1:NSampPerSymb, real(sreal), 'bx', 1:NSampPerSymb, real(sref),'ro');
           title('BEFORE DF - REAL(time): PhaseRef Symbol (o) OFDM Symbol (x)');
           subplot(212); plot( 1:NSampPerSymb, imag(sreal), 'bx', 1:NSampPerSymb, imag(sref),'ro');
           title('BEFORE DF - IMAG(time): PhaseRef Symbol (o) OFDM Symbol (x)');
           ERR_TIME_PHASE_REF = max(abs(sreal-sref)), pause
          
         % FREQ: compare OFDM symbol and Phase Reference symbol in FREQUENCY DOMIAN
           sreal = Signal(i+offs+NSampPerGuard : i+offs+NSampPerGuard+Nfft-1); Sreal=fft(sreal); Sreal=Sreal/max(abs(Sreal));
           figure('Name','BEFORE DF - Spectra');;
           subplot(211); plot(1:Nfft,real(PhaseRefSymb),'ro',1:Nfft,real(Sreal),'bx');
           title('BEFORE DF - REAL(freq): PhaseRef Symbol (o) OFDM Symbol (x)');
           subplot(212); plot(1:Nfft,imag(PhaseRefSymb),'ro',1:Nfft,imag(Sreal),'bx');
           title('BEFORE DF - IMAG(freq): PhaseRef Symbol (o) OFDM Symbol (x)');
           ERR_FREQ_PHASE_REF = max(abs(PhaseRefSymb-Sreal)), pause
       end

  % df_fract = ? (fractional): FIRST fractional frequency offset estimation ##################################

  % HELP:
  % z = sum( conj( Signal(PrefixOFDM) ) .* Signal(LastSamplesOFDM) );
  % df = angle(z)/(2*pi);
  
    Ncut = 25;  % TZ!!! assumed max offset error +/-
    z =  sum( conj(Signal(i+Ncut : i+NSampPerGuard-Ncut-1)) .* Signal(i+Nfft+Ncut:i+Nfft+NSampPerGuard-Ncut-1) );
    df_fractnew = angle(z)/(2*pi),
  
  % First df_fract compensation   
    s(1:NSampPerGuard+Nfft,1) = Signal(i:i+NSampPerGuard+Nfft-1) .* exp(-j*2*pi/Nfft*df_fractnew*(i:i+NSampPerGuard+Nfft-1)).';   % df fractional\

         if( PlotOn )
             figure('Name','AFTER DF FRACT - first compensation');
             subplot(211); plot( 1:NSampPerSymb, real(s)/max(abs(s)), 'bx', 1:NSampPerSymb, real(sigPhaseRefSymb)/max(abs(sigPhaseRefSymb)),'ro');
             title('AFTER first DF FRACT - REAL(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             subplot(212); plot( 1:NSampPerSymb, imag(s)/max(abs(s)), 'bx', 1:NSampPerSymb, imag(sigPhaseRefSymb/max(abs(sigPhaseRefSymb))),'ro');
             title('AFTER first DF FRACT - IMAG(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             ERR_TIME_PHASE_REF = max(abs(s-sigPhaseRefSymb)), pause
          end
     
  % df_int = ? ###############################################################################################
    
   % Cyclic Correlation of Signal Spectrum with the PhaseRef Spectrum --> find maximum
   % Multiplication in time domain = cyclic convolution (correlation) in frequency domain
    
     M = 20;  % M = 10? 20?
   % XC = fft( ifft( circshift( PhaseRefSymb, -7)).*conj( ifft( PhaseRefSymb ) ) );   % for test only
     XC = fft( s(NSampPerGuard+1:NSampPerGuard+Nfft).*conj( sigPhaseRefSymb(NSampPerGuard+1:NSampPerGuard+Nfft ) ) );
     XC = [ XC(end-M+1:end); XC(1:1+M) ];
     [ val, indx ] = max( XC ); 
     df_intnew = indx - (M+1), % pause
         
            if( PlotOn )
               m = -M:+M;
               figure('Name','Integer frequency offset - estimation #1');  
               subplot(311); plot( m, real(XC) ); title('Re(Xcorr) - df integer shift');
               subplot(312); plot( m, imag(XC) ); title('Im(Xcorr) - df integer shift');
               subplot(313); stem( m,  abs(XC) ); title('Abs(Xcorr) - df integer shift'); grid; pause
            end

            if(0) % Test only
                 if(df_intnew>0) work = [ work((df_intnew+1):end);  work(1:df_intnew) ]; end
                 if(df_intnew<0) work = [ work(end+df_intnew+1:end); work(1:end+df_intnew) ]; end
                 df_int_error = max( abs( work-PhaseRefSymb ) ),
                 subplot(111); plot(1:Nfft,abs(PhaseRefSymb),'ro',1:Nfft,abs(work),'bx'); % pause
            end

% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% IMPROVING - START +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    
  % Optional better time synchronization and improved df calculation
  % Result: better localization GuardInterval (CP)
    
    if( CorrectTimeSynchro ) 

         % +++++++++++++++++++++++++++++++  
         % Improving time synchronization
         % +++++++++++++++++++++++++++++++  
       
         % UPDATE of dfx estimate
           if( prod(size(df))==0 ) dfx = df_intnew + df_fractnew;
           else                    dfx = coef*df + (1-coef)*(df_intnew+df_fractnew);
           end
         % dfx = df_intnew + df_fractnew;
         
         % DOING df correction using dfx estimate
           s(1:NSampPerGuard+Nfft,1) = Signal(i:i+NSampPerGuard+Nfft-1) .* exp(-j*2*pi/Nfft*dfx*(i:i+NSampPerGuard+Nfft-1)).';

           if( PlotOn )
             figure('Name','AFTER DF INT - first compensation')
             subplot(211); plot( 1:NSampPerSymb, real(s)/max(abs(s)), 'bx', 1:NSampPerSymb, real(sigPhaseRefSymb)/max(abs(sigPhaseRefSymb)),'ro');
             title('AFTER first DF INT - REAL(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             subplot(212); plot( 1:NSampPerSymb, imag(s)/max(abs(s)), 'bx', 1:NSampPerSymb, imag(sigPhaseRefSymb/max(abs(sigPhaseRefSymb))),'ro');
             title('AFTER first DF INT - IMAG(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             ERR_TIME_PHASE_REF = max(abs(s-sigPhaseRefSymb)), pause
           end

         % TIME OFFSET CALCULATION IN FREQ DOMAIN using PhaseRefSymb - correlation in freq domain  
         % s = circshift(sref,-5); % for test only
           S = fft( s(NSampPerGuard+1 : NSampPerGuard+Nfft) ); S=S/max(abs(S));
           work = ifft( S .* conj(PhaseRefSymb) );
           if(PlotOn)
               figure('Name','TIME OFFSET #1 in FREQ via XCORR with PhaseRef');
               stem(-Nfft/2+1:Nfft/2,abs(fftshift(work))); grid; title('TIME OFFSET in FREQ #1 by XCORR using Phase Ref Symbol');  pause,
           end

           M=7; work=[ work(end-M+1:end); work(1:M+1) ]; % TZ !!! Assumed max +/- 7
           [ dummy indx ] = max(abs(work(1:2*M+1))); 
           offs1a = indx-(M+1),
           if(PlotOn)
               figure('Name','TIME OFFSET #2 in FREQ via XCORR with PhaseRef');
               stem(-M:M,abs(work)); grid; title('TIME OFFSET in FREQ #2 by XCORR using Phase Ref Symbol');  pause,
           end
       
         % TIME OFFSET CALCULATION IN TIME DOMAIN using PhaseRefSymb   
           M = 7; XC=[]; % TZ !!! Assumed max +/- 7
           XC(M+1) = sum( s .* conj(sigPhaseRefSymb) )/NSampPerSymb;
           for m = 1 : M
               XC( M+1+m ) = sum( s(1+m:NSampPerSymb) .* conj(sigPhaseRefSymb(1:NSampPerSymb-m)) )/(NSampPerSymb-m);
           end
           for m = 1 : M
               XC( M+1-m ) = sum( s(1:NSampPerSymb-m) .* conj(sigPhaseRefSymb(1+m:NSampPerSymb)) )/(NSampPerSymb-m);
           end
          [ dummy offs1b ] = max(abs(XC));
          offs1b = offs1b - (M+1),
          if(PlotOn)
              figure('Name','TIME OFFSET in TIME via XCORR with PhaseRef');
              stem( -M : M, abs(XC) ); title('TIME OFFSET in TIME by XCORR using Phase Ref Symbol'); pause,
          end
      
        % TAKING DECISION ABOUT THE TIME OFFSET
          offs1 = offs1b; % calculated correction is taken into account or not (=0)
          if(PlotOn)
              figure('Name','SYNCHRO #1 via NULL zone & PhaseRef');
              plot(i,0,'ro',i+offs1,0,'go',1:length(ABS_Signal),ABS_Signal,'b-'); title('Synchro via NULL zone & PhaseRef: RED-to-GEEN=corrected'); pause,
          end

        % TEST: Check synchronization using the Cyclic Prefix test
          if(0)
             for k = 1 : 5 % for first 5 OFDM symbols only
                 k,
                 jump = (k-1)*NSampPerSymb;
                 sreal = Signal(i+offs1+jump : i+offs1+jump+NSampPerGuard+Nfft-1);
                 figure;
                 subplot(211); plot( abs(sreal) ), title('Samples of an OFDM symbol');
                 subplot(212); plot( abs(sreal(1:NSampPerGuard)) - abs(sreal(end-NSampPerGuard+1:1:end)) ), title('Cyclic Prefix check');
                 ERROR_CYCLIC_PREFIX = max(abs(sreal(1:NSampPerGuard))-abs(sreal(end-NSampPerGuard+1:1:end))), pause
             end
          end
          
     else % without improving of time synchronization 

          offs1 = 0;
        
    end  % of if (improving time synchronization)

    if( CorrectDeltaFreq )
          
         % +++++++++++++++++++++++++++++++++++++++++++++++  
         % Improving df - take more GI (CP) signal samples 
         % +++++++++++++++++++++++++++++++++++++++++++++++  

        % Cutting adjusted proper fragment of PhaseRef signal
          s(1:NSampPerGuard+Nfft,1) = Signal(i+offs1:i+offs1+NSampPerGuard+Nfft-1);
          
        % df_fract = ? - repeat calculation of df_fract taking more signal samples 
          Ncut = 5; % TZ !!! Assumed max +/- 5
          z =  sum( conj(s(1+Ncut : NSampPerGuard-Ncut)) .* s(1+Nfft+Ncut:NSampPerGuard+Nfft-Ncut) );
          df_fractnew = angle(z)/(2*pi),
          
        % Second df_fract compensation
          s = s .* exp(-j*2*pi/Nfft*df_fractnew*(i+offs1:i+offs1+NSampPerGuard+Nfft-1)).';   % df fractional      
       
        % df_int = ? - repeat calculation of df_int taking more signal samples
          XC = fft( s(NSampPerGuard+1:NSampPerGuard+Nfft).*conj( sigPhaseRefSymb(NSampPerGuard+1:NSampPerGuard+Nfft ) ) );
          XC = [ XC(end-M+1:end); XC(1:1+M) ];
         [ val, indx ] = max( XC ); 
          df_intnew = indx - (M+1), % pause
         
            if( PlotOn )
               m = -M:+M;
               figure('Name','Integer frequency offset - estimation #2');  
               subplot(311); plot( m, real(XC) ); title('Re(Xcorr) - df integer shift');
               subplot(312); plot( m, imag(XC) ); title('Im(Xcorr) - df integer shift');
               subplot(313); stem( m,  abs(XC) ); title('Abs(Xcorr) - df integer shift'); grid; pause
            end
   
       end   % of if (improving df)
    
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% IMPROVING - STOP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

% Calculate frame startpoints and lengths
  overlap = 100;
  DABFrameFoundNr = DABFrameFoundNr + 1;
  isum = isum + (i+offs1);
  FrameStartpoints( DABFrameFoundNr ) = isum;
  isum = isum + NSymbPerFrame*NSampPerSymb - overlap;
  if(DABFrameFoundNr > 1) FrameLengths( DABFrameFoundNr ) = FrameStartpoints( DABFrameFoundNr ) -  FrameStartpoints( DABFrameFoundNr-1 ), end

  if(0)  
     % VERY IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  
       df = df_intnew + df_fractnew; % THE BEST RESULT WHEN PhaseRef IS USED AND NO HISTORY IS APPLIED !!!!
       df_int = df_intnew;
       df_fract = df_fractnew;
     % VERY IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  
  else
     % Correct df  
       if( prod(size(df))==0 ) df = df_intnew + df_fractnew,
       else                    df = coef*df + (1-coef)*(df_intnew+df_fractnew),
       end
       df_int = round(df),
       df_fract = df - df_int, % pause
  end
  
         if( PlotOn )
           % DOING df correction of the PHASE REFERENCE signal
             s(1:NSampPerGuard+Nfft,1) = Signal(n:n+NSampPerGuard+Nfft-1) .* exp(-j*2*pi/Nfft*df*(n:n+NSampPerGuard+Nfft-1)).';  
             
             figure('Name','OVERALL frequency offset compensation for Phase Reference')
             subplot(211); plot( 1:NSampPerSymb, real(s)/max(abs(s)), 'bx', 1:NSampPerSymb, real(sigPhaseRefSymb  )/max(abs(sigPhaseRefSymb  )),'ro');
             title('AFTER DF - REAL(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             subplot(212); plot( 1:NSampPerSymb, imag(s)/max(abs(s)), 'bx', 1:NSampPerSymb, imag(sigPhaseRefSymb  /max(abs(sigPhaseRefSymb  ))),'ro');
             title('AFTER DF - IMAG(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             ERR_TIME_PHASE_REF = max(abs(s-sigPhaseRefSymb  )), pause
         end
  
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ADC CORRECTION - START ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

% ADC sampling rate correction

  if( CorrectADC ) % set 0 for "clear" ETI-generated TX signals since their DAB frame is +1 sample longer!  

       if( DABFrameFoundNr > 1 )
         % FrameStartpoints(DABFrameFoundNr), FrameStartpoints(DABFrameFoundNr-1),
           NSampPerFrameADCnew = FrameStartpoints(DABFrameFoundNr) - FrameStartpoints(DABFrameFoundNr-1);  % should be equal NSampPerFrame
           if( prod(size(NSampPerFrameADC))==0 ) NSampPerFrameADC = NSampPerFrameADCnew;
           else                                  NSampPerFrameADC = round(coef*NSampPerFrameADC + (1-coef)*NSampPerFrameADCnew);
           end
           NSampPerFrameDIF =  NSampPerFrameADC - NSampPerFrame;
           
           if(NSampPerFrameDIF ~= 0)  % for ETI generated "clear" TX signals frame is +1 sample longer! WHY! 
              step = NSampPerFrameADC/NSampPerFrame;
              if( step ~= 1 )
                  s(1:NSampPerFrame,1)=interp1( [0:1:NSampPerFrameADC-1]', Signal(i+offs1:i+offs1+NSampPerFrameADC-1), [0:step:NSampPerFrameADC-1]', 'spline' );
                  Signal(i+offs1 : i+offs1+NSymbPerFrame*NSampPerSymb-1,1)=s(1:NSymbPerFrame*NSampPerSymb,1);
              end
           end
       end % of DABFrameFoundNr > 0? 
  end     

% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ADC CORRECTION - STOP +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++       

% FINAL DF CORRECTION OF THE WHOLE LONG DAB FRAME

    SignalOut = Signal(i+offs1 : i+offs1+NSymbPerFrame*NSampPerSymb-1) .* exp(-j*2*pi/Nfft*df*(0 : NSymbPerFrame*NSampPerSymb-1)).' ;    % detected DAB frame
    OldData = Signal(i+offs1+NSymbPerFrame*NSampPerSymb - overlap: end);  % data to be analyzed later

          if( PlotOn )
             s = SignalOut(1:NSampPerGuard+Nfft);
             figure('Name','OVERALL freq & ADC compensation for Phase Reference')
             subplot(211); plot( 1:NSampPerSymb, real(s)/max(abs(s)), 'bx', 1:NSampPerSymb, real(sigPhaseRefSymb  )/max(abs(sigPhaseRefSymb  )),'ro');
             title('AFTER DF & ADC - REAL(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             subplot(212); plot( 1:NSampPerSymb, imag(s)/max(abs(s)), 'bx', 1:NSampPerSymb, imag(sigPhaseRefSymb  /max(abs(sigPhaseRefSymb  ))),'ro');
             title('AFTER DF & ADC - IMAG(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             ERR_TIME_PHASE_REF = max(abs(s-sigPhaseRefSymb  )), pause
          end
          
          x = abs(nullFFT);
          x(1) = 0;
          null = [x(1280:2048); x(1: 769)];
          maximum = max(null)
          average_x = mean(x(769:1280))
          pause
          null = null./maximum;
          if (maximum - average_x) > 10
              prog = (16*average_x)/maximum;
          else
              prog = (4*average_x)/maximum;
          end
          for c = 1 : 24
              for p = 1 : 70
                  match = 0;
                  for n = 1 : 1538
                      if null(n) > prog
                          null(n) = 1;
                      else
                          null(n) = 0;
                      end
                      if (A(n,p,c) * null(n)) == 1
                          match = match + 1;
                          if match >= 14
                              figure
                              plot(null);hold on
                              plot(A(:,p,c), 'r--');
                              X = [p c]
                              match = 0;
                              pause
                              close
                          end
                      end
                  end
              end
          end

    return;  % <================= !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       
end % of if (sufficient signal length) - sampling frequency correction
%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

% Read more signal samples

OldData = Signal;       % all data returned back
SignalOut = [];         % no DAB frame detected
