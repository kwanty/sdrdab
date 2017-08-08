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
% Read data from IQ file and resample them
% -----------------------------------------------------------------------------------------

function [Signal] = DataLoad_RT( Filename, NSamples, fs_file, fs, sigtype )

if(sigtype==1)                                     % From Internet
                                                   % Record1 Swiss -> 2 MSample,  9.8Sec, 20 091 724 Samples, 227.360MHz
   t = fread( Filename, [2, NSamples], 'float' );  % Record2 Swiss -> 2 MSample, 10.2Sec, 20 314 354 Samples, 194.064MHz
   b = t(1,:) + t(2,:)*i;                          % Record3 Katowice --> 2.048 MSample
end

if(sigtype==2)                                     % USRP Ettus Research
   b = fread(f,[2,NSamples],'int16');              % np. dab181-936MHz_at2MHz_cplx_short.dat
   b  = b(1,:) + sqrt(-1)*b(2,:);                  %
end  

if(sigtype==3)                                     % SDRSharp USB Stick
   b = wavread(Filename,NSamples);                 % SDRSharp_date_xxxxxxx_freqkHz_IQ.wav
   b = b(:,1) - i*b(:,2);                          % np. SDRSharp_20140728_132615Z_223930kHz_IQ.wav   
end

if(sigtype==4)                                         % RTL USB Stick, recored by rtl_sdr (linux)
    [b count] = fread(Filename, 2*NSamples, 'uint8');  % RTL has 8bit ADC
    if( count ~= 2*NSamples )
        throw();
    end
    b = real(b);      % eg. rtl_sdr parameters:
    b = (b-127)/128;  % rtl_sdr -f 229072000 -s 2400000 -n 24000000 -g 28 -S dab_229072kHz_fs24MHz_gain28_sync.raw      
    b = b';
    b = b(1:2:end) + 1i*b(2:2:end);
end 
                                                   % FOR ALL
if( fs_file ~= fs)                                 % resampling to frequency fs if necessary
    b = resample(b, fs/1e3, fs_file/1e3);          % X MS -> 2.048 MS, Standard Page 145, T = 1/2048000 s
end

Signal = b;
Signal = Signal - mean(Signal);
