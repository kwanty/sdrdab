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

function [ FrameStartpoints ] = DetFreqOffset( Signal, FrameNr, FrameStartpoints, NSymbPerFrame, NSampPerSymb, NSampPerGuard, Nfft, NCarrPerSymb, PhaseRefSymb, sigPhaseRefSymb, Offset, coef, PlotOn)

global df df_int df_fract

  i = FrameStartpoints( FrameNr);
  
% df_fract = ? fractional frequency offset estimation #1
  Ncut = 25;
  z =  sum( conj(Signal(i+Ncut : i+NSampPerGuard-Ncut-1)) .* Signal(i+Nfft+Ncut:i+Nfft+NSampPerGuard-Ncut-1) );
  df_fractnew = angle(z)/(2*pi),
    
% df_fract compensation #1
  s(1:NSampPerGuard+Nfft,1) = Signal(i:i+NSampPerGuard+Nfft-1) .* exp(-j*2*pi/Nfft*df_fractnew*(i:i+NSampPerGuard+Nfft-1)).';   % df fractional
 
% Figures  
  if( PlotOn )
      subplot(211); plot( 1:NSampPerSymb, real(s)/max(abs(s)), 'bx', 1:NSampPerSymb, real(sigPhaseRefSymb)/max(abs(sigPhaseRefSymb)),'ro');
      title('AFTER DF - REAL(time): PhaseRef Symbol (o) OFDM Symbol (x)');
      subplot(212); plot( 1:NSampPerSymb, imag(s)/max(abs(s)), 'bx', 1:NSampPerSymb, imag(sigPhaseRefSymb/max(abs(sigPhaseRefSymb))),'ro');
      title('AFTER DF - IMAG(time): PhaseRef Symbol (o) OFDM Symbol (x)');
      ERR_TIME_PHASE_REF = max(abs(s-sigPhaseRefSymb)), pause
   end
     
% df_int = ? using PhaseRef OFDM symbol
  if(1)  % real work: find integer frequency shift
      work = s; 
      work = fft( work(NSampPerGuard+1:end) );
      work = work/max(abs(work));
  else   % test only
      shift = -3;
      if(shift>0)  work = [ PhaseRefSymb(end-shift+1:end); PhaseRefSymb(1:end-shift) ]; end
      if(shift<0)  work = [ PhaseRefSymb((-shift+1):end); PhaseRefSymb(1:(-shift)) ]; end
      if(shift==0) work =   PhaseRefSymb; end
  end
  XC = xcorr( work, PhaseRefSymb);
  if(PlotOn)
      M=25; m = Nfft-M : Nfft+M;
      figure(4);  
      subplot(311); plot( m-Nfft, real(XC(m)) ); title('Re(Xcorr) - df integer shift');
      subplot(312); plot( m-Nfft, imag(XC(m)) ); title('Im(Xcorr) - df integer shift');
      subplot(313); stem( m-Nfft,  abs(XC(m)) ); title('Abs(Xcorr) - df integer shift'); grid; pause
  end

  M=20; m = Nfft-M : Nfft+M;
  [ val indx ] = max( XC(m) ); 
  df_intnew = indx - (M+1),
  if(PlotOn)
      figure(4);
      subplot(311); plot(real(XC)); title('Re(Xcorr) - df integer shift');
      subplot(312); plot(imag(XC)); title('Im(Xcorr) - df integer shift');
      subplot(313); plot( abs(XC)); title('Abs(Xcorr) - df integer shift'); pause
  end
     
% df_int compensation #1
  s = s .* exp(-j*2*pi/Nfft*df_intnew*(i:i+NSampPerGuard+Nfft-1)).';   % df integer      
  if(PlotOn) % Compare spectra
      work = fft( s(NSampPerGuard+1:end) );
      work = work/max(abs(work));
      if(df_intnew>0) work = [ work((df_intnew+1):end);  work(1:df_intnew) ]; end
      if(df_intnew<0) work = [ work(end+df_intnew+1:end); work(1:end+df_intnew) ]; end
      df_int_error = max( abs( work-PhaseRefSymb ) ),
      subplot(111); plot(1:Nfft,abs(PhaseRefSymb),'ro',1:Nfft,abs(work),'bx'); title('PhaseRef Spectra: Ref (RED) Sig (BLUE'); % pause  
  end

 % ###############################################################
   if(1) % Time offset calculation:  FrameStartpoint(i) correction 
 % ###############################################################
  
 % Time offset calculation in FREQ DOMAIN using PhaseRefSymb   
 % s = circshift(sref,-5); % for test only
   S = fft( s(NSampPerGuard+1 : NSampPerGuard+Nfft) ); S=S/max(abs(S));
   work = ifft( S .* conj(PhaseRefSymb) );
   if(PlotOn) subplot(111); stem(-Nfft/2+1:Nfft/2,abs(fftshift(work))); grid; title('TIME OFFSET in FREQ by XCORR using Phase Ref Symbol');  pause, end
   if(1) % OK
       M=7; work=[ work(end-M+1:end); work(1:M+1) ];
       [ dummy indx ] = max(abs(work(1:2*M+1))); 
       offs1a = indx-(M+1),
       if(PlotOn) subplot(111); stem(-M:M,abs(work)); grid; title('TIME OFFSET in FREQ by XCORR using Phase Ref Symbol');  pause, end
   else   
       [ dummy offs1a ] = max(abs(work));
       if(offs1a > Nfft/2) offs1a = -(Nfft-offs1a+1),
       else                offs1a =   offs1a-1,
       end
   end   
  
% Time offset calculation in TIME DOMAIN using PhaseRefSymb   
  M = 7; XC=[];
  XC(M+1) = sum( s .* conj(sigPhaseRefSymb) )/NSampPerSymb;
  for m = 1 : M
      XC( M+1+m ) = sum( s(1+m:NSampPerSymb) .* conj(sigPhaseRefSymb(1:NSampPerSymb-m)) )/(NSampPerSymb-m);
  end
  for m = 1 : M
      XC( M+1-m ) = sum( s(1:NSampPerSymb-m) .* conj(sigPhaseRefSymb(1+m:NSampPerSymb)) )/(NSampPerSymb-m);
  end
  [ dummy offs1b ] = max(abs(XC));
  offs1b = offs1b - (M+1),
  if(PlotOn) subplot(111); stem( -M : M, abs(XC) ); title('TIME OFFSET in TIME by XCORR using Phase Ref Symbol'); pause, end
      
% ##### TAKING DECISION ABOUT THE TIME OFFSET: calculated correction is taken into account or not (=0):
% offs1a, offs1b, ofss1a+offs1b)/2 or 0
  
  offs1 = offs1b;
  if(PlotOn)
      ABS_Signal = abs(Signal);
      figure(1); subplot(111); plot(i,0,'ro',i+offs1,0,'go',1:length(ABS_Signal),ABS_Signal,'b-'); title('Syncho 1 via NULL zone: RED-to-GEEN=corrected'); pause,
  end

% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  FrameStartpoints( FrameNr ) = i+offs1;  % CORRECTION!
% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

% #############################################################
   end % Time offset calculation 
 % ############################################################
 
% $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  
  if(1) % precise df correction - time offset is NECESSARY !!!
% $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

% df_fract = ? fractional frequency offset estimation #2  
  Ncut = 5;
  z =  sum( conj(Signal(i+offs1+Ncut : i+offs1+NSampPerGuard-Ncut-1)) .* Signal(i+offs1+Nfft+Ncut:i+offs1+Nfft+NSampPerGuard-Ncut-1) );
  df_fractnew = angle(z)/(2*pi),

% df_fract compensation #2
  s = Signal(i+offs1:i+offs1+NSampPerGuard+Nfft-1) .* exp(-j*2*pi/Nfft*df_fractnew*(i+offs1:i+offs1+NSampPerGuard+Nfft-1)).';   % df fractional      
       
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
     
% df_int = ? coarse integer frequency offset estimation
   work = fft( s(NSampPerGuard+1:end) );
   work = work/max(abs(work));
   XC = xcorr( work, PhaseRefSymb);
   if(PlotOn)
      M=25; m = Nfft-M : Nfft+M;
      figure(4);  
      subplot(311); plot( m-Nfft, real(XC(m)) ); title('Re(Xcorr) - df integer shift');
      subplot(312); plot( m-Nfft, imag(XC(m)) ); title('Im(Xcorr) - df integer shift');
      subplot(313); stem( m-Nfft,  abs(XC(m)) ); title('Abs(Xcorr) - df integer shift'); grid; pause
   end
   M=20; m = Nfft-M : Nfft+M;
   [ val indx ] = max( XC(m) ); 
   df_intnew = indx - (M+1),
   if(PlotOn)
      figure(4);
      subplot(311); plot(real(XC)); title('Re(Xcorr) - df integer shift');
      subplot(312); plot(imag(XC)); title('Im(Xcorr) - df integer shift');
      subplot(313); plot( abs(XC)); title('Abs(Xcorr) - df integer shift'); pause

      if(df_int>0) work = [ work((df_int+1):end);  work(1:df_int) ]; end
      if(df_int<0) work = [ work(end+df_int+1:end); work(1:end+df_int) ]; end
      df_int_error = max( abs( work-PhaseRefSymb ) ),
      subplot(111); plot(1:Nfft,abs(PhaseRefSymb),'ro',1:Nfft,abs(work),'bx'); % pause
  end
       
% $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
  end % precise df correction
% $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
       
  df_new = df_intnew + df_fractnew;
  if( prod(size(df))==0 ) df = df_new,
  else                    df = coef*df + (1-coef)*df_new,
  end
  df_int = round(df);
  df_fract = df - df_int; % pause
