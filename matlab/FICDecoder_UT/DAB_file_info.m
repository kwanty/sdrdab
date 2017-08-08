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
% ####################################
% ####################################
% ####################################
% ####################################
% ####################################
%                                     Synthetic files for testing Modes (1, 2, 3, 4) 
% ####################################
% ####################################
% ####################################
% ####################################
% ####################################

  if(TestMode==1)
    
    % ------------------------------------------------------------------------------------------
    % Testing bit detection (ONLY!) in different modes - for starting student labs
    % See programs DAB_gen_big.m, DAB_gen_simple.m and DAB_dqpsk_simple.m
    %
    % if(Mode==1) ReadFile = fopen('DAB_synt_2.048MHz_IQ_float_Mode1.dat', 'rb');  end % Mode=1  
    % if(Mode==2) ReadFile = fopen('DAB_synt_2.048MHz_IQ_float_Mode2.dat', 'rb');  end % Mode=2
    % if(Mode==3) ReadFile = fopen('DAB_synt_2.048MHz_IQ_float_Mode3.dat', 'rb');  end % Mode=3
    % if(Mode==4) ReadFile = fopen('DAB_synt_2.048MHz_IQ_float_Mode4.dat', 'rb');  end % Mode=4
    % fs_file = 2.048e6; sigtype=1; fi=-pi/4; coef=0.9;
    % SubChStartAdr = 84;  SubChSize = 48; Audiokbps =  64; Protection = 4; UEP = true; % Noname
    % ------------------------------------------------------------------------------------------
    
      if(Mode==1) % London Mode=1, only DAB
       %  ReadFile = fopen('London_mode1_iq.dat', 'rb');
       %  SubChStartAdr =  0;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=1 DAB
       %  SubChStartAdr =605;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=2 DAB
       %  SubChStartAdr =288;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=4 DAB
       %  SubChStartAdr = 96;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=5 DAB

          ReadFile = fopen('Record3_katowice_iq.dat', 'rb');
          SubChStartAdr = 0;   SubChSize = 84; Audiokbps = 0;   Protection = ['3A'];  UEP=false; % SubChID=1
       %  SubChStartAdr = 84;  SubChSize = 96; Audiokbps = 0;   Protection = ['3A'];  UEP=false; % SubChID=2
       %  SubChStartAdr = 444; SubChSize = 72; Audiokbps = 0;   Protection = ['3A'];  UEP=false; % SubChID=7
       %  SubChStartAdr = 516; SubChSize = 78; Audiokbps = 0;   Protection = ['3A'];  UEP=false; % SubChID=8

      end   
      if(Mode==2) % London Mode=2 DAB or Kielce Mode=2 DAB+
        % ReadFile = fopen('London_mode2_iq.dat', 'rb');
        % SubChStartAdr =  0;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=1 DAB
        % SubChStartAdr =605;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=2 DAB
        % SubChStartAdr =288;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=4 DAB
        % SubChStartAdr = 96;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=5 DAB
        
          ReadFile = fopen('../../data/Kielce_mode2_iq.dat', 'rb');
          SubChStartAdr =    0;  SubChSize = 72; Audiokbps = 0;   Protection = ['3A']; UEP = false;  % SubChID=1 DAB+
        % SubChStartAdr =   72;  SubChSize = 78; Audiokbps = 0;   Protection = ['3A']; UEP = false;  % SubChID=2 DAB+
      end
      if(Mode==3) % London Mode=3 DAB & DAB+ 
          ReadFile = fopen('../../data/London_mode3_iq.dat', 'rb');
          SubChStartAdr =    0;  SubChSize = 72; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=1 DAB+
        % SubChStartAdr =  308;  SubChSize = 72; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=4 DAB+
        % SubChStartAdr =   72;  SubChSize =140; Audiokbps = 128; Protection = 1;      UEP = true;  % SubChID=2 DAB 
        % SubChStartAdr =  212;  SubChSize = 96; Audiokbps = 128; Protection = 3;      UEP = true;  % SubChID=3 DAB
      
        % ReadFile = fopen('London_mode3p_iq.dat', 'rb');
        % SubChStartAdr =   64;  SubChSize = 108; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=5 DAB+
        % SubChStartAdr =  172;  SubChSize = 108; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=6 DAB+
        % SubChStartAdr =  280;  SubChSize = 108; Audiokbps = 0;   Protection = ['3A']; UEP = false; % SubChID=7 DAB+
      end
      if(Mode==4) % London Mode=4 DAB and Kielce Mode=4 DAB+
        % ReadFile = fopen('London_mode4_iq.dat', 'rb');
        % SubChStartAdr =  0;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=1 DAB
        % SubChStartAdr =605;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=2 DAB
        % SubChStartAdr =288;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=4 DAB
        % SubChStartAdr = 96;  SubChSize = 96; Audiokbps = 128; Protection = 3; UEP = true;  % SubChID=5 DAB
        
          ReadFile = fopen('../../data/Kielce_mode4_iq.dat', 'rb');
          SubChStartAdr =  0;   SubChSize = 72; Audiokbps = 0; Protection = ['3A']; UEP = false;  % SubChID=1 DAB+
        % SubChStartAdr =  72;  SubChSize = 78; Audiokbps = 0; Protection = ['3A']; UEP = false;  % SubChID=2 DAB+
      end
      fs_file = 2.048e6; sigtype=1; fi=0; coef=0.9;
      
  else % of TestMode==1

% ####################################
% ####################################
% ####################################
% ####################################
% ####################################
%                                      REAL REACORDINGS IN MODE=1 !!!!! 
% ####################################
% ####################################
% ####################################
% ####################################
% ####################################

      
% #########################
% #########################
% #########################
% #########################
% #########################
%                          USRP Ettus Research float 
% #########################
% #########################
% #########################
% #########################
% #########################

% ###############################################################################################################
% RECORD1_SWISS AND RECORD2_SWISS
% OK decoding
% 1) not very noisy
% 2) small IF freq drift
% 3) small ADC freq drift
% 4) every second NULL zone is more oscilatory but synchronization is not lost

% Record1.dat ###################################################################################################
% Mainly DAB

if(0) ReadFile = fopen('Record1.dat', 'rb');   fs_file = 2.0e6;   sigtype=1;   fi=0;   coef = 0.9; end

      % DAB
if(0) SubChStartAdr = 188;  SubChSize = 84;  Audiokbps = 128;  Protection = 4;  UEP = true; end  % DAB   A. DRS 3
if(0) SubChStartAdr = 272;  SubChSize = 96;  Audiokbps = 128;  Protection = 3;  UEP = true; end  % DAB   B. DRS Musikwelle
if(0) SubChStartAdr = 368;  SubChSize = 96;  Audiokbps = 128;  Protection = 3;  UEP = true; end  % DAB   C. DRS Virus
if(0) SubChStartAdr = 464;  SubChSize = 84;  Audiokbps = 128;  Protection = 4;  UEP = true; end  % DAB   D. chosen by TZ
if(0) SubChStartAdr = 548;  SubChSize = 84;  Audiokbps = 128;  Protection = 4;  UEP = true; end  % DAB   E. CH Classic
if(0) SubChStartAdr = 785;  SubChSize = 42;  Audiokbps = 56;   Protection = 3;  UEP = true; end  % DAB   F. DRS 4 News 
      % DAB+
if(0) SubChStartAdr = 702;  SubChSize = 24;  Audiokbps = 0;    Protection = ['3A'];  UEP=false;  end     % DAB+
if(0) SubChStartAdr = 726;  SubChSize = 24;  Audiokbps = 0;    Protection = ['3A'];  UEP=false;  end     % DAB+
if(0) SubChStartAdr = 827;  SubChSize = 24;  Audiokbps = 0;    Protection = ['3A'];  UEP=false;  end     % DAB+

% Record2.dat #######################################################################################################
% Only DAB+. Difference only in SubChStartAdr!

if(0) ReadFile = fopen('Record2.dat', 'rb');   fs_file = 2.0e6;   sigtype=1;   fi=0;   coef = 0.9; end
      
      % DAB+
if(0)
      SubChStartAdr = 96; % 0, 48, 96, 144, 192, 240, 288, 336, 384, 432, 528, 576, 624      
      SubChSize = 48;
      Audiokbps = 0;
      Protection = ['3A'];
      UEP=false;
end

% ###############################################################################################################
% RECORD3_KATOWICE
% OK everything
% Constellation = perfect square
% Only DAB+
% Record3.dat KATOWICE #############################################################################################

if(1) ReadFile = fopen('../../data/Record3_katowice_iq.dat', 'rb'); fs_file = 2.048e6;  sigtype=1;  fi=0;  coef = 0; end

      % DAB+ only
if(1) SubChStartAdr = 0;    SubChSize = 84;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #1
if(0) SubChStartAdr = 84;   SubChSize = 96;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #2
if(0) SubChStartAdr = 180;  SubChSize = 84;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #3
if(0) SubChStartAdr = 264;  SubChSize = 84;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #4
if(0) SubChStartAdr = 348;  SubChSize = 48;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #5
if(0) SubChStartAdr = 396;  SubChSize = 48;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #6
if(0) SubChStartAdr = 444;  SubChSize = 72;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #7 BAD jumps
if(0) SubChStartAdr = 516;  SubChSize = 78;   Audiokbps = 0;  Protection = ['3A'];  UEP=false;  end     % DAB+  Channel #8

% #########################
% #########################
% #########################
% #########################
% #########################
%                          USRP Ettus Research int16 from TT
% #########################
% #########################
% #########################
% #########################
% #########################

% TO BIG FREQUENCY DRIFT

if(0)
   ReadFile = fopen('dab181-936MHz_at2MHz_cplx_short.dat', 'rb'); fs_file = 2e6; sigtype=2; fi=0; coef=0;
   t = fread( ReadFile, [2, 50000], 'int16' );            % only for dab181-936MHz_at2MHz_cplx_short.dat
end

if(0)
   ReadFile = fopen('dab223-936MHz_at2MHz_cplx_short.dat', 'rb'); fs_file = 2e6; sigtype=2;  fi=0; coef=0;
end

% ##########################
% ##########################
% ##########################
% ##########################
% ##########################
%                           SDRSharp USB Stick from TT
% ##########################
% ##########################
% ##########################
% ##########################
% ##########################

% Problems:
% 1) very noisy
% 2) big IF freq drift
% 3) big ADC freq drift

% USB SDRSharp DAB 181MHz TT ############################################################################################# 
% mainly DAB

if(0) ReadFile = 'SDRSharp_20140728_133001Z_181930kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi= 0/180*pi;   coef=0.9; end % -75,  -15

      % DAB
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
      % DAB+
if(0) SubChStartAdr = 846; SubChSize = 12;  Audiokbps = 0;   Protection = ['3A'];  UEP=false; end % Channel #62 DAB+ (only one)


% USB SDRSharp DAB 223 MHz TT #################################################################################################
% mainly DAB

if(0) ReadFile = 'SDRSharp_20140728_132615Z_223930kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi= 0/180*pi;   coef=0.9; end % -35,  -15

      % DAB
if(0) SubChStartAdr = 48;  SubChSize = 96;  Audiokbps = 128;  Protection = 3;      UEP=true;  end % Channel #15 DAB 
if(0) SubChStartAdr = 144; SubChSize = 96;  Audiokbps = 128;  Protection = 3;      UEP=true;  end % Channel #16 DAB 
if(0) SubChStartAdr = 240; SubChSize = 140; Audiokbps = 192;  Protection = 3;      UEP=true;  end % Channel #17 DAB 
if(0) SubChStartAdr = 380; SubChSize = 70;  Audiokbps = 96;   Protection = 3;      UEP=true;  end % Channel #18 DAB 
if(0) SubChStartAdr = 450; SubChSize = 48;  Audiokbps = 64;   Protection = 3;      UEP=true;  end % Channel #19 DAB 
if(0) SubChStartAdr = 498; SubChSize = 96;  Audiokbps = 128;  Protection = 3;      UEP=true;  end % Channel #20 DAB 
      % DAB+
if(0) SubChStartAdr = 0;   SubChSize = 48;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #11 DAB+ (only one)

% USB SDRShart DAB 227 MHz TT ##################################################################################################
% mainly DAB+

if(0) ReadFile = 'SDRSharp_20140728_132746Z_227360kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi= 0/180*pi;   coef=0.9; end % -115, -15 stable f, big noise 
      % DAB+
if(0) SubChStartAdr = 608; SubChSize = 60;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #8  DAB+ OK pocz\B9tek
if(0) SubChStartAdr = 668; SubChSize = 60;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #9  DAB+ OK ca\B3y
if(0) SubChStartAdr = 728; SubChSize = 60;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #10 DAB+ Ok \9Cwisty
if(0) SubChStartAdr = 788; SubChSize = 12;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #13 DAB+
if(0) SubChStartAdr = 800; SubChSize = 12;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #14 DAB+
if(0) SubChStartAdr = 812; SubChSize = 12;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #17 DAB+
if(0) SubChStartAdr = 824; SubChSize = 12;  Audiokbps = 0;    Protection = ['3A']; UEP=false; end % Channel #18 DAB+
      % DAB
if(0) SubChStartAdr = 0;   SubChSize = 104; Audiokbps = 160;  Protection = 4;      UEP=true;  end % Channel #1 DAB
if(0) SubChStartAdr = 104; SubChSize = 84;  Audiokbps = 128;  Protection = 4;      UEP=true;  end % Channel #2 DAB
if(0) SubChStartAdr = 188; SubChSize = 116; Audiokbps = 192;  Protection = 4;      UEP=true;  end % Channel #3 DAB
if(0) SubChStartAdr = 304; SubChSize = 104; Audiokbps = 160;  Protection = 4;      UEP=true;  end % Channel #4 DAB
if(0) SubChStartAdr = 408; SubChSize = 104; Audiokbps = 160;  Protection = 4;      UEP=true;  end % Channel #5 DAB
if(0) SubChStartAdr = 512; SubChSize = 48;  Audiokbps = 64;   Protection = 3;      UEP=true;  end % Channel #6 DAB
if(0) SubChStartAdr = 560; SubChSize = 48;  Audiokbps = 64;   Protection = 3;      UEP=true;  end % Channel #7 DAB

% ##########################
% ##########################
% ##########################
% ##########################
% ##########################
%                           SDRSharp USB Stick from TZ & JB
% ##########################
% ##########################
% ##########################
% ##########################
% ##########################

      % DAB+ Krak\F3w - TZ without AGC correct BAD BAD BAD
if(0) ReadFile = 'SDRSharp_20150102_110331Z_229277kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150102_111436Z_229277kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150102_123014Z_229297kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150102_205931Z_229279kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'DAB_Krakow_20150102_IQ_3secs.wav';            fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
      % DAB+ KRAK\D3W - TZ Krak\F3w with AGC correct OK OK OK OK
if(0) ReadFile = 'SDRSharp_20150102_125222Z_229077kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150102_125438Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150102_185516Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150102_190532Z_229067kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150103_104431Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150103_105533Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150103_110445Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end 
if(0) ReadFile = 'SDRSharp_20150103_192637Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150103_192918Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150103_194107Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
if(0) ReadFile = 'SDRSharp_20150103_194351Z_229069kHz_IQ.wav';  fs_file = 2.048e6;  sigtype=3;   fi=0;  coef=0.9; end
      % DAB+ KRAK\D3W - JB
if(0) ReadFile = fopen('antena_3_dab_229072kHz_fs2048kHz_gain44_BAD.raw', 'rb'); fs_file=2.048e6; sigtype=4; fi=0; coef=0.9; end
if(0) ReadFile = fopen('antena_3_dab_229072kHz_fs2048kHz_gain44_OK.raw', 'rb');  fs_file=2.048e6; sigtype=4; fi=0; coef=0.9; end

% #################################################################################################################
% DAB+ Krak\F3w

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
% SubChID:  8   StartAdr: 516   EEP   ProtLevel: 3-A   SubChSize:  78   % Radio Krak\F3w


end % End of TestMode

  
% ############################################################## 
% fid = fopen('AudioX.mp2', 'w');   % output audio file for DAB
  fid = fopen('AudioX.aac', 'w');   % output audio file for DAB+
% ##############################################################
