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
% Reading next block of complex IQ data from a file
% -----------------------------------------------------------------------------------------

function [Signal] = ComplexDataLoad( ReadFile, ReadSize, fs_file, fs, sigtype )

persistent N1 ReadAlready

if( prod(size(ReadAlready))==0 ) ReadAlready = 0; end

ReadSize = 600000; 

if(sigtype==1)                                            % USRP Ettus Research from Hoin/Swiss Record.dat, ETI_Katowice.dat
   t = fread( ReadFile, [2, ReadSize], 'float' );
   v = t(1,:) + t(2,:)*i;
%  size(v), pause
end

if(sigtype==2)                                            % USRP Ettus Research from TT
   t = fread(ReadFile,[2,ReadSize],'int16');              % np. dab181-936MHz_at2MHz_cplx_short.dat
   v = t(1,:) + t(2,:)*i;
end

if(sigtype==3)                                            % SDRSharp USB Stick                  
   if( isempty(N1) ) N1=1; end                            % SDRSharp_date_xxxxxxx_freqkHz_IQ.wav 
   t = wavread( ReadFile, [N1, N1+ReadSize-1] ); t=t';    % np. SDRSharp_20140728_132615Z_223930kHz_IQ.wav
   N1 = N1 + ReadSize;                                    %
   v = t(1,:) - t(2,:)*i;                                 %
end

if(sigtype==4)                                            % RTL USB Stick, recored by rtl_sdr (linux)
    [v count] = fread(ReadFile, 2*ReadSize, 'uint8');     % RTL has 8bit ADC
    if( count ~= 2*ReadSize )
        throw();
    end
    v = real(v);      % eg. rtl_sdr parameters:
    v = (v-127)/128;  % rtl_sdr -f 229072000 -s 2400000 -n 24000000 -g 28 -S dab_229072kHz_fs24MHz_gain28_sync.raw      
    v = v';
    v = v(1:2:end) + 1i*v(2:2:end);
end 

[r, c] = size(v);
v = reshape(v, c, r);
if( fs_file ~= fs )
    v = resample(v, fs/1e3, fs_file/1e3);     % signal resampling from fs_file to fs (page 145)
end    
Signal = v;
%size(v), pause
Signal = Signal - mean(Signal);

if(0)
   % WriteFile = fopen('DAB_real_2.048MHz_IQ_float.dat', 'wb');
     WriteFile = fopen('DAB_synt_2.048MHz_IQ_float.dat', 'wb');
     n1=100001; n2=ReadSize;
     vv = [ real(v(n1:n2))'; imag(v(n1:n2))']; 
     fwrite(WriteFile,vv,'float');
     fclose(WriteFile);
     disp('WRITTEN TO DISC!');
     pause
end

ReadAlready = ReadAlready + ReadSize; % pause

