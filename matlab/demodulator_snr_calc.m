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
%% Clear environment
close all;
clear all;

    ReadSize = 500000;  
ReadFile = fopen('../data/antena-1_dab_229072kHz_fs2048kHz_gain42_1_long.raw', 'rb');
if (ReadFile == -1) disp('---- Cannot read from: data/antena-1_dab_229072kHz_fs2048kHz_gain42_1_long.raw ----'); break; end

    x = real(fread(ReadFile, 2*ReadSize, 'uint8'));
    fclose(ReadFile);
    x = x-127;
    x = x(1:2:end) + sqrt(-1)*x(2:2:end);  

    
null_poz = 170134; %Antena #1
PRS_poz = null_poz + 2656;
  
df = -7.159513588596336;
NSymbPerFrame = 76;
NSampPerSymb = 2552;
Nfft = 2048;

tab_snr = zeros(1, NSymbPerFrame);

x(PRS_poz:PRS_poz+NSymbPerFrame*NSampPerSymb-1) = x(PRS_poz : PRS_poz+NSymbPerFrame*NSampPerSymb-1) .* exp(-j*2*pi/Nfft*df*(0 : NSymbPerFrame*NSampPerSymb-1)).' ;

for i = 0:NSymbPerFrame-1

  G = 504; %CP length
  N = Nfft;
  S=0; W=0;
  start = PRS_poz + i * NSampPerSymb;
  for k = start : start + G - 1
      S = S + real(x(k))*real(x(k+N)) + imag(x(k))*imag(x(k+N));
      W = W + (real(x(k)) - real(x(k+N)))*(real(x(k)) - real(x(k+N))) + (imag(x(k)) - imag(x(k+N)))*(imag(x(k)) - imag(x(k+N)));
  end
  
  if(W == 0) logSNR = 1000;
  else
      SNR = 2*S/W;
      if (SNR <= 0) logSNR = -1000 
      else logSNR = 10*log10(SNR);
      end
  end
  tab_snr(i+1) = logSNR;
end


data = [real(x(PRS_poz:PRS_poz+NSymbPerFrame*NSampPerSymb-1)) imag(x(PRS_poz:PRS_poz+NSymbPerFrame*NSampPerSymb-1))].';
data = reshape(data, 2*NSymbPerFrame*NSampPerSymb, 1);


fp = fopen( '../data/ut/demodulator_snr_input.txt', 'w' );

for i=1:NSampPerSymb*NSymbPerFrame*2
   fprintf( fp, '%1.18f\n', data(i) );
end
fclose( fp );

fp = fopen( '../data/ut/demodulator_snr_output.txt', 'w' );

for i=1:NSymbPerFrame
   fprintf( fp, '%1.18f\n', tab_snr(i) );
end
fclose( fp );
%============================Mode==2=====================================
close all;
clear all;

    ReadSize = 500000;  
ReadFile = fopen('../data/London_mode2_iq.raw', 'rb');
if (ReadFile == -1) disp('---- Cannot read from: data/London_mode2_iq.raw ----'); break; end

    x = real(fread(ReadFile, 2*ReadSize, 'uint8'));
    fclose(ReadFile);
    x = x-127;
    x = x(1:2:end) + sqrt(-1)*x(2:2:end);  

    
null_poz = 98305; %London mode2
PRS_poz = null_poz + 664;
  
df = -7.159513588596336;
NSymbPerFrame = 76;
NSampPerSymb = 638;
Nfft = 512;

tab_snr = zeros(1, NSymbPerFrame);

% x(PRS_poz:PRS_poz+NSymbPerFrame*NSampPerSymb-1) = x(PRS_poz : PRS_poz+NSymbPerFrame*NSampPerSymb-1) .* exp(-j*2*pi/Nfft*df*(0 : NSymbPerFrame*NSampPerSymb-1)).' ;

for i = 0:NSymbPerFrame-1

  G = 126; %CP length
  N = Nfft;
  S=0; W=0;
  start = PRS_poz + i * NSampPerSymb;
  for k = start : start + G - 1
      S = S + real(x(k))*real(x(k+N)) + imag(x(k))*imag(x(k+N));
      W = W + (real(x(k)) - real(x(k+N)))*(real(x(k)) - real(x(k+N))) + (imag(x(k)) - imag(x(k+N)))*(imag(x(k)) - imag(x(k+N)));
  end
  
  if(W == 0) logSNR = 1000;
  else
      SNR = 2*S/W;
      if (SNR <= 0) logSNR = -1000 
      else logSNR = 10*log10(SNR);
      end
  end
  tab_snr(i+1) = logSNR;
end


data = [real(x(PRS_poz:PRS_poz+NSymbPerFrame*NSampPerSymb-1)) imag(x(PRS_poz:PRS_poz+NSymbPerFrame*NSampPerSymb-1))].';
data = reshape(data, 2*NSymbPerFrame*NSampPerSymb, 1);

% figure; plot(real(abs(x(PRS_poz:PRS_poz+G)-x(PRS_poz+Nfft:PRS_poz+Nfft+G))));

fp = fopen( '../data/ut/demodulator_snr_input_mode_2.txt', 'w' );

for i=1:NSampPerSymb*NSymbPerFrame*2
   fprintf( fp, '%1.18f\n', data(i) );
end
fclose( fp );

fp = fopen( '../data/ut/demodulator_snr_output_mode_2.txt', 'w' );

for i=1:NSymbPerFrame
   fprintf( fp, '%1.18f\n', tab_snr(i) );
end
fclose( fp );