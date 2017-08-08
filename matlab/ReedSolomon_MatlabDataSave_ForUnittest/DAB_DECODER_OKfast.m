
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
global nrDABFrame FrameStartpoints FrameLengths NSampPerFrameADC

Test = 0;             % 0/1 testing bit extraction (only) in different modes on synthezised signals
Mode = 1;             % 1/2/3/4 which transmission mode FOR TESTING ONLY; otherwise 1

PlotOn = 0;           % 0/1 displaying figures 
DebugOn = 0;          % 0/1 displaying partial results

lan = 0;
SuperFrame2AAC = 1;   % 1 = transcoding in this program, 0 = outside by DABp2AAC.exe

% EXAMPLE $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
% Examplary calculations are done for the following example:
% DAB, Mode=1; SubChStartAdr = 188;  SubChSize = 84;  Audiokbps = 128;  Protection = 4;  UEP = true;
% EXAMPLE $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

% DAB_IQ input file to be decoded, sampling freq, signal type

% ####################################################
% ####################################################
% ####################################################
% ####################################################
% ####################################################
% Synthetic test files in different Modes (1, 2, 3, 4) 
% ####################################################
% ####################################################
% ####################################################
% ####################################################
% ####################################################

  if(Test==1)         % Testing bit detection (ONLY!) in different modes - for student labs
      
    % if(Mode==1) ReadFile = fopen('DAB_synt_2.048MHz_IQ_float_Mode1.dat', 'rb');  end % Mode=1  
    % if(Mode==2) ReadFile = fopen('DAB_synt_2.048MHz_IQ_float_Mode2.dat', 'rb');  end % Mode=2
    % if(Mode==3) ReadFile = fopen('DAB_synt_2.048MHz_IQ_float_Mode3.dat', 'rb');  end % Mode=3
    % if(Mode==4) ReadFile = fopen('DAB_synt_2.048MHz_IQ_float_Mode4.dat', 'rb');  end % Mode=4
    % fs_file = 2.048e6; sigtype=1; fi=-pi/4; coef=0.9;
    % SubChStartAdr = 84;  SubChSize = 48; Audiokbps =  64; Protection = 4; UEP = true; % Noname
    
      if(Mode==1) % London Mode=1
          ReadFile = fopen('London_mode1_iq.dat', 'rb');
       %  SubChStartAdr =  0;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=1 DAB
       %  SubChStartAdr =605;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=2 DAB
       %  SubChStartAdr =288;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=4 DAB
          SubChStartAdr = 96;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=5 DAB
       end   
      if(Mode==2) % London Mode=2
        % ReadFile = fopen('London_mode2_iq.dat', 'rb');
        % SubChStartAdr =  0;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=1 DAB
        % SubChStartAdr =605;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=2 DAB
        % SubChStartAdr =288;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=4 DAB
        % SubChStartAdr = 96;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=5 DAB
        
          ReadFile = fopen('Kielce_mode2_iq.dat', 'rb');
          SubChStartAdr =    0;  SubChSize = 72; Audiokbps = 0;   Protection = ['3A']; UEP = false;  % SubChID=1 DAB+
        % SubChStartAdr =   72;  SubChSize = 78; Audiokbps = 0;   Protection = ['3A']; UEP = false;  % SubChID=2 DAB+
      end
      if(Mode==3) % London Mode=3
        % ReadFile = fopen('London_mode3_iq.dat', 'rb');
        % SubChStartAdr =    0;  SubChSize = 72; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=1 DAB+
        % SubChStartAdr =  308;  SubChSize = 72; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=4 DAB+
        % SubChStartAdr =   72;  SubChSize =140; Audiokbps = 128; Protection = 1;      UEP = true;  % SubChID=2 DAB 
        % SubChStartAdr =  212;  SubChSize = 96; Audiokbps = 128; Protection = 3;      UEP = true;  % SubChID=3 DAB
      
          ReadFile = fopen('London_mode3p_iq.dat', 'rb');
        % SubChStartAdr =   64;  SubChSize = 108; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=5 DAB+
        % SubChStartAdr =  172;  SubChSize = 108; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=6 DAB+
          SubChStartAdr =  280;  SubChSize = 108; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=7 DAB+
      end
      if(Mode==4) % London Mode=4
        % ReadFile = fopen('London_mode4_iq.dat', 'rb');
        % SubChStartAdr =  0;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=1 DAB
        % SubChStartAdr =605;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=2 DAB
        % SubChStartAdr =288;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=4 DAB
        % SubChStartAdr = 96;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=5 DAB
        
          ReadFile = fopen('Kielce_mode4_iq.dat', 'rb');
          SubChStartAdr =  0;   SubChSize = 72; Audiokbps = 0; Protection = ['3A']; UEP = false;  % SubChID=1 DAB+
        % SubChStartAdr =  72;  SubChSize = 78; Audiokbps = 0; Protection = ['3A']; UEP = false;  % SubChID=2 DAB+

      end
      fs_file = 2.048e6; sigtype=1; fi=0; coef=0.9;

  else
      
      Mode = 1 ;      % FOR REAL REACORDINGS !!!!!
      
  end
    
% #########################
% USRP Ettus Research float 
% #########################

% <------------------------------ SWISS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
% RECORD1_SWISS AND RECORD2_SWISS
% ReadFile = fopen('Record1.dat', 'rb');   fs_file = 2.0e6;   sigtype=1;   fi=0;   coef = 0.9;
% ReadFile = fopen('Record2.dat', 'rb');   fs_file = 2.0e6;   sigtype=1;   fi=0;   coef = 0.9;

% OK decoding
% 1) not very noisy
% 2) small IF freq drift
% 3) small ADC freq drift
% 4) every second NULL zone is more oscilatory but synchronization is not lost

% <------------------------------ KATOWICE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
% RECORD3_KATOWICE
  ReadFile = fopen('Record3_katowice_iq.dat', 'rb'); fs_file = 2.048e6;  sigtype=1;  fi=0;  coef = 0;
% OK everything
% Constellation = perfect square

% #########################
% USRP Ettus Research int16
% #########################
% TO BIG FREQUENCY DRIFT

% ReadFile = fopen('dab181-936MHz_at2MHz_cplx_short.dat', 'rb');         fs_file = 2e6;      sigtype=2;  fi=0;  coef=0;
% t = fread( ReadFile, [2, 50000], 'int16' ); % only for dab181-936MHz_at2MHz_cplx_short.dat

% ReadFile = fopen('dab223-936MHz_at2MHz_cplx_short.dat', 'rb');         fs_file = 2e6;      sigtype=2;  fi=0;  coef=0;

% ##########################
% SDRSharp USB Stick from TT
% ##########################

% SDRSharp DAB_181MHz TT <------------------------------------ 
% ReadFile = 'SDRSharp_20140728_133001Z_181930kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi= 0/180*pi;   coef=0.9; % -75,  -15
% SDRSharp DAB_223MHz TT <------------------------------------
% ReadFile = 'SDRSharp_20140728_132615Z_223930kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi= 0/180*pi;   coef=0.9; % -35,  -15
% SDRSharp DAB_227MHz TT <------------------------------------ 
% ReadFile = 'SDRSharp_20140728_132746Z_227360kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi= 0/180*pi;   coef=0.9; % -115, -15 stable f, big noise 
% Problems:
% 1) very noisy
% 2) big IF freq drift
% 3) big ADC freq drift

% USB STICK DAB/FM - TZ Kraków                                                                                            % Without ADC correct 
% ReadFile = 'SDRSharp_20150102_110331Z_229277kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150102_111436Z_229277kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150102_123014Z_229297kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150102_205931Z_229279kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% ReadFile = 'DAB_Krakow_20150102_IQ_3secs.wav';            fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;

% DAB KRAKÓW TZ <------------------------------------ 
% ReadFile = 'SDRSharp_20150102_125222Z_229077kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150102_125438Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150102_185516Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150102_190532Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% ReadFile = 'SDRSharp_20150103_104431Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% ReadFile = 'SDRSharp_20150103_105533Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% ReadFile = 'SDRSharp_20150103_110445Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; 
% ReadFile = 'SDRSharp_20150103_192637Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150103_192918Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150103_194107Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;
% ReadFile = 'SDRSharp_20150103_194351Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9;

% DAB KRAKÓW JB <------------------------------------
% ReadFile = fopen('antena_3_dab_229072kHz_fs2048kHz_gain44_BAD.raw', 'rb'); fs_file=2.048e6; sigtype=4; fi=0; coef=0.9;
% ReadFile = fopen('antena_3_dab_229072kHz_fs2048kHz_gain44_OK.raw', 'rb');  fs_file=2.048e6; sigtype=4; fi=0; coef=0.9;

% ##################################################### 
% fid = fopen('AudioX.mp2', 'w');    % output audio file
  fid = fopen('AudioX.aac', 'w');    % output audio file
% #####################################################

% #########################################################################################
% #########################################################################################
% #########################################################################################
% #########################################################################################
% #########################################################################################
% Sender information written from FIC (fast information channel) done by DAB_FIC.m program
% #########################################################################################
% #########################################################################################
% #########################################################################################
% #########################################################################################

% Record1.dat ###################################################################################################
% Mainly DAB

if(0) SubChStartAdr = 188;  SubChSize = 84;  Audiokbps = 128;  Protection = 4;  UEP = true; end  % DAB   A. DRS 3
if(0) SubChStartAdr = 272;  SubChSize = 96;  Audiokbps = 128;  Protection = 3;  UEP = true; end  % DAB   B. DRS Musikwelle
if(0) SubChStartAdr = 368;  SubChSize = 96;  Audiokbps = 128;  Protection = 3;  UEP = true; end  % DAB   C. DRS Virus
if(0) SubChStartAdr = 464;  SubChSize = 84;  Audiokbps = 128;  Protection = 4;  UEP = true; end  % DAB   D. chosen by TZ
if(0) SubChStartAdr = 548;  SubChSize = 84;  Audiokbps = 128;  Protection = 4;  UEP = true; end  % DAB   E. CH Classic
if(0) SubChStartAdr = 785;  SubChSize = 42;  Audiokbps = 56;   Protection = 3;  UEP = true; end  % DAB   F. DRS 4 News 

if(0) SubChStartAdr = 702;  SubChSize = 24;  Audiokbps = 0;    Protection = ['3A'];  UEP=false;  end     % DAB+
if(0) SubChStartAdr = 726;  SubChSize = 24;  Audiokbps = 0;    Protection = ['3A'];  UEP=false;  end     % DAB+
if(0) SubChStartAdr = 827;  SubChSize = 24;  Audiokbps = 0;    Protection = ['3A'];  UEP=false;  end     % DAB+

% Record2.dat #######################################################################################################
% Only DAB+. Difference only in SubChStartAdr!

if(0) % DAB+
   SubChStartAdr = 96;                  % 0, 48, 96, 144, 192, 240, 288, 336, 384, 432, 528, 576, 624      
   SubChSize = 48;
   Audiokbps = 0;
   Protection = ['3A'];
   UEP=false;
end

% Record3.dat KATOWICE #############################################################################################
% Only DAB+

if(1) SubChStartAdr = 0;    SubChSize = 84;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #1
if(0) SubChStartAdr = 84;   SubChSize = 96;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #2
if(0) SubChStartAdr = 180;  SubChSize = 84;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #3
if(0) SubChStartAdr = 264;  SubChSize = 84;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #4
if(0) SubChStartAdr = 348;  SubChSize = 48;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #5
if(0) SubChStartAdr = 396;  SubChSize = 48;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #6
if(0) SubChStartAdr = 444;  SubChSize = 72;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #7 BAD jumps
if(0) SubChStartAdr = 516;  SubChSize = 78;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #9

% USB DAB 181 MHz ##################################################################################################
% mainly DAB

if(0) SubChStartAdr = 846; SubChSize = 12;  Audiokbps = 0;   Protection = ['3A'];  UEP=false; end % Channel #62 DAB+ (only one)

if(0) SubChStartAdr = 0;   SubChSize = 96;  Audiokbps = 128; Protection = 3;       UEP=true;  end % Channel #1  DAB
if(0) SubChStartAdr = 96;  SubChSize = 96;  Audiokbps = 128; Protection = 3;       UEP=true;  end % Channel #2  DAB 
if(0) SubChStartAdr = 192; SubChSize = 116; Audiokbps = 160; Protection = 3;       UEP=true;  end % Channel #3  DAB 
if(0) SubChStartAdr = 308; SubChSize = 96;  Audiokbps = 128; Protection = 3;       UEP=true;  end % Channel #4  DAB 
if(0) SubChStartAdr = 404; SubChSize = 48;  Audiokbps = 64;  Protection = 3;       UEP=true;  end % Channel #5  DAB 
if(0) SubChStartAdr = 452; SubChSize = 48;  Audiokbps = 64;  Protection = 3;       UEP=true;  end % Channel #7  DAB 
if(0) SubChStartAdr = 500; SubChSize = 48;  Audiokbps = 64;  Protection = 3;       UEP=true;  end % Channel #8  DAB 
if(0) SubChStartAdr = 548; SubChSize = 48;  Audiokbps = 64;  Protection = 3;       UEP=true;  end % Channel #9  DAB 
if(0) SubChStartAdr = 596; SubChSize = 96;  Audiokbps = 128; Protection = 3;       UEP=true;  end % Channel #10 DAB 
if(0) SubChStartAdr = 692; SubChSize = 96;  Audiokbps = 128; Protection = 3;       UEP=true;  end % Channel #11 DAB 
if(0) SubChStartAdr = 788; SubChSize = 58;  Audiokbps = 80;  Protection = 3;       UEP=true;  end % Channel #12 DAB 

% USB DAB 223 MHz #################################################################################################
% mainly DAB

if(0) SubChStartAdr = 0;   SubChSize = 48;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #11 DAB+ (only one)

if(0) SubChStartAdr = 48;  SubChSize = 96;  Audiokbps = 128;  Protection = 3;      UEP=true;  end % Channel #15 DAB 
if(0) SubChStartAdr = 144; SubChSize = 96;  Audiokbps = 128;  Protection = 3;      UEP=true;  end % Channel #16 DAB 
if(0) SubChStartAdr = 240; SubChSize = 140; Audiokbps = 192;  Protection = 3;      UEP=true;  end % Channel #17 DAB 
if(0) SubChStartAdr = 380; SubChSize = 70;  Audiokbps = 96;   Protection = 3;      UEP=true;  end % Channel #18 DAB 
if(0) SubChStartAdr = 450; SubChSize = 48;  Audiokbps = 64;   Protection = 3;      UEP=true;  end % Channel #19 DAB 
if(0) SubChStartAdr = 498; SubChSize = 96;  Audiokbps = 128;  Protection = 3;      UEP=true;  end % Channel #20 DAB 

% USB DAB 227 MHz ##################################################################################################
% mainly DAB+

if(0) SubChStartAdr = 608; SubChSize = 60;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #8  DAB+ OK pocz¹tek
if(0) SubChStartAdr = 668; SubChSize = 60;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #9  DAB+ OK ca³y
if(0) SubChStartAdr = 728; SubChSize = 60;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #10 DAB+ Ok œwisty
if(0) SubChStartAdr = 788; SubChSize = 12;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #13 DAB+
if(0) SubChStartAdr = 800; SubChSize = 12;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #14 DAB+
if(0) SubChStartAdr = 812; SubChSize = 12;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #17 DAB+
if(0) SubChStartAdr = 824; SubChSize = 12;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #18 DAB+

if(0) SubChStartAdr = 0;   SubChSize = 104; Audiokbps = 160;  Protection = 4;      UEP=true;  end % Channel #1 DAB
if(0) SubChStartAdr = 104; SubChSize = 84;  Audiokbps = 128;  Protection = 4;      UEP=true;  end % Channel #2 DAB
if(0) SubChStartAdr = 188; SubChSize = 116; Audiokbps = 192;  Protection = 4;      UEP=true;  end % Channel #3 DAB
if(0) SubChStartAdr = 304; SubChSize = 104; Audiokbps = 160;  Protection = 4;      UEP=true;  end % Channel #4 DAB
if(0) SubChStartAdr = 408; SubChSize = 104; Audiokbps = 160;  Protection = 4;      UEP=true;  end % Channel #5 DAB
if(0) SubChStartAdr = 512; SubChSize = 48;  Audiokbps = 64;   Protection = 3;      UEP=true;  end % Channel #6 DAB
if(0) SubChStartAdr = 560; SubChSize = 48;  Audiokbps = 64;   Protection = 3;      UEP=true;  end % Channel #7 DAB

% #################################################################################################################
% DAB+ Kraków

if(0) SubChStartAdr =   0; SubChSize = 84;  Audiokbps = 0;    Protection = ['3A']; UEP=false;  end % Channel #7 DAB+
if(0) SubChStartAdr =  84; SubChSize = 96;  Audiokbps = 0;    Protection = ['3A']; UEP=false;  end % Channel #7 DAB+
if(0) SubChStartAdr = 180; SubChSize = 84;  Audiokbps = 0;    Protection = ['3A']; UEP=false;  end % Channel #7 DAB+
if(0) SubChStartAdr = 264; SubChSize = 84;  Audiokbps = 0;    Protection = ['3A']; UEP=false;  end % Channel #7 DAB+
if(0) SubChStartAdr = 348; SubChSize = 48;  Audiokbps = 0;    Protection = ['3A']; UEP=false;  end % Channel #7 DAB+
if(0) SubChStartAdr = 396; SubChSize = 48;  Audiokbps = 0;    Protection = ['3A']; UEP=false;  end % Channel #7 DAB+
if(0) SubChStartAdr = 444; SubChSize = 72;  Audiokbps = 0;    Protection = ['3A']; UEP=false;  end % Channel #7 DAB+
if(0) SubChStartAdr = 516; SubChSize = 78;  Audiokbps = 0;    Protection = ['3A']; UEP=false;  end % Channel #7 DAB+

% SubChID:  1   StartAdr:   0   EEP   ProtLevel: 3-A   SubChSize:  84   % PR #1
% SubChID:  2   StartAdr:  84   EEP   ProtLevel: 3-A   SubChSize:  96   % PR #2
% SubChID:  3   StartAdr: 180   EEP   ProtLevel: 3-A   SubChSize:  84   % PR #3
% SubChID:  4   StartAdr: 264   EEP   ProtLevel: 3-A   SubChSize:  84   % PR #4
% SubChID:  5   StartAdr: 348   EEP   ProtLevel: 3-A   SubChSize:  48   % PR Polonia
% SubChID:  6   StartAdr: 396   EEP   ProtLevel: 3-A   SubChSize:  48   % PR 24
% SubChID:  7   StartAdr: 444   EEP   ProtLevel: 3-A   SubChSize:  72   % Radio Rytm
% SubChID:  8   StartAdr: 516   EEP   ProtLevel: 3-A   SubChSize:  78   % Radio Kraków

% ------------------------------------------------
% GENERAL PARAMETERS
% ------------------------------------------------  

% Mode      I       II     III    IV             % from ETSI EN300401, page 145, Table 38
  Tab38 = [ 76,     76,    153,   76;            % 1, L, the number of OFDM symbols per one TX DAB frame (without NULL symbol)
            1536,   384,   192,   768;           % 2, K, the number of transmitted carriers
            196608, 49152, 49152, 98304;         % 3, Tf, the transmission frame duration (in samples)
            2656,   664,   345,   1328;          % 4, TNULL, the Null symbol duration (in samples)
            2552,   638,   319,   1276;          % 5, Ts, the duration of OFDM symbols of indices: l=1,2,3,...,L
            2048,   512,   256,   1024;          % 6, Tu, the inverse of the carrier spacing
            504,    126,   63,    252 ];         % 7, delta, the duration of guard interval
        
                                                 % from ETSI EN300401, page 28, Table 2
  Tab2 =  [ 12,     3,     4,     6;             % 1. number of FIBs per one DAB frame (part of FIC)  
            4,      1,     1,     2  ];          % 2, number of CIFs per one DAB frame (part of MSC)
            
  TabTZ = [ 3,      3,     8,     3; ];          % 9, ADDED, number of OFDM frames per FIC

NSymbPerFrame = Tab38(1,Mode);     % 76          % L, the number of OFDM symbols per one DAB frame (without NULL symbol)                        
NCarrPerSymb  = Tab38(2,Mode);     % 1536        % K, number of transmitted carriers
NSampPerFrame = Tab38(3,Mode);     % 196608      % Tf, the transmission frame duration (in samples)
NSampPerNull  = Tab38(4,Mode);     % 2656        % TNULL, the Null symbol duration (in samples)
NSampPerSymb  = Tab38(5,Mode);     % 2552        % Ts, the duration of OFDM symbols of indices: l=1,2,3,...,L
Nfft          = Tab38(6,Mode);     % 2048        % Tu, the inverse of the carrier spacing
NSampPerGuard = Tab38(7,Mode);     % 504         % delta, the duration of guard interval

NFIBsPerFrame = Tab2(1,Mode);      % 12    % (FIC) number of FIBs per one DAB frame (non-interleaved)
NCIFsPerFrame = Tab2(2,Mode);      % 4     % (MSC) number of CIFs per one DAB frame (time-interleaved)
NSymbPerFIC   = TabTZ(1,Mode);     % 3     % (FIC) number of OFDM frames per one FIC

if UEP == true
  % DAB: Parameters for Unequal Error Protection (UEP), used in depuncturing, pages 133-135, table 31
    [L1Range, L2Range, L3Range, L4Range, PI1, PI2, PI3, PI4, Padding] = UEPTab(Audiokbps, Protection);
  %    836,    2012,    5184,     5364,  11,  5,   6,   7,   0        % Example  
else
  % DAB+: Parameters for Equal Error Protection (EEP), used in depuncturing, pages 135-136
    [Audiokbps, PI1, PI2, L1Range, L2Range] = EEPTab(Protection, SubChSize);
end

fs = 2.048e6;                                               % sampling frequency used in the DAB algorithm
ReadSize = 3*NSampPerFrame/4;                               % number of samples read into the buffer (in the loop)
DataLastRound = zeros( 16, 4*SubChSize*64 );                % buffer used for MSC time interleaving: 16frames, each 4CIFs with 64bits    
DabFrameCount = 0;                                          % frame counter on output of Energy Dispersion
SuperFrameSync = false;                                     % is Audio Super Frame synchronised? (false/true)
AllAudio = [];                                              % variable for extracted audiofile

DispSequense = EnergyDispGen( 100000 );                     % creating pseudo-random binary sequence (PRBS), used in energy de-dispersal

[FreqInterleaverTab]   = BuildFreqInterleaverTab( Nfft );                       % creating Frequency Inteleaving Table
[FreqDeinterleaverTab] = BuildFreqDeInterleaverTab( FreqInterleaverTab, Nfft ); % creating Frequency De-inteleaving Table

%[PhaseRefSymb, sigPhaseRefSymb] = PhaseRefSymbGen( Mode ); % generation of phase reference symbol (used for frame synchro)
 [PhaseRefSymb, sigPhaseRefSymb] = PhaseRefSymbGenX( Mode, PlotOn );

                                                            % polynomial generation for Trellis (page 130, below fig. 72
trellis = poly2trellis(7,[133 171 145 133]);                % 7 = delay of each bitstream
                                                            % [133 ... 133] = the octal numbers specifying input-output

                           % For DAB+ (Audio Super Frame)
au_start = 0;              % starting position in synchronous access unit
num_aus = 0;               % number of audiopacketes in one Audio Super Frame

read = true;               % true = there are still DAB data waiting for processing
OldData = [];              % data not-processed, still waiting in data stream
FrameNr = 0;               % number of DAB frames processed already

OFDMOffset = round( NSampPerGuard/2 );  % added shift "left" in signal samples in respect to the detected BEGINNING OF OFDM signal WITHOUT CP
                                        % OK for [ 0, NSampPerGuard-1]
                                        % ERROR for <0
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
    [Signal, OldData ] = DetFrameStartStreamTZ3e( [OldData; NewData], ReadSize, coef, PlotOn);  % frame synchronization
  % disp('##### Signal & OldData lengths'); length(Signal), length(OldData), pause
  
  % FRAME DECODING ---------------------------------------------------------------------------
    if( ~isempty(Signal) & (nrDABFrame>1) )                                          % signal proceesing
        % close all;
          
          FrameNr = FrameNr + 1; % pause
          
        % FFT of the latest symbols and D-QPSK medomulation - in rows results for consecutive OFDM symbols ------
          FFTFrame = SymbolFFTStreamTZ3e( Signal, OFDMOffset, NSymbPerFIC, coef, fi, PlotOn);
          if( DebugOn==1 )
              disp('##### FFTFrame'); FFTFrame(1:5),
              SizeFFTFrame = size(FFTFrame), pause
          end    
          
        % Frequency de-interleaving - in rows results for consecutive OFDM symbols ------------------------------
          DeintFFTFrame = zeros(size(FFTFrame));
          for r = 1 : length( FreqDeinterleaverTab(:,4) )                               
              DeintFFTFrame( :, FreqDeinterleaverTab(r,4) ) = FFTFrame(:,r);  % Frequency de-interleaving
          end  
          if( DebugOn==1 )
              disp('##### DeintFFTFrame'); DeintFFTFrame(1:5),
              SizeDeintFFTFrame = size(DeintFFTFrame), pause
          end    
        
        % D-QPSK symbol demapper - from 2D matrix to 1D vector --------------------------------------------------
          Data = zeros(1,(NSymbPerFrame-NSymbPerFIC-1)*NCarrPerSymb*2); % Our example (Mode=1): 76-3-1(differential) = 72 Symbols in MSC
          for f = 1 : NSymbPerFrame-NSymbPerFIC-1  % for consecutive OFDM symbols - first real parts, then imag parts 
              Data( 1+(f-1)*NCarrPerSymb*2 : (f)*NCarrPerSymb*2 ) = [real(DeintFFTFrame(f,:)) imag(DeintFFTFrame(f,:))];
          end
          if( DebugOn==1 )
              disp('##### Data'); Data(1:5),
              SizeData = size(Data), pause
          end    

        % Building convolutional codeword for each CIF (NCIFsPerFrame=4/1/1/2 for Mode=1/2/3/4)
          NData = length(Data); NCIF=55296;      % Ndata = (NSymbPerFrame-NCIFsPerFrame-1) * NCarrPerSymb*2bits = 221184 bits (for Mode=1)
          MSC = zeros( NCIFsPerFrame, NCIF );    % NCIF = 864 * 64bits (1CU) = 55296 bits = one CIF = 24 msec        
          for r = 0 : (NCIFsPerFrame-1)          % put CIF codewords in rows; for Mode 1/2/3/4 = 4/1/1/2 CIFs (rows) 
              MSC(r+1, :) = Data( 1 + r*NCIF : NCIF + r*NCIF );
          end
          if( DebugOn==1 )
              disp('##### MSC'); MSC(1,1:5),
              SizeMSC = size(MSC), pause
          end    
          
        % Extracting data of one sub-channel requested by the user (CU=64bits = the smallest addressable unit)----
          SubChData(1:NCIFsPerFrame, :) = MSC(:, 64*SubChStartAdr+1 : 64*SubChStartAdr + 64*SubChSize ); % from 4/1/1/2 CIFs
          if( DebugOn==1 )
              disp('##### SubChData'); SubChData(1,1:5),
              SizeSubChData = size(SubChData), pause
          end
        % Here we have a matrix with dimension NCIFsPerFrame x N SubChan bits in one CIF

        % Time de-interleaving: output = rows: NCIFsPerFrame, columns: SubChSize*64bits(1CU) = 84*64 = 5376 (our example) 
          [TiDeIntSubChData, DataLastRound] = TimeDeInterleavingStream( SubChData, SubChSize, DataLastRound);
          if( DebugOn==1 )
              disp('##### TiDeIntSubChData'); TiDeIntSubChData(1,1:5),
              SizeTiDeIntSubChData = size(TiDeIntSubChData), pause
          end

        % -----------------------------------------------------------------------------------------
        % DAB or DAB+
        % -----------------------------------------------------------------------------------------
        
        if UEP == true % DAB
           
           % (De)puncturing: different parts of CIFs are protected in different manner -------------------------------------
           % Output = rows: NCIFsPerFrame, cols = 12312 (our example) WHY? GENERAL RULE?
              DataDep = [ depuncturing( TiDeIntSubChData(:,1:L1Range),            PI1 )...  % part 1: 4 rows x  836 cols
                          depuncturing( TiDeIntSubChData(:,L1Range+1:L2Range),    PI2 )...  % part 2: 4 x 1176 =837...2012
                          depuncturing( TiDeIntSubChData(:,L2Range+1:L3Range),    PI3 )...  % part 3: 4 x 3172 =2013...5184
                          depuncturing( TiDeIntSubChData(:,L3Range+1:L4Range),    PI4 )...  % part 4: 4 x 180  =5185...5364
                          depuncturing( TiDeIntSubChData(:,L4Range+1:L4Range+12), 8   ) ];  % tail:   4 x 12   =5365...5376
             if( DebugOn==1 )
                 disp('##### DataDep'); DataDep(1,1:5),
                 SizeDataDep = size(DataDep), pause
             end

           % Viterbi decoder, CIF after CIF, removing 4x redundancy: (xn0,xn1,xn2,xn3) (punctured) --> xn -----------------
             DataVit = zeros(length(DataDep(:,1)),length(DataDep(1,:))/4+1); % rows=NCIFsPerFrame, cols/4 - 4x stream reduction
             for f=1:length(DataDep(:,1)) % each CIF
                 DataVit(f,:) = vitdec( [DataDep(f,:) 0 0 0 0], trellis, 1, 'cont', 'unquant');  % each 4 samples--> 1 sample 
             end
             DataVit = DataVit(:, 2:end-6);      % tail removing (6 last numbers); in our example: 12312/4-6=3078-6=3072 columns
             if( DebugOn==1 )
                 disp('##### DataVit'); DataVit(1,1:5),
                 SizeDataVit = size(DataVit), pause
             end
           
           % Energy de-dispersal(XOR with generated PRBS sequence); for our example input has NCIFsPerFrame=4 rows, 3072 columns
             DataEnerg = zeros(size(DataVit));
             for m=1:length(DataEnerg(:,1))
                 DataEnerg(m,:) = xor( DataVit(m,:), DispSequense(1:length(DataEnerg(1,:))) );    % 
             end
             if( DebugOn==1 )
                 disp('##### DataEnerg'); DataEnerg(1,1:5),
                 SizeDataEnerg = size(DataEnerg), pause
             end
             
           % Audio data creation (from bits to bytes): scan by rows (CIF by CIF), write bits into 8-element rows,
           % inner product with vertical weights 2^p, tranpose to horizontal orientation
             Audio=( reshape( DataEnerg',8,[] )' * [128 64 32 16 8 4 2 1]' )';  % 1536 8-bit numbers = NCIFsPerFrame(4)*3072 bits / 8 bits
             if( DebugOn==1 ) disp('##### Audio'); SizeAudio = size(Audio), pause, end
             
           % Write to a file (bytes to a file) --------------------------------------------------------
           % if FrameNr >= 6
                disp('WWWWWWWWWWWWWWWWWWWWWWWWWWWW Audio MP2 written to HD!');
                fwrite(fid,Audio, 'uint8');
           % end
             
        else % DAB+
        
          % (De)puncturing ----------------------------------------------------------------------------
                                                                                     % SubChSize =     24    48    96    84    72    76               
                                                                                                % 4x  1536  3072  6144  5376  4608  4992
            DataDep = [ depuncturing( TiDeIntSubChData(:,1:L1Range),            PI1)...         % 4x  1344  2880  5952  5184  4416  4800
                        depuncturing( TiDeIntSubChData(:,L1Range+1:L2Range),    PI2)...         % 4x   180   180   180   180   180   180 
                        depuncturing( TiDeIntSubChData(:,L2Range+1:L2Range+12), 8) ];  % Tail   % 4x    12    12    12    12    12    12
            if( DebugOn==1 )
                disp('##### DataDep'); DataDep(1,1:5),
                SizeDataDep = size(DataDep), pause
            end  
            
          % Viterbi Decoder ---------------------------------------------------------------------------
            DataVit = zeros(length(DataDep(:,1)),length(DataDep(1,:))/4+1); % Audio vector + Tail (6) + Matlab Viterbi (1)
            for f=1:length(DataDep(:,1))
                DataVit(f,:) = vitdec([DataDep(f,:) 0 0 0 0], trellis, 1, 'cont','unquant');
            end
            DataVit = DataVit(:,2:end-6);      % Tail removing
            if( DebugOn==1 )
                disp('##### DataVit'); DataVit(1,1:5),
                SizeDataVit = size( DataVit ), pause
            end    
            
          % Energy dispersal ------------------------------------------------------------------------
            DataEnerg = zeros(size(DataVit));
            for m=1:length(DataEnerg(:,1))
                DataEnerg(m,:) = xor( DataVit(m,:), DispSequense(1:length(DataEnerg(1,:))) );
            end
            if( DebugOn==1 )
                disp('##### DataEnerg'); DataEnerg(1,1:5),
                SizeDataEnerg = size( DataEnerg ), pause
            end    
                   
          % Super Frame Handling --------------------------------------------------------------------
          
            if(SuperFrame2AAC==0) % Super Frame decoding - by outside program DABp2AAC.EXE
               % Audio data creation (from bits to bytes) -------------------------------------------------
                 Audio=( reshape( DataEnerg',8,[] )' * [128 64 32 16 8 4 2 1]' )';
               % Write to a file (bytes to a file) --------------------------------------------------------
                 fwrite(fid, Audio, 'uint8');  % file extension ".dat"
            end
            
            if(SuperFrame2AAC==1) % Super Frame decoding - below
                
               % size( NBitsPerFrame*DabFrameCount+1 : NBitsPerFrame*(DabFrameCount+4) ), size(reshape(DataEnerg',1,[])), pause 
               % DataEnergChain( NBitsPerFrame*DabFrameCount+1 : NBitsPerFrame*(DabFrameCount+4) ) = reshape(DataEnerg',1,[]);   % reshaping to one row
               % DabFrameCount, 1+NBitsPerFrame*DabFrameCount, NDE+NBitsPerFrame*DabFrameCount, pause
                
                 DataEng = reshape(DataEnerg',1,[]); [ dummy, NDE ] = size(DataEng); NBitsPerFrame=NDE/NCIFsPerFrame; DabFrameCount; % BY£O OK: NBitsPerFrame=NDE/4;
                 DataEnergChain( 1+NBitsPerFrame*DabFrameCount : NDE+NBitsPerFrame*DabFrameCount ) = DataEng(1:NDE);
                 if( DebugOn==1 )
                     disp('##### DataEnergChain'); DataEnergChain(1,1:5),
                     SizeDataEnergChain = size( DataEnergChain ), pause
                 end    
                 if FrameNr >= 6
                  % [ DabFrameCount, SuperFrameSync, DataEnergChain, au_start, num_aus ] = SuperFrameHandling( DabFrameCount, SuperFrameSync, DataEnergChain, au_start, Audiokbps, lan, num_aus, NBitsPerFrame, fid);
                    [ DabFrameCount, SuperFrameSync, DataEnergChain, au_start, num_aus ] = SuperFrameHandlingX( DabFrameCount, SuperFrameSync, DataEnergChain, au_start, Audiokbps, lan, num_aus, NBitsPerFrame, NCIFsPerFrame, fid);
                 end
            end    % end of Super Frame decoding 
            
        end    % end of UEP/EEP
    end    % end of ~isempty(Signal)
end    % end of while (reading data) 

fprintf('Time ellapsed %3i sec \n',round(toc));

fclose('all');

if lan == 1
socket.close();
sSocket.close();
end
