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

  clc; clear all; close all;

  global df df_int df_fract
  global NSymbPerFrame NSampPerFrame NSampPerSymb NSampPerNull Nfft NSampPerGuard PhaseRefSymb sigPhaseRefSymb

  NSamples = 2^20;
  PlotOn = 0;
  DebugOn = 0;
  fs = 2.048e6;     % standard sampling frequency for DAB

% Record1.dat              ->  2 MSample, 9.8Sec,  20 091 724 Samples, 227.360MHz
% Record2.dat              ->  2 MSample, 10.2Sec, 20 314 354 Samples, 194.064MHz
% Record3_katowice_iq.dat  ->  2.048 MSample
% SDRSharp_20140728_132615Z_223930kHz_IQ.wav -> 2.048 MSample

% DAB_IQ input file to be decoded, sampling freq, signal type

% Internet
% Filename = 'Record1.dat';                                 fs_file = 2.0e6;    sigtype=1;   fi=0;   coef = 0.9;
% Filename = 'Record2.dat';                                 fs_file = 2.0e6;    sigtype=1;   fi=0;   coef = 0.9;
% Filename = 'Record3_katowice_iq.dat';                     fs_file = 2.048e6;  sigtype=1;   fi=0;   coef = 0;

% USRP #1 TT
% FileName = 'dab181-936MHz_at2MHz_cplx_short.dat';         fs_file = 2e6;      sigtype=2;   fi=0;  coef=0;
% FileName = 'dab223-936MHz_at2MHz_cplx_short.dat';         fs_file = 2e6;      sigtype=2;   fi=0;  coef=0;  

% USB STICK DAB/FM - TT                                                                                            % Without ADC correct 
% Filename = 'SDRSharp_20140728_133001Z_181930kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;  % fi=-75/180*pi;    
% Filename = 'SDRSharp_20140728_132615Z_223930kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;  % fi=-35/180*pi;
  Filename = 'SDRSharp_20140728_132746Z_227360kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;  % fi=-115/180*pi; 

% USB STICK DAB/FM - TZ Kraków                                                                                            % Without ADC correct 
% Filename = 'SDRSharp_20150102_110331Z_229277kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'SDRSharp_20150102_111436Z_229277kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'SDRSharp_20150102_123014Z_229297kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'DAB_Krakow_20150102_IQ_3secs.wav'; fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% OK
% Filename = 'SDRSharp_20150102_125222Z_229077kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'SDRSharp_20150102_125438Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'SDRSharp_20150102_185516Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'SDRSharp_20150102_190532Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% Filename = 'SDRSharp_20150103_104431Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% Filename = 'SDRSharp_20150103_105533Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% Filename = 'SDRSharp_20150103_110445Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% Filename = 'SDRSharp_20150103_192637Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'SDRSharp_20150103_192918Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'SDRSharp_20150103_194107Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% Filename = 'SDRSharp_20150103_194351Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;

% DAB KRAKÓW JB
% ReadFile = fopen('antena_3_dab_229072kHz_fs2048kHz_gain44_BAD.raw', 'rb'); fs_file=2.048e6; sigtype=4; fi=0; coef=0.9;
% ReadFile = fopen('antena_3_dab_229072kHz_fs2048kHz_gain44_OK.raw', 'rb');  fs_file=2.048e6; sigtype=4; fi=0; coef=0.9;

% Filename = 'London_mode1_iq.dat';  fs_file = 2.048e6;  sigtype=1;   fi=0;  coef=0.9;
% Filename = 'London_mode2_iq.dat';  fs_file = 2.048e6;  sigtype=1;   fi=0;  coef=0.9;
% Filename = 'London_mode3_iq.dat';  fs_file = 2.048e6;  sigtype=1;   fi=0;  coef=0.9;
% Filename = 'London_mode3p_iq.dat'; fs_file = 2.048e6;  sigtype=1;   fi=0;  coef=0.9;
% Filename = 'London_mode4_iq.dat';  fs_file = 2.048e6;  sigtype=1;   fi=0;  coef=0.9;

% Filename = 'Kielce_mode2_iq.dat';  fs_file = 2.048e6;  sigtype=1;   fi=0;   coef = 0;
% Filename = 'Kielce_mode4_iq.dat';  fs_file = 2.048e6;  sigtype=1;   fi=0;   coef = 0;

% For Matlab Vitterbi decoder when used                     % When used for FIC & MSC:
%                                                           % polynomial generation for Trellis (page 130, below fig. 72
  trellis = poly2trellis(7,[133 171 145 133]);              % 7 = delay of each bitstream

  tic;
% Load IQ file ----------------------------------------------------------------------------
  [ Signal ] = DataLoad( Filename, NSamples, fs_file, fs, sigtype );

% Detect frame begining -------------------------------------------------------------------
  FrameStartpoints = DetFrameStart( Signal, PlotOn );
  FrameLengths = FrameStartpoints(2:end) - FrameStartpoints(1:end-1),

% Time in samples between two frames (checking ADC sampling frequency - optional ) --------
  if( PlotOn) ADCFreqCheck(FrameStartpoints); end

  df=[]; df_int=[]; df_fract=[];
% Main loop
  for FrameNr = 1 : length(FrameStartpoints)-1
      
         disp('XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX');
         FrameNr
      
      % Mode detection --------------------------------------------------------------------
      % [ Mode, NSampPerFrame, NSampPerNull, NSymbPerFrame, NSampPerSymb, NSampPerGuard, Nfft, NCarrPerSymb, FreqIntTabSize, Offset ] = DetMode( FrameStartpoints );
        [ Mode, NSampPerFrame, NSampPerNull, NSymbPerFrame, NSampPerSymb, NSampPerGuard, Nfft, NCarrPerSymb, FreqIntTabSize, Offset, NCIFsPerFrame, NSymbPerFIC ] = DetModeX( Signal, FrameStartpoints );

      % Mode, pause
        
      % Phase Ref Symbol generation (used for synchro) ------------------------------------
      % [ PhaseRefSymb, sigPhaseRefSymb ] = PhaseRefSymbGen( Mode );
        [PhaseRefSymb, sigPhaseRefSymb] = PhaseRefSymbGen( Mode, PlotOn );
 
      % Frequency offset calculation, FrameStartpoints correction --------------------------
        [ FrameStartpoints ] = DetFreqOffset( Signal, FrameNr, FrameStartpoints, NSymbPerFrame, NSampPerSymb, NSampPerGuard, Nfft, NCarrPerSymb, PhaseRefSymb, sigPhaseRefSymb, Offset, coef, PlotOn);

      % FFT of OFDM symbol #1, optional ---------------------------------------------------
        if( PlotOn )fftSymb1( Signal, FrameStartpoints, Nfft, fs ); end
      
      % FFT of all symbols ----------------------------------------------------------------
        [ fft_Frame ] = SymbolFFT_TZ2( Signal, FrameNr, FrameStartpoints, NSampPerFrame, NSampPerNull, NSymbPerFrame, NSampPerSymb, NSampPerGuard, Nfft, NCarrPerSymb, Offset, coef, fi, PlotOn);
        if(DebugOn==1) FFTFrameSize = size(fft_Frame), pause, end

        disp(' '); disp('################################## Wait ...');

      % Frequency de-interleaving ---------------------------------------------------------
       [InterleavingTab] = FreqInterleavingTab(FreqIntTabSize);
       [DeintFFTFrame] = FreqDeInterleaving(InterleavingTab,fft_Frame,FreqIntTabSize);
        
        % D-QPSK symbol demapper - from 2D matrix to 1D vector ----------------------------------
        Data = zeros(1,(NSymbPerFrame-1)*NCarrPerSymb*2);          % 76 FFT's -1 differentialy
        for f=1:NSymbPerFrame-2
            Data(1+(f-1)*NCarrPerSymb*2:(f)*NCarrPerSymb*2) = [real(DeintFFTFrame(f,:)) imag(DeintFFTFrame(f,:))];
        end
        if(DebugOn==1) DataSize = size(Data), pause, end  % Mode1=230400, M2=57600, M3=58368, M4=115200
        
      % Building convolutinal codewords ---------------------------------------------------------
        if(Mode==1) NCIFsPerDABFrame=4; NFIBsPerCIF=3; NFIBsPerDABFrame=12; end
        if(Mode==2) NCIFsPerDABFrame=1; NFIBsPerCIF=3; NFIBsPerDABFrame=3;  end
        if(Mode==3) NCIFsPerDABFrame=1; NFIBsPerCIF=4; NFIBsPerDABFrame=4;  end
        if(Mode==4) NCIFsPerDABFrame=2; NFIBsPerCIF=3; NFIBsPerDABFrame=6;  end
        
        NbitsPerFIBs = NFIBsPerCIF * 256;               % 256 = number of bits per one FIB
        FIC = zeros(NCIFsPerDABFrame, 3*NbitsPerFIBs);  % 3*NbitsPerFIBs=2304/2304/3072/2304
        for r=0:NCIFsPerDABFrame-1
            FIC(r+1,:) = Data(r*(3*NbitsPerFIBs)+1:(r+1)*3*(NbitsPerFIBs));
        end
        if(DebugOn==1) FICSize = size(FIC), pause, end
        
      % De-puncturing ---------------------------------------------------------------------------
        if(Mode~=3) DataDep = [depuncturing(FIC(:,1:2016),16) depuncturing(FIC(:,2017:2292),15) depuncturing(FIC(:,2293:2304),8)];
        else        DataDep = [depuncturing(FIC(:,1:2784),16) depuncturing(FIC(:,2785:3060),15) depuncturing(FIC(:,3061:3072),8)];
        end
        if(DebugOn==1) DataDepSize = size(DataDep), pause, end
        
      % Viterbi decoder -------------------------------------------------------------------------
        DataVit = zeros(NCIFsPerDABFrame,NFIBsPerCIF*256+6);          % Bitvector + Tail
        for f=1:NCIFsPerDABFrame
      %     DataVit(f,:) = DABViterbi(DataDep(f,:));                                       % OK! Ours
            DataVit(f,:) = vitdec( DataDep(f,:), trellis, 1, 'trunc', 'unquant');  %       % OK! Matlab
      %     DataVit(f,:) = vitdec( [DataDep(f,:) 0 0 0 0], trellis, 1, 'cont', 'unquant'); % BAD! From MSC 
        end
        DataVit = DataVit(:,1:end-6);               % Tail removing
        if(DebugOn==1) DataVitSize = size(DataVit), pause, end

      % Energy dispersal ------------------------------------------------------------------------
        DataEnerg = zeros(size(DataVit));
        for m=1:NCIFsPerDABFrame
            DataEnerg(m,:) = xor( DataVit(m,:), EnergyDispGen(NFIBsPerCIF*256) );
        end
        if(DebugOn==1) DataEnergSize = size(DataEnerg), pause, end

      % FIBs building ---------------------------------------------------------------------------
      %  if(Mode ~= 3) FIB = reshape(DataEnerg',256,NFIBsPerDABFrame)';
      %  else          FIB = DataEnerg';
      %  end
        FIB = reshape(DataEnerg',256,NFIBsPerDABFrame)';
        if(DebugOn==1) FIBSize = size( FIB ), pause, end
        
      % CRC checking of FIBS --------------------------------------------------------------------
      %  Correct for Mode = 1 
      %  FIBCRCCheck = [ CRC16(FIB(1,:)) CRC16(FIB(2,:))  CRC16(FIB(3,:))  CRC16(FIB(4,:)) ...
      %                  CRC16(FIB(5,:)) CRC16(FIB(6,:))  CRC16(FIB(7,:))  CRC16(FIB(8,:)) ...
      %                  CRC16(FIB(9,:)) CRC16(FIB(10,:)) CRC16(FIB(11,:)) CRC16(FIB(12,:)) ];
        FIBCRCCheck = zeros(1,NFIBsPerDABFrame);
        for k=1:NFIBsPerDABFrame
            FIBCRCCheck(k) = CRC16(FIB(k,:));
        end    
        if(DebugOn==1) FIBCRCCheckSize = size(FIBCRCCheck), pause, end
        
        if sum(FIBCRCCheck) == NFIBsPerDABFrame % 12/3/4/6 for Mode=1/2/3/4
             disp('CRC OK');                    % FIGs building from FIBS 
             for FIBNr = 1:NFIBsPerDABFrame     % to 12/3/4/6 for Mode=1/2/3/4
                 pos = 1;
                 while pos < 241
                     if FIB(FIBNr,pos:pos+7) == [1 1 1 1 1 1 1 1], break, end
                       % FIG type and length finding
                         Type = BinToDec( FIB(FIBNr,pos:pos+2), 3 );
                         FIGDataLength = BinToDec( FIB(FIBNr,pos+3:pos+7), 5 );
                       % FIG building (reconstruction)
                         FIGType( FrameNr, FIBNr, Type, FIGDataLength, FIB(FIBNr,pos+8:pos+8*(FIGDataLength+1)-1) )
                         pos = pos + (FIGDataLength+1)*8;
                  end
             end
             disp('################################## Ready !');
             disp('PRESS ANY KEY !'); pause

        else
             disp(['CRC Fail! Frame: ',num2str(FrameNr)]);
        end
        
        close all;  % figures
        
  end



