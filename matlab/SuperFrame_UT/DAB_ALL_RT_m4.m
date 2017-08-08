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

clc; clear all; close all; fclose('all');

global NSymbPerFrame NSampPerFrame NSampPerSymb NSampPerNull NCarrPerSymb Nfft NSampPerGuard PhaseRefSymb sigPhaseRefSymb fs df_fract df_int df FrameStartpoints FrameLengths
global DABFrameFoundNr FrameStartpoints FrameLengths NSampPerFrameADC

% NEW! After division the program into two functions: decodeFIC(), decodeMSC()
global NSymbPerFrame NCarrPerSymb
global NFIBsPerFrame NCIFsPerFrame NSymbPerFIC  NFIBsPerCIF
global L1Range L2Range L3Range L4Range PI1 PI2 PI3 PI4 Padding
global DispSequense
global DataLastRound
global CIFCount SuperFrameSync DataEnergChain au_start num_aus lan fid

% PROGRAM CONTROL / STEARING PARAMETERS !!!

% Testing four DAB modes - at present do not change
  TestMode = 1;         % 0/1 testing different DAB modes; 0 --> only Mode=1 (DAB+ Krakow)
  Mode = 4;             % 1/2/3/4: FOR TestMode=1 ONLY; otherwise Mode=1 

% Testing DAB+ only (e.g. DAB+ Krakow)  
  if(TestMode==0) Mode=1; end
                        %
  PlotON = 0;           % 0/1 displaying figures 
  DebugON = 0;          % 0/1 displaying partial results: values and vector/matrix dimensions
  DetectModeON = 0;     % 0/1 example of DAB mode detection
  DetectFICON =  0;     % 0/1 decoding FIC information about available services
  DetectMSCON =  1;     % 0/1 decoding MSC content of radio audio service
 
                           % DO NOT CHANGE !  
                           % For function DetFrameStartStream_RT() doing Time Synchro and Delta Freq correction 
  CorrectTimeSynchro = 0;  % 0/1 TRY TO CORRECT time synchro using known DF (delta freq)
  CorrectDeltaFreq   = 0;  % 0/1 TRY TO CORRECT df for the second time after improving Time Synchro
  CorrectADC         = 1;  % 0/1 correct sampling frequency by signal resampling 

                        % DO NOT CHANGE !  
  SuperFrame2AAC = 1;   % 1 = DAB+ to AAC transcoding in this program, 0 = outside by DABp2AAC.exe
  lan = 0;              % at present not used
  
% EXAMPLE $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
% Examplary calculations are done for the following example:
% DAB, Mode=1; SubChStartAdr = 188;  SubChSize = 84;  Audiokbps = 128;  Protection = 4;  UEP = true;
% EXAMPLE $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

% ####################################################################################
% Information about available services is read from FIC (Fast Information Channel)
% by DAB_FIC.m program (or DAB_ALL_RT.m with DetectFICON = 1)
% and written into DAB_file_info.m script
% ####################################################################################

% CHOOSE DAB_IQ input file to be decoded, read its sampling freq, signal type, ... etc.
  
  DAB_file_info  % execute this script! or use Example #1 or #2

% Example #1: PR Katowice without "noise" 
% ReadFile = fopen('Record3_katowice_iq.dat', 'rb'); fs_file = 2.048e6;  sigtype=1; fi=0; coef = 0;
% SubChStartAdr = 0;  SubChSize = 84; Audiokbps = 0; Protection = ['3A'];  UEP=false;
  
% Example #2: PR Krak�w with "noise"
% ReadFile = 'SDRSharp_20150102_125222Z_229077kHz_IQ.wav'; fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% SubChStartAdr = 516; SubChSize = 78;  Audiokbps = 0;  Protection = ['3A']; UEP=false;

% Example #3: your DAB file
% ReadFile = '?????'; fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% SubChStartAdr = ???; SubChSize = ???;  Audiokbps = 0;  Protection = ['??']; UEP=false;

% #############################
% Example of DAB Mode detection
% #############################

  if( DetectModeON )  % Mode = ?
      tic
      NSamples = 2^19;
    % Load IQ file
      [ Signal ] = DataLoad_RT( ReadFile, NSamples, fs_file, fs, sigtype );
    % Detect frame beginings
      FrameStartpoints = DetFrameStart( Signal, PlotON );
      FrameLengths = FrameStartpoints(2:end) - FrameStartpoints(1:end-1),
    % Time in samples between two frames (checking ADC sampling frequency - optional )
      if( PlotON) ADCFreqCheck(FrameStartpoints); end
    % Mode detection - other parameters depends on Mode and are calculated later
      [ Mode, NSampPerFrame, NSampPerNull, NSymbPerFrame, NSampPerSymb, NSampPerGuard, Nfft, NCarrPerSymb, FreqIntTabSize, Offset, NCIFsPerFrame, NSymbPerFIC ] = DetModeX( Signal, FrameStartpoints );
    % The main parameter, other values are based on it and recalculated after (done below) ! 
      toc; disp(' '); disp('Detected DAB Mode = '); Mode, pause  
  end
  
% ###################################################
% Now set vaules of params depending on DAB Mode .... 
% ###################################################

% ---------------------------------------------------
% GENERAL PARAMETERS
% ---------------------------------------------------  

% Mode      I       II     III    IV         % from ETSI EN300401, page 145, Table 38
  Tab38 = [ 76,     76,    153,   76;        % 1, L, the number of OFDM symbols per one TX DAB frame (without NULL symbol)
            1536,   384,   192,   768;       % 2, K, the number of transmitted carriers
            196608, 49152, 49152, 98304;     % 3, Tf, the transmission frame duration (in samples)
            2656,   664,   345,   1328;      % 4, TNULL, the Null symbol duration (in samples)
            2552,   638,   319,   1276;      % 5, Ts, the duration of OFDM symbols of indices: l=1,2,3,...,L
            2048,   512,   256,   1024;      % 6, Tu, the inverse of the carrier spacing
            504,    126,   63,    252 ];     % 7, delta, the duration of guard interval
        
                                             % from ETSI EN300401, page 28, Table 2
  Tab2 =  [ 12,     3,     4,     6;         % 1. number of FIBs per one DAB frame (part of FIC)  
            4,      1,     1,     2;         % 2. number of CIFs per one DAB frame (part of MSC)
            3,      3,     8,     3;         % 3. ADDED by TZ, number of OFDM frames per FIC
            3,      3,     4,     3; ];      % 4. ADDED by TZ, number of FIBs per one CIF

  NSymbPerFrame = Tab38(1,Mode);     % 76      % L, the number of OFDM symbols per one DAB frame (without NULL symbol)                        
  NCarrPerSymb  = Tab38(2,Mode);     % 1536    % K, number of transmitted carriers
  NSampPerFrame = Tab38(3,Mode);     % 196608  % Tf, the transmission frame duration (in samples)
  NSampPerNull  = Tab38(4,Mode);     % 2656    % TNULL, the Null symbol duration (in samples)
  NSampPerSymb  = Tab38(5,Mode);     % 2552    % Ts, the duration of OFDM symbols of indices: l=1,2,3,...,L
  Nfft          = Tab38(6,Mode);     % 2048    % Tu, the inverse of the carrier spacing
  NSampPerGuard = Tab38(7,Mode);     % 504     % delta, the duration of guard interval

  NFIBsPerFrame = Tab2(1,Mode);      % 12      % (FIC) number of FIBs per one DAB frame (non-interleaved)
  NCIFsPerFrame = Tab2(2,Mode);      % 4       % (MSC) number of CIFs per one DAB frame (time-interleaved)
  NSymbPerFIC   = Tab2(3,Mode);      % 3       % (FIC) number of OFDM frames per one FIC
  NFIBsPerCIF   = Tab2(4,Mode);      % 3       % (FIC) number of FIBs per one CIF
  
  fs = 2.048e6;                                % sampling frequency used in the DAB algorithm
  ReadSize = 3*NSampPerFrame/4;                % number of samples read into the buffer (in the loop)
  CIFCount = 0;                                % CIF counter on output of Energy Dispersion
  SuperFrameSync = false;                      % is Audio Super Frame synchronised? (false/true)
  AllAudio = [];                               % variable for extracted audiofile

                         % For DAB+ (Audio Super Frame)
  au_start = 0;          % starting position in synchronous access unit
  num_aus = 0;           % number of audiopacketes in one Audio Super Frame

  read = true;           % true = there are still DAB data waiting for processing
  OldData = [];          % DAB data still not-processed, waiting in the data buffer
  DABFrameNr = 0;        % number of a DAB frame that is processed now (starting with 1)

% Protection against Time Synchro Error. For long channel impulse response prefered OFDMOffset = 0 !!!
% Added shift "left" in signal samples in respect to the detected BEGINNING OF OFDM signal WITHOUT CP

  OFDMOffset = round( NSampPerGuard/2 );    % OK for [ 0, NSampPerGuard-1]  
                                            % ERROR for <0

% ---------------------------------------------------
% CALLING FUNCTIONS - table generation
% ---------------------------------------------------  

% Generation of a Pseudo-Random Binary Sequence (PRBS), used in energy de-dispersal (ETSI pp. 128-129)
  DispSequense = EnergyDispGen( 100000 );
  
% Generation of a Frequency Inteleaving and Frequency De-inteleaving Tables (ETSI pp. 157-161)
  [FreqInterleaverTab]   = FreqInterleaverTabGen( Nfft );                       
  [FreqDeinterleaverTab] = FreqDeinterleaverTabGen( FreqInterleaverTab, Nfft );
  
% Generation of Phase Reference Symbol (used for frame synchro) (ETSI pp. 147-149)
  [PhaseRefSymb, sigPhaseRefSymb] = PhaseRefSymbGen( Mode, PlotON );
  
% Generation of polynomial for Trellis (ETSI, page 129-130, below fig. 72)
  trellis = poly2trellis(7,[133 171 145 133]); % 7 = delay of each bitstream
                                               % [133 ... 133] = the octal numbers specifying input-output

% Generation of polynomial for Reed-Solomon decoder being part of DAB+ (ETSI 2010, page 15-16)
  RSpolynomial = rsgenpoly(255,245,285); % 285 = 100011101 = x^8+x^4+x^3+x^2+1
                                               
% ##########################################################################################
% These parameters should be set AFTER! FIC decoding and choosing a broadcast by a listener!
% ##########################################################################################

% Distinguishing between DAB and DAB+ programme!
% Depends on type of a channel to be decoded and played.

  if( UEP == true )
    % DAB: Parameters for Unequal Error Protection (UEP), used in depuncturing, pages 133-135, table 31
      [L1Range, L2Range, L3Range, L4Range, PI1, PI2, PI3, PI4, Padding] = UEPTab(Audiokbps, Protection);
    %    836,    2012,    5184,     5364,  11,  5,   6,   7,   0        % Example  
  else
    % DAB+: Parameters for Equal Error Protection (EEP), used in depuncturing, pages 135-136
      [Audiokbps, PI1, PI2, L1Range, L2Range] = EEPTab(Protection, SubChSize);
      PI3=0; PI4=0; L3Range = 0; L4Range = 0; Padding=0; 
  end

% Buffer for time-interleaved MSC data (ETSI, pp. 137-142)
% buffer used for MSC time interleaving: 16frames, each 4CIFs with 64bits    
  Delay = 4*SubChSize;                                              % Example: SubChSize=84, Delay=336
  DataLastRound = zeros( 16, 16*Delay + (NCIFsPerFrame-1)*Delay );  % Example: NCIFs=4 -> 16 x 6384 

% #################################
% CALCULATION DIMENSION OF MATRICES
% #################################  
  
  if( UEP == true )
      v1 = PIDet( PI1 );                          % puncturing 0/1 32-element vector
      NBits1 = L1Range           * 32/sum(v1);    % bit number after depuncturing
      v2 = PIDet( PI2 );                          % puncturing 0/1 32-element vector
      NBits2 = (L2Range-L1Range) * 32/sum(v2);    % bit number after depuncturing
      v3 = PIDet( PI3 );                          % puncturing 0/1 32-element vector
      NBits3 = (L3Range-L2Range) * 32/sum(v3);    % bit number after depuncturing
      v4 = PIDet( PI4 );                          % puncturing 0/1 32-element vector
      NBits4 = (L4Range-L3Range) * 32/sum(v4);    % bit number after depuncturing
      v5 = PIDet( 8 );                            % puncturing 0/1 32-element vector
      NBits5 = 12                * 32/sum(v5);
      NBitsAfterTimeDepunct = NBits1 + NBits2 + NBits3 + NBits4 + NBits5;
  else
      v1 = PIDet( PI1 );                          % puncturing 0/1 32-element vector
      NBits1 = L1Range           * 32/sum(v1);    % bit number after depuncturing
      v2 = PIDet( PI2 );                          % puncturing 0/1 32-element vector
      NBits2 = (L2Range-L1Range) * 32/sum(v2);    % bit number after depuncturing
      v3 = PIDet( 8 );                            % puncturing 0/1 32-element vector
      NBits3 = 12                * 32/sum(v3);
      NBitsAfterTimeDepunct = NBits1 + NBits2 + NBits3;
  end

% ####################
% ####################
% ####################
% MAIN PROCESSING LOOP
% ####################
% ####################
% ####################

% IMPORTANT
%          Delay = 1 Frame (ADC) + 16 CIFs (Time Interleaving) + 5 CIFs (DAB+ Super Frame)
% IMPORTANT

tic;

disp('Work in progress ... ');

while read == true
  
  % CHECK AMOUNT OF THE DATA IN THE BUFFER   
    [MDAT,NDAT] = size(OldData);  
    if( MDAT < 2*NSampPerFrame)
      % READING NEXT DATA BLOCK FROM A DAB FILE --------------------------------------------------
        disp('RRRRRRRRRRRRRRRRRRRRRRRRRRRR Reading new data from disc ...');
        try    [NewData] = ComplexDataLoad( ReadFile, ReadSize, fs_file, fs, sigtype);  % load new block of input complex IQ data
        catch  read = false; break;                                                     % data are not enough (available)
        end
      % SizeOldData = size(OldData), if( ~isempty(OldData) ) plot(abs(OldData)); pause; end
    else
        NewData = [];
    end
  
  % FRAME SYNCHRONIZATION --------------------------------------------------------------------
    [Signal, OldData ] = DetFrameStartStream_RT( [OldData; NewData], ReadSize, CorrectTimeSynchro, CorrectDeltaFreq, CorrectADC, coef, PlotON);  % frame synchronization
    
    if( DebugON==1 )
        disp('#################### Signal & OldData lengths');
        LengthSignal = length(Signal), %  = 193952, 48488, 48807, 96976
        ERROR = LengthSignal - (NSampPerFrame - NSampPerNull), pause
    end    
  
  % FRAME DECODING ---------------------------------------------------------------------------------
    if( ~isempty(Signal) & (DABFrameFoundNr>1) )  % first frame is without the freq sampling correction                                         % signal proceesing
         
          DABFrameNr = DABFrameNr + 1; % pause
          
        % FFT of the latest symbols and D-QPSK medomulation - in rows results for consecutive OFDM symbols ------
        % ETSI pp. 153-157
          FFTFrame = SymbolFFTStream_RT( Signal, OFDMOffset, NSymbPerFIC, coef, fi, PlotON);
  
          if( DebugON==1 )
              disp('#################### FFTFrame'); FFTFrame(1:5),
              SizeFFTFrame = size(FFTFrame),   % (NSymbPerFrame-1) x NCarrPerSymb
              ERROR = SizeFFTFrame - [(NSymbPerFrame-1), NCarrPerSymb ], pause   
          end    
          
        % Frequency de-interleaving - in rows results for consecutive OFDM symbols ------------------------------
        % ETSI pp. 157-161
          DeintFFTFrame = zeros(size(FFTFrame));
          for r = 1 : length( FreqDeinterleaverTab(:,4) )                               
              DeintFFTFrame( :, FreqDeinterleaverTab(r,4) ) = FFTFrame(:,r);  % Frequency de-interleaving
          end

          if( DebugON==1 )
              disp('#################### DeintFFTFrame'); DeintFFTFrame(1:5),
              SizeDeintFFTFrame = size(DeintFFTFrame),   % (NSymbPerFrame-1) x NCarrPerSymb
              ERROR = SizeDeintFFTFrame - [(NSymbPerFrame-1), NCarrPerSymb ], pause   
          end    
        
        % D-QPSK symbol demapper - from 2D matrix to 1D vector --------------------------------------------------
        % ETSI pp. 157
          DataFIC = zeros(1, NSymbPerFIC*NCarrPerSymb*2);                  % Our example (Mode=1): 3*1536*2
          DataMSC = zeros(1,(NSymbPerFrame-NSymbPerFIC-1)*NCarrPerSymb*2); % Our example (Mode=1): 76-3-1(differential) = 72 Symbols in MSC
          for f = 1 : NSymbPerFIC                  % FIC DATA from OFDM symbols - first real parts, then imag parts 
              DataFIC( 1+(f-1)*NCarrPerSymb*2 : (f)*NCarrPerSymb*2 ) = [real(DeintFFTFrame(f,:)) imag(DeintFFTFrame(f,:))];
          end
          for f = NSymbPerFIC+1 : NSymbPerFrame-1  % MSC DATA from OFDM symbols - first real parts, then imag parts 
              ff = f - NSymbPerFIC;
              DataMSC( 1+(ff-1)*NCarrPerSymb*2 : (ff)*NCarrPerSymb*2 ) = [real(DeintFFTFrame(f,:)) imag(DeintFFTFrame(f,:))];
          end

          if( DebugON==1 )
              disp('#################### DataFIC'); DataFIC(1:5),
              SizeDataFIC = size(DataFIC),         % 1 x 2*NSymbPerFIC*NCarrPerSymb
              ERROR = SizeDataFIC - [1, 2*NSymbPerFIC*NCarrPerSymb], pause
              disp('#################### DataMSC'); DataMSC(1:5),
              SizeDataMSC = size(DataMSC),         % 1 x 2*(NSymbPerFrame-NSymbPerFIC-1)*NCarrPerSymb
              ERROR = SizeDataMSC - [1, 2*(NSymbPerFrame-NSymbPerFIC-1)*NCarrPerSymb], pause       
          end    

        % Decode FIC (Fast Information Channel ) - info about services ------------------------------------------
          if( DetectFICON )
              [ Start, Size, kbps, Protect, ErrProtect ] = decodeFIC( Mode, DABFrameNr, DataFIC, trellis, DebugON );
          end
        % Decode MSC (Main Service Channel ) - music, etc. ------------------------------------------------------  
          if( DetectMSCON )
              decodeMSCm4( DABFrameNr, DataMSC, SubChStartAdr, SubChSize, Audiokbps, Protection, UEP, trellis, RSpolynomial, SuperFrame2AAC, NBitsAfterTimeDepunct, DebugON );
              if (DABFrameNr == 24)
                  return;
              end
          end

          close all;  % figures                   

    end    % end of ~isempty(Signal)
end    % end of while (reading data) 

fprintf('Time ellapsed %3i sec \n',round(toc));

fclose('all');

if lan == 1
   socket.close();
   sSocket.close();
end
