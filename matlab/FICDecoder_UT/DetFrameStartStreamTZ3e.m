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

function [ SignalOut, OldData ] = DetFrameStartStreamTZ3( Signal, ReadSize, coef, PlotOn )

global NSymbPerFrame NSampPerFrame NSampPerSymb NSampPerNull NCarrPerSymb Nfft NSampPerGuard PhaseRefSymb sigPhaseRefSymb fs df_fract df_int df
global nrDABFrame FrameStartpoints FrameLengths NSampPerFrameADC

persistent isum

if( prod(size(nrDABFrame))==0 ) isum = 0; nrDABFrame = 0; end

ABS_Signal = abs(Signal);

%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
if( length(Signal) > 2*NSampPerFrame )  % continue only if Signal is long enough
%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
  disp(' NEW PhaseRef ###################################################################################');

  if( nrDABFrame == 0 )   NSampPerSynchro = NSampPerFrame+NSampPerNull;
  else                    NSampPerSynchro = 2*NSampPerNull;
  end

  SUM = zeros(1,NSampPerSynchro);
  SUM(1) = sum( ABS_Signal(1:NSampPerNull) );
  for n = 2 : NSampPerSynchro; 
      SUM(n) = SUM(n-1) - ABS_Signal(n-1) + ABS_Signal(NSampPerNull+n-1);
  end
  [dummy imin] = min(SUM);
  i = imin+NSampPerNull;

       if( PlotOn ) % Show detected first sample of the Phase Reference OFDM Symbol 
           i,
           figure
           subplot(111); plot(i,0,'ro',1:length(ABS_Signal),real(Signal),'b-'); title('REAL Signal'); pause

           subplot(211); plot(i,0,'ro',1:length(ABS_Signal),real(Signal),'b-'); title('REAL Signal');
           subplot(212); plot(i,0,'ro',1:length(ABS_Signal),imag(Signal),'b-'); title('IMAG Signal'); pause
           figure; subplot(111); plot(abs(fft(Signal(i-Nfft-Nfft/8:i-1-Nfft/8)))); title('SPECTRUM ABS of NULL signal'); pause
           figure; subplot(111); plot(i,0,'ro',1:length(ABS_Signal),ABS_Signal,'b-'); title('SYNCHRO ABS via NULL signal and RunSum'); pause
       
           offs = 0; % for test only

         % TIME: compare first OFDM symbol with Phase Reference Symbol in TIME DOMAIN
           sreal = Signal(i+offs : i+offs+NSampPerSymb-1);  % Symbol with Cyclic Prefix
           sreal = sreal/max(abs(sreal));
           sref = ifft( PhaseRefSymb ); sref = [ sref(end-NSampPerGuard+1:end); sref ]; % Ref with Cyclic Prefix
           sref = sref/max(abs(sref));
           figure
           subplot(211); plot( 1:NSampPerSymb, real(sreal), 'bx', 1:NSampPerSymb, real(sref),'ro');
           title('BEFORE DF - REAL(time): PhaseRef Symbol (o) OFDM Symbol (x)');
           subplot(212); plot( 1:NSampPerSymb, imag(sreal), 'bx', 1:NSampPerSymb, imag(sref),'ro');
           title('BEFORE DF - IMAG(time): PhaseRef Symbol (o) OFDM Symbol (x)');
           ERR_TIME_PHASE_REF = max(abs(sreal-sref)), pause
          
         % FREQ: compare OFDM symbol and Phase Reference symbol in FREQUENCY DOMIAN
           sreal = Signal(i+offs+NSampPerGuard : i+offs+NSampPerGuard+Nfft-1); Sreal=fft(sreal); Sreal=Sreal/max(abs(Sreal));
           figure;
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
             figure
             subplot(211); plot( 1:NSampPerSymb, real(s)/max(abs(s)), 'bx', 1:NSampPerSymb, real(sigPhaseRefSymb)/max(abs(sigPhaseRefSymb)),'ro');
             title('AFTER FIRST FRACT DF - REAL(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             subplot(212); plot( 1:NSampPerSymb, imag(s)/max(abs(s)), 'bx', 1:NSampPerSymb, imag(sigPhaseRefSymb/max(abs(sigPhaseRefSymb))),'ro');
             title('AFTER FIRST FRACT DF - IMAG(time): PhaseRef Symbol (o) OFDM Symbol (x)');
             ERR_TIME_PHASE_REF = max(abs(s-sigPhaseRefSymb)), pause
          end
     
  % df_int = ? ###############################################################################################
    
  % Spectrum of signal samples belonging to estimated OFDM PhaseREf symbol (without Cyclic Prefix)
     if(1)  % in real mode
         work = s;                                   % work variable
         work = fft( work(NSampPerGuard+1:end) );    % cutting Guard Interval, then FFT
         work = work/max(abs(work));                 % scaling
     else   % for test only - shifted PhaseRef spectrum
         shift = -3;
         if(shift>0)  work = [ PhaseRefSymb(end-shift+1:end); PhaseRefSymb(1:end-shift) ]; end
         if(shift<0)  work = [ PhaseRefSymb((-shift+1):end); PhaseRefSymb(1:(-shift)) ]; end
         if(shift==0) work =   PhaseRefSymb; end
     end
 
  % Signal Spectrum correlation with the PhaseRef Spectrum
     XC = xcorr( work, PhaseRefSymb);
     
           if( PlotOn )
               M=25; m = Nfft-M : Nfft+M;
               figure;  
               subplot(311); plot( m-Nfft, real(XC(m)) ); title('Re(Xcorr) - df integer shift');
               subplot(312); plot( m-Nfft, imag(XC(m)) ); title('Im(Xcorr) - df integer shift');
               subplot(313); stem( m-Nfft,  abs(XC(m)) ); title('Abs(Xcorr) - df integer shift'); grid; pause
            end

      M=20; m = Nfft-M : Nfft+M;    % TZ !!! Assumed max frequency shift in freq bins
      [ val indx ] = max( XC(m) ); 
      df_intnew = indx - (M+1),
      
            if( PlotOn )
                figure;
                subplot(311); plot(real(XC)); title('Re(Xcorr)  - df integer shift');
                subplot(312); plot(imag(XC)); title('Im(Xcorr)  - df integer shift');
                subplot(313); plot( abs(XC)); title('Abs(Xcorr) - df integer shift'); pause
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
    
    if(1) 

         % +++++++++++++++++++++++++++++++  
         % Improving time synchronization
         % +++++++++++++++++++++++++++++++  
       
         if( prod(size(df))==0 ) dfx = df_intnew + df_fractnew;
         else                    dfx = coef*df + (1-coef)*(df_intnew+df_fractnew);
         end
       % dfx = df_intnew + df_fractnew;
         
         % df correction using dfx       
           s(1:NSampPerGuard+Nfft,1) = Signal(i:i+NSampPerGuard+Nfft-1) .* exp(-j*2*pi/Nfft*dfx*(i:i+NSampPerGuard+Nfft-1)).';
           
         % TIME OFFSET CALCULATION IN FREQ DOMAIN using PhaseRefSymb - correlation in freq domain  
         % s = circshift(sref,-5); % for test only
           S = fft( s(NSampPerGuard+1 : NSampPerGuard+Nfft) ); S=S/max(abs(S));
           work = ifft( S .* conj(PhaseRefSymb) );
           if(PlotOn) figure; subplot(111); stem(-Nfft/2+1:Nfft/2,abs(fftshift(work))); grid; title('TIME OFFSET in FREQ by XCORR using Phase Ref Symbol');  pause, end

           if(1) % OK
              M=7; work=[ work(end-M+1:end); work(1:M+1) ]; % TZ !!! Assumed max +/- 7
              [ dummy indx ] = max(abs(work(1:2*M+1))); 
              offs1a = indx-(M+1),
              if(PlotOn) figure; subplot(111); stem(-M:M,abs(work)); grid; title('TIME OFFSET in FREQ by XCORR using Phase Ref Symbol');  pause, end
           else  % BAD  
              [ dummy offs1a ] = max(abs(work));
              if(offs1a > Nfft/2) offs1a = -(Nfft-offs1a+1),
              else                offs1a =   offs1a-1,
              end
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
          if(PlotOn) figure; subplot(111); stem( -M : M, abs(XC) ); title('TIME OFFSET in TIME by XCORR using Phase Ref Symbol'); pause, end
      
        % TAKING DECISION ABOUT THE TIME OFFSET
          offs1 = offs1b; % calculated correction is taken into account or not (=0)
          if(PlotOn) figure(1); subplot(111); plot(i,0,'ro',i+offs1,0,'go',1:length(ABS_Signal),ABS_Signal,'b-'); title('Syncho 1 via NULL zone: RED-to-GEEN=corrected'); pause, end

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

    if(1)
          
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
       
        % df_int = ? - repeat calculation of df_fract taking more signal samples
          work = s; 
          work = fft( work(NSampPerGuard+1:end) );
          work = work/max(abs(work));
          XC = xcorr( work, PhaseRefSymb);
          if( PlotOn )
              M=25; m = Nfft-M : Nfft+M;
              figure;  
              subplot(311); plot( m-Nfft, real(XC(m)) ); title('Re(Xcorr) - df integer shift');
              subplot(312); plot( m-Nfft, imag(XC(m)) ); title('Im(Xcorr) - df integer shift');
              subplot(313); stem( m-Nfft,  abs(XC(m)) ); title('Abs(Xcorr) - df integer shift'); grid; pause
          end
          M=20; m = Nfft-M : Nfft+M;
          [ val indx ] = max( XC(m) ); 
           df_intnew = indx - (M+1),
           if( PlotOn )
               figure;
               subplot(311); plot(real(XC)); title('Re(Xcorr) - df integer shift');
               subplot(312); plot(imag(XC)); title('Im(Xcorr) - df integer shift');
               subplot(313); plot( abs(XC)); title('Abs(Xcorr) - df integer shift'); pause
           end
         
       end   % of if (improving df)
    
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% IMPROVING - STOP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

% Calculate frame startpoints and lengths
  overlap = 100;
  nrDABFrame = nrDABFrame + 1;
  isum = isum + (i+offs1);
  FrameStartpoints( nrDABFrame ) = isum;
  isum = isum + NSymbPerFrame*NSampPerSymb - overlap;
  if(nrDABFrame > 1) FrameLengths( nrDABFrame ) = FrameStartpoints( nrDABFrame ) -  FrameStartpoints( nrDABFrame-1 ), end

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
  
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ADC CORRECTION - START ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

% ADC sampling rate correction

  if(1) % set 0 for "clear" ETI-generated TX signals since their DAB frame is +1 sample longer!  

       if( nrDABFrame > 1 )
         % FrameStartpoints(nrDABFrame), FrameStartpoints(nrDABFrame-1),
           NSampPerFrameADCnew = FrameStartpoints(nrDABFrame) - FrameStartpoints(nrDABFrame-1);  % should be equal NSampPerFrame
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
       end % of nrDABFrame>0? 
  end     

% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ADC CORRECTION - STOP +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++       

    SignalOut = Signal(i+offs1 : i+offs1+NSymbPerFrame*NSampPerSymb-1) .* exp(-j*2*pi/Nfft*df*(0 : NSymbPerFrame*NSampPerSymb-1)).' ;   % detected DAB frame
    OldData = Signal(i+offs1+NSymbPerFrame*NSampPerSymb - overlap: end);  % data to be analyzed later
    return;  % <================= !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       
end % of if (sufficient signal length)
%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

% Read more signal samples

OldData = Signal;       % all data returned back
SignalOut = [];         % no DAB frame detected
