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

function [ Start, Size, kbps, Protect, ErrProtect ] = decodeFIC( Mode, DABFrameNr, DataFIC, trellis, DebugOn )

global NFIBsPerFrame NCIFsPerFrame NSymbPerFIC  NFIBsPerCIF

  % Constants for FIC only
    if(Mode==3) NSampAfterTimeDepunct = 4120; NSampAfterVit = NSampAfterTimeDepunct/4-6; % 1024;
    else        NSampAfterTimeDepunct = 3096; NSampAfterVit = NSampAfterTimeDepunct/4-6; % 768; 
    end          
 
  % Put bits of FIBs corresponding to one CIF in separate row -------------------------------
    NbitsPerFIBs = NFIBsPerCIF * 256;              % 256 = number of bits per one FIB
    FIC = zeros(NCIFsPerFrame, 3*NbitsPerFIBs);    % 3*NbitsPerFIBs=2304/2304/3072/2304
    for r=0:NCIFsPerFrame-1
        FIC(r+1,:) = DataFIC(r*(3*NbitsPerFIBs)+1:(r+1)*3*(NbitsPerFIBs));
    end
    if( DebugOn==1)
        disp('#####'); FICSize = size(FIC),   % NCIFsPerFrame x 3*NFIBsPerCIF*256
        ERROR = FICSize - [ NCIFsPerFrame, 3*NFIBsPerCIF*256 ], pause,
    end
        
  % De-puncturing ---------------------------------------------------------------------------
    if(Mode~=3) DataDep = [depuncturing(FIC(:,1:2016),16) depuncturing(FIC(:,2017:2292),15) depuncturing(FIC(:,2293:2304),8)];
    else        DataDep = [depuncturing(FIC(:,1:2784),16) depuncturing(FIC(:,2785:3060),15) depuncturing(FIC(:,3061:3072),8)];
    end
    if( DebugOn==1)
        disp('#####'); FICDataDepSize = size(DataDep),   % NCIFsPerFrame x NSampAfterTimeDepunct
        ERROR = FICDataDepSize - [ NCIFsPerFrame, NSampAfterTimeDepunct ], pause,
    end
        
  % Viterbi decoder -------------------------------------------------------------------------
    DataVit = zeros(NCIFsPerFrame,NFIBsPerCIF*256+6);          % Bitvector + Tail
    for f=1:NCIFsPerFrame
      % DataVit(f,:) = DABViterbi(DataDep(f,:));                                       % OK! Ours
        DataVit(f,:) = vitdec( DataDep(f,:), trellis, 1, 'trunc', 'unquant');  %       % OK! Matlab
      % DataVit(f,:) = vitdec( [DataDep(f,:) 0 0 0 0], trellis, 1, 'cont', 'unquant'); % BAD! From MSC 
    end
    DataVit = DataVit(:,1:end-6);               % Tail removing
    if( DebugOn==1)
        disp('#####'); FICDataVitSize = size(DataVit),   % NCIFsPerFrame x NSampAfterVit
        ERROR = FICDataVitSize - [ NCIFsPerFrame, NSampAfterVit ], pause,
    end

  % Energy dispersal ------------------------------------------------------------------------
    DataEnerg = zeros(size(DataVit));
    for m=1:NCIFsPerFrame
        DataEnerg(m,:) = xor( DataVit(m,:), EnergyDispGen(NFIBsPerCIF*256) );
    end
    if( DebugOn==1)
        disp('#####'); FICDataEnergSize = size(DataEnerg),   % NCIFsPerFrame x NSampAfterVit
        ERROR = FICDataEnergSize - [ NCIFsPerFrame, NSampAfterVit ], pause,
    end

  % FIBs building ---------------------------------------------------------------------------
  % if(Mode ~= 3) FIB = reshape(DataEnerg',256,NFIBsPerFrame)';
  % else          FIB = DataEnerg';
  % end
    FIB = reshape(DataEnerg',256,NFIBsPerFrame)';
    if( DebugOn==1)
        disp('#####'); FIBSize = size( FIB ),   % NFIBsPerFrame x 256
        ERROR = FIBSize - [ NFIBsPerFrame, 256 ], pause,
    end 
        
  % CRC checking of FIBS --------------------------------------------------------------------
  %  Correct for Mode = 1 
  %  FIBCRCCheck = [ CRC16(FIB(1,:)) CRC16(FIB(2,:))  CRC16(FIB(3,:))  CRC16(FIB(4,:)) ...
  %                  CRC16(FIB(5,:)) CRC16(FIB(6,:))  CRC16(FIB(7,:))  CRC16(FIB(8,:)) ...
  %                  CRC16(FIB(9,:)) CRC16(FIB(10,:)) CRC16(FIB(11,:)) CRC16(FIB(12,:)) ];
     FIBCRCCheck = zeros(1,NFIBsPerFrame);
     for k=1:NFIBsPerFrame
         FIBCRCCheck(k) = CRC16(FIB(k,:));
     end    
     if( DebugOn==1) FIBCRCCheckSize = size(FIBCRCCheck), pause, end
        
     if sum(FIBCRCCheck) == NFIBsPerFrame % 12/3/4/6 for Mode=1/2/3/4
          disp('CRC OK');                    % FIGs building from FIBS 
          for FIBNr = 1:NFIBsPerFrame     % to 12/3/4/6 for Mode=1/2/3/4
              pos = 1;
              while pos < 241
                    if FIB(FIBNr,pos:pos+7) == [1 1 1 1 1 1 1 1], break, end
                  % FIG type and length finding
                    Type = BinToDec( FIB(FIBNr,pos:pos+2), 3 );
                    FIGDataLength = BinToDec( FIB(FIBNr,pos+3:pos+7), 5 );
                  % FIG building (reconstruction)
                    FIGType( DABFrameNr, FIBNr, Type, FIGDataLength, FIB(FIBNr,pos+8:pos+8*(FIGDataLength+1)-1) )
                    pos = pos + (FIGDataLength+1)*8;
              end
          end
           disp('################################## Ready !');
           disp('PRESS ANY KEY !'); pause

     else
           disp(['CRC Fail! Frame: ',num2str( DABFrameNr )]);
     end
        
   % To be changed / done  
     Start = 0;
     Size = 0;
     kbps = 0;
     Protect = ['3A'];
     ErrProtect = true;
        

