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

fprintf('\n*************** frame_pos_fft generator started ***************\n\n');

rng(1);

Nfft = 2048;
NSymbPerFrame = 76;
NCarrPerSymb = Nfft * 3 / 4;
NSymbPerFIC = 3;
NSampPerGuard = 504;
NSampPerSymb = Nfft + NSampPerGuard;
OFDMOffset = NSampPerGuard / 2;
n1st = 1;
ofdm_Array = zeros( [NSymbPerFrame-(n1st-1), Nfft] );
fft_Array = zeros( [NSymbPerFrame-(n1st-1), Nfft] );


Data = randi(100, NSymbPerFrame * NSampPerSymb, 1) + 1i * randi(100, NSymbPerFrame * NSampPerSymb, 1);

% save input data
fp = fopen('../data/ut/demodulator_ofdm_fft_input_mode_1.txt', 'w');
for i = 1: size(Data, 1)
    fprintf(fp, '%1.18f\n%1.18f\n', real(Data(i)), imag(Data(i)));
end
fclose(fp);


for nrOFDMSymb = n1st : NSymbPerFrame                   % taking only subset of all symbols (in MSC [4-76] = [4-NSymbPerFrame])
    
  % disp('OFDM Frame');
    i = (nrOFDMSymb-1) * NSampPerSymb;                  % index of the first sample
    OFDMSymbol = Data(1+i:NSampPerGuard+Nfft+i);        % data samples corresponding to one OFDM symbol
    OFDMSymbol = OFDMSymbol(NSampPerGuard+1-OFDMOffset : NSampPerGuard+Nfft-OFDMOffset );
    
    ofdm_Array( nrOFDMSymb-(n1st-1), : ) = OFDMSymbol;
  
    fft_Array( nrOFDMSymb-(n1st-1), : ) = fft( OFDMSymbol );      % FFT of them
    
end % of for ( series of FFT )


% save output data - ofdm
fp = fopen('../data/ut/demodulator_ofdm_output_mode_1.txt', 'w');
for l = 1: size(ofdm_Array, 1)
    
    for k = 1: size(ofdm_Array, 2)
        
        fprintf(fp, '%1.18f\n%1.18f\n', real(ofdm_Array(l, k)), imag(ofdm_Array(l, k)));
    end
end
fclose(fp);


% save output data - fft
fp = fopen('../data/ut/demodulator_fft_output_mode_1.txt', 'w');
for l = 1: size(fft_Array, 1)
    
    for k = 1: size(fft_Array, 2)
        
        fprintf(fp, '%1.18f\n%1.18f\n', real(fft_Array(l, k)), imag(fft_Array(l, k)));
    end
end
fclose(fp);