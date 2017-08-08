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
clear;

rng(1);

fprintf('\n*************** deqpsk generator started ***************\n\n');

Nfft = 2048;
NSymbPerFrame = 76;
NCarrPerSymb = Nfft * 3 / 4;
NSampPerGuard = 504;
NSampPerSymb = Nfft + NSampPerGuard;
OFDMOffset = NSampPerGuard / 2;
NSymbPerFIC = 3;
n1st = 1;
fft_Array_in = zeros( [NSymbPerFrame-(n1st-1), Nfft] );
fft_Array = zeros( [NSymbPerFrame-(n1st-1), NCarrPerSymb] );

Data = randi(100, NSymbPerFrame * NSampPerSymb, 1) + 1i * randi(100, NSymbPerFrame * NSampPerSymb, 1);

for nrOFDMSymb = n1st : NSymbPerFrame                   % taking only subset of all symbols (in MSC [4-76] = [4-NSymbPerFrame])
    
    i = (nrOFDMSymb-1) * NSampPerSymb;                  % index of the first sample
    OFDMSymbol = Data(1+i:NSampPerGuard+Nfft+i);        % data samples corresponding to one OFDM symbol
    OFDMSymbol = OFDMSymbol(NSampPerGuard+1-OFDMOffset : NSampPerGuard+Nfft-OFDMOffset );
    
    fft_Array_in( nrOFDMSymb-(n1st-1), : ) = OFDMSymbol;      % FFT of them
    
    Y = fftshift( OFDMSymbol );          % reordering: negative frequencies to the left
    fft_Array( nrOFDMSymb-(n1st-1), 1 : NCarrPerSymb/2 )              = Y( Nfft/2-NCarrPerSymb/2+1 : Nfft/2 );   % setting FFT array
    fft_Array( nrOFDMSymb-(n1st-1), NCarrPerSymb/2+1 : NCarrPerSymb ) = Y( Nfft/2+2 : Nfft/2+NCarrPerSymb/2+1 ); % take used carriers only
    
end % of for ( series of FFT )

fft_Array2 = fft_Array( 2:NSymbPerFrame-(n1st-1), : ) .* conj( fft_Array( 1:NSymbPerFrame-(n1st-1)-1, :) );  % phase difference calculation

FFTFrame = fft_Array2;

cd ref/

[FreqInterleaverTab]   = FreqInterleaverTabGen(Nfft);                       
[FreqDeinterleaverTab] = FreqDeinterleaverTabGen(FreqInterleaverTab, Nfft);

cd ..

DeintFFTFrame = zeros(size(FFTFrame));
for r = 1 : length( FreqDeinterleaverTab(:,4) )                               
  DeintFFTFrame( :, FreqDeinterleaverTab(r,4) ) = FFTFrame(:,r);  % Frequency de-interleaving
end

% save input data

fp = fopen('../data/ut/demodulator_deqpsk_input_mode_1.txt', 'w');
for l = 1: size(fft_Array_in, 1)
    
    for k = 1: size(fft_Array_in, 2)
        
        fprintf(fp, '%1.18f\n%1.18f\n', real(fft_Array_in(l, k)), imag(fft_Array_in(l, k)));
    end
end
fclose(fp);

% dpsk output

DataFIC = zeros(1, NSymbPerFIC*NCarrPerSymb*2);                  % Our example (Mode=1): 3*1536*2
DataMSC = zeros(1,(NSymbPerFrame-NSymbPerFIC-1)*NCarrPerSymb*2); % Our example (Mode=1): 76-3-1(differential) = 72 Symbols in MSC
for f = 1 : NSymbPerFIC                  % FIC DATA from OFDM symbols - first real parts, then imag parts 
  DataFIC( 1+(f-1)*NCarrPerSymb*2 : (f)*NCarrPerSymb*2 ) = [real(DeintFFTFrame(f,:)) imag(DeintFFTFrame(f,:))];
end
for f = NSymbPerFIC+1 : NSymbPerFrame-1  % MSC DATA from OFDM symbols - first real parts, then imag parts 
  ff = f - NSymbPerFIC;
  DataMSC( 1+(ff-1)*NCarrPerSymb*2 : (ff)*NCarrPerSymb*2 ) = [real(DeintFFTFrame(f,:)) imag(DeintFFTFrame(f,:))];
end

dpsk_output = [DataFIC DataMSC];
  
fp = fopen('../data/ut/demodulator_deqpsk_output_dpsk_mode_1.txt', 'w');
for i = 1: length(dpsk_output)
        
    fprintf(fp, '%1.18f\n', dpsk_output(i));
end
fclose(fp);