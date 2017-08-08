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
% FIGs reading and interpretation (result dispayed on the screen)

% -----------------------------------------------------------------------------------------
function [] = FIGType( FrameNr, BlockNr, Type, FIGDataLength, Data )
%%%%%%%%%%%%%%%%%%%%%%%%%% TO SA MOJE GLUPOTY
liczwiersz = 1;
liczkolumna = 1;
wyjscie = [];

for(dlugosc=1:length(Data))
    if mod(dlugosc, 8)==1
        liczwiersz = liczwiersz + 1;
        liczkolumna = 1;
    end
    
    wyjscie(liczwiersz, liczkolumna) = Data(dlugosc);
    liczkolumna = liczkolumna + 1;
end

size(wyjscie);              %
wyjscie = num2str(wyjscie); % USUN ; JESLI CHCESZ SPRAWDZIC STRUKTURE


wyjscie = bin2dec(wyjscie);
wyjscie = wyjscie(2:end);
%%%%%%%%%%%%%%%%%%%%%%%%%% TO JUZ NIE SA MOJE GLUPOTY


Extension = BinToDec(Data(4:8),5);

switch Type

% ##############################################################################################################################
% ##############################################################################################################################
% ##############################################################################################################################

    case 0
        
        CN=Data(1);
        OE=Data(2);
        PD=Data(3);
        
        switch Extension
          % ------------------------------------------------------------------------------------------------------------------------------    
            case 0 % (FIG 0/0) Ensemble information
                pos = 9;
                
                EId = BinToDec(Data(pos:pos+15),16);
                ChangeFlag = BinToDec(Data(pos+16:pos+17),2);
                ALFlag = BinToDec(Data(pos+8),1);
                CIFCount = BinToDec(Data(pos+9:pos+13),5)*250 + BinToDec(Data(pos+14:pos+21),8);
                OccurenceChange = BinToDec(Data(pos+22:pos+29),8);
                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'Output.txt'];
                fp = fopen(name, 'w');
                fprintf( fp, '%d\n%d\n%d\n%d\n', EId, ChangeFlag,ALFlag,CIFCount,OccurenceChange);
                %tu bedzie wiecej rzeczy
                fclose( fp );

                disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d EId: %5d ChangeFlag: %1d ALFlag: %1d CIFCount: %5d OccurenceChange: %3d'...
                             ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,EId,ChangeFlag,ALFlag,CIFCount,OccurenceChange));
                
          % ------------------------------------------------------------------------------------------------------------------------------    
            case 1  % (FIG 0/1) Basic Sub-channel organisation
                pos = 9;
                showmore = 1;  % show more information
                while pos<length(Data)
                    SubChID = BinToDec(Data(pos:pos+5),6);
                    StartAdress = BinToDec(Data(pos+6:pos+15),10);
                    
                    if Data(pos+16) == 1     % Long form
                        
                        SubChannelSize = BinToDec(Data(pos+22:pos+31),10);
                        ProtLevel = BinToDec(Data(pos+20:pos+21),2) + 1;
                        name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'LongOutput.txt'];
                        fp = fopen(name, 'w');
                        fprintf( fp, '%d\n%d\n%d\n', SubChID, StartAdress,SubChannelSize,ProtLevel);
                        %tu bedzie wiecej rzeczy
                        fclose( fp );
                        if Data(pos+20) == 1
                            if showmore == 1
                                disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SubChID: %2d StartAdr:%4d EEP ProtLevel: %1d-A SubChSize: %3d'...
                                             ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SubChID,StartAdress,ProtLevel,SubChannelSize));
                                showmore = 0;
                            else
                                disp(sprintf('                                                               SubChID: %2d StartAdr:%4d EEP ProtLevel: %1d-A SubChSize: %3d'...
                                             ,SubChID,StartAdress,ProtLevel,SubChannelSize));
                            end
                        else
                            if showmore == 1
                                disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SubChID: %2d StartAdr:%4d EEP ProtLevel: %1d-B SubChSize: %3d'...
                                             ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SubChID,StartAdress,ProtLevel,SubChannelSize));
                                showmore = 0;
                            else
                                disp(sprintf('                                                               SubChID: %2d StartAdr:%4d EEP ProtLevel: %1d-B SubChSize: %3d'...
                                          ,SubChID,StartAdress,ProtLevel,SubChannelSize));
                            end
                        end
                        pos = pos + 32;
                        
                    else                  % Short form
                        
                        TabelIndexNr = BinToDec(Data(pos+18:pos+23),6);
                        [SubChannelSize, ProtLevel, BitRate]=TabelIndex(TabelIndexNr); % Tabelle Standard page 50
                        name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'ShortOutput.txt'];
                        fp = fopen(name, 'w');
                        fprintf( fp, '%d\n%d\n%d\n', SubChID, StartAdress,TabelIndexNr);
                        %tu bedzie wiecej rzeczy
                        fclose( fp );
                        if showmore == 1
                            disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SubChID: %2d StartAdr:%4d UEP ProtLevel: %1d   SubChSize: %3d BitBitRate: %3d'...
                                         ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SubChID,StartAdress,ProtLevel,SubChannelSize,BitRate));
                            showmore = 0;
                        else
                            disp(sprintf('                                                               SubChID: %2d StartAdr:%4d UEP ProtLevel: %1d   SubChSize: %3d BitBitRate: %3d'...
                                         ,SubChID,StartAdress,ProtLevel,SubChannelSize,BitRate));
                        end
                        pos = pos + 24;
                    end
                end
                
          % ------------------------------------------------------------------------------------------------------------------------------    
            case 2 % (FIG 0/2) Basic service and service component definition
                pos = 9;
                showmore = 1; % show more information
                while pos<length(Data)
                    if PD == 1              % Long form
                        SId = BinToDec(Data(pos:pos+31),32); % ECC (8Bit) + Country Id(4Bit) + Service reference(20Bit)
                        LocalFlag = Data(32);
                        NbrServiceComponents = BinToDec(Data(pos+36:pos+39),4);
                        pos = pos + 40;
                    else                    % Short form
                        SId = BinToDec(Data(pos:pos+15),16); % Country Id(4Bit) + Service reference(12Bit)
                        LocalFlag = Data(16);
                        NbrServiceComponents = BinToDec(Data(pos+20:pos+23),4);
                        pos = pos + 24;
                    end
                    
                    for k=0:NbrServiceComponents-1
                        TMId = BinToDec(Data(pos:pos+1),2); % Transport Mechanism Identifier
                        PS = Data(pos+14);
                        CAFlag = Data(pos+15);
                        switch TMId
                            case 0 % MSC stream audio
                                ASCTy = BinToDec(Data(pos+2:pos+7),6); % Audio Service Component Type
                                SubChId = BinToDec(Data(pos+8:pos+13),6);
                                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'MSCOutput.txt'];
                                fp = fopen(name, 'w');
                                fprintf( fp, '%d\n%d\n%d\n%d\n%d\n', SId, LocalFlag,NbrServiceComponents,ASCTy,SubChId);
                                %tu bedzie wiecej rzeczy
                                fclose( fp );
                                if showmore == 1
                                    disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d MSC stream audio ASCTy: %2d SubChId: %2d P/S: %1d CAFlag: %1d'...
                                                 ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,ASCTy,SubChId,PS,CAFlag));
                                    showmore = 0;
                                else
                                    disp(sprintf('                                                               SId: %5d MSC stream audio ASCTy: %2d SubChId: %2d P/S: %1d CAFlag: %1d'...
                                              ,SId,ASCTy,SubChId,PS,CAFlag));
                                end
                                
                            case 1 % MSC stream data
                                DSCTy = BinToDec(Data(pos+2:pos+7),6); % Data Service Component Type
                                SubChId = BinToDec(Data(pos+8:pos+13),6);
                                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'MSCSOutput.txt'];
                                fp = fopen(name, 'w');
                                fprintf( fp, '%d\n%d\n%d\n%d\n%d\n', SId, LocalFlag,NbrServiceComponents,DSCTy,SubChId);
                                %tu bedzie wiecej rzeczy
                                fclose( fp );
                                if showmore == 1
                                    disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d MSC stream data DSCTy: %2d SubChId: %2d P/S: %1d CAFlag: %1d'...
                                                 ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,DSCTy,SubChId,PS,CAFlag));
                                    showmore = 0;
                                else
                                    disp(sprintf('                                                               SId: %5d MSC stream data DSCTy: %2d SubChId: %2d P/S: %1d CAFlag: %1d'...
                                              ,SId,DSCTy,SubChId,PS,CAFlag));
                                end
                                
                            case 2 % FIDC
                                DSCTy = BinToDec(Data(pos+2:pos+7),6); % Data Service Component Type
                                FIDCid = BinToDec(Data(pos+8:pos+13),6);
                                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'FOutput.txt'];
                                fp = fopen(name, 'w');
                                fprintf( fp, '%d\n%d\n%d\n%d\n%d\n', SId, LocalFlag,NbrServiceComponents,DSCTy,FIDCid);
                                %tu bedzie wiecej rzeczy
                                fclose( fp );
                                if showmore == 1
                                    disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d FIDC DSCTy: %2d FIDCid: %2d P/S: %1d CAFlag: %1d'...
                                                 ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,DSCTy,FIDCid,PS,CAFlag));
                                    showmore = 0;
                                else
                                    disp(sprintf('                                                               SId: %5d FIDC DSCTy: %2d FIDCid: %2d P/S: %1d CAFlag: %1d'...
                                              ,SId,DSCTy,FIDCid,PS,CAFlag));
                                end
                                
                            otherwise % MSC packet data
                                SCId = BinToDec(Data(pos+2:pos+13),12);  % Service Component Identifier
                                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'MSCPOutput.txt'];
                                fp = fopen(name, 'w');
                                fprintf( fp, '%d\n%d\n%d\n%d\n', SId, LocalFlag,NbrServiceComponents,SCid);
                                %tu bedzie wiecej rzeczy
                                fclose( fp );
                                if showmore == 1
                                    disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d MSC packet data SCId: %4d P/S: %1d CAFlag: %1d'...
                                                 ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,SCId,PS,CAFlag));
                                    showmore = 0;
                                else
                                    disp(sprintf('                                                               SId: %5d MSC packet data SCId: %4d P/S: %1d CAFlag: %1d'...
                                              ,SId,SCId,PS,CAFlag));
                                end
                                
                        end
                        
                        pos = pos + 16;
                    end
                end
                
          % ------------------------------------------------------------------------------------------------------------------------------                   
          % HERE WE HAVE AN ERROR FOR RECORDINGS LONDON_MODE3_IQ.DAT
          % case 5  % (FIG 0/5) Service Component Language
            case 88 % (FIG 0/5) Service Component Language
                pos = 9;
                displaymore = 1;  % display more information
                while pos<length(Data)
                    if Data(pos) == 1 % Long form
                        SCId = BinToDec(Data(pos+5:pos+16),12);
                     %  Sprache = LanguagesCodeTables(BinToDec(Data(pos+16:pos+23),8));  %TZ % Tabelle von TS 101 765[16] implementiert
                        Sprache = 'Unknown TZ';
                        if displaymore == 1
                            disp([sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SCId: %2d '...
                                          ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SCId) Sprache]);
                            displaymore = 0;
                        else
                            disp([sprintf('                                                               SCId: %2d '...
                                          ,SCId) Sprache]);
                        end
                        pos = pos + 24;
                    else              % Short form
                        Id=BinToDec(Data(pos+2:pos+7),6);
                     %  Sprache = LanguagesCodeTables(BinToDec(Data(pos+8:pos+15),8)); %TZ
                        sprache = (BinToDec(Data(pos+8:pos+15),8));
                        if Data(pos+1)==0
                            if displaymore == 1
                                disp([sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d MSC Stream SubChId: %2d '...
                                              ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,Id) Sprache]);
                                displaymore = 0;
                            else
                                disp([sprintf('                                                               MSC Stream SubChId: %2d '...
                                              ,Id) Sprache]);
                            end
                        else
                            if displaymore == 1
                                disp([sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d FIC FIDCId: %2d '...
                                              ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,Id) Sprache]);
                                displaymore = 0;
                            else
                                disp([sprintf('                                                               FIC FIDCId: %2d '...
                                              ,Id) Sprache]);
                            end
                        end
                        
                        pos = pos + 16;
                   end
                    
                end
                
          % ------------------------------------------------------------------------------------------------------------------------------                
            case 6 % (FIG 0/6) Service linking information
                pos = 9;
                displaymore = 1; % display more information; Standard page 118
                while pos<length(Data)
                
                    LA = Data(pos+1);
                    SH = Data(pos+2);
                    ILS = Data(pos+3);
                    LSN = BinToDec(Data(pos+4:pos+15),12);

                    if Data(pos) == 1                                   % Id list flag = 1

                        if PD == 0                                      % P/D = 0
                            IdLQ = BinToDec(Data(pos+17:pos+18),2);
                            Shd = Data(pos+19);
                            NbrOfIds = BinToDec(Data(pos+20:pos+23),4);

                            pos = pos + 24;

                            if ILS == 0                                 % ILS = 0
                                for b=1:NbrOfIds
                                    Id = BinToDec(Data(pos:pos+15),16);
                                    name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'ILS0Output.txt'];
                                    fp = fopen(name, 'w');
                                    fprintf( fp, '%d\n%d\n%d\n%d\n%d\n', LA,SH,ILS,NbrOfIds,Id);
                                    %tu bedzie wiecej rzeczy
                                    fclose( fp );
                                    if displaymore == 1
                                        disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d LA: %1d S/H: %1d ILS: %1d LSN: %4d IdLQ: %1d Shd: %1d Id: %5d'...
                                                     ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,LA,SH,ILS,LSN,IdLQ,Shd,Id));
                                        displaymore = 0;
                                    else
                                        disp(sprintf('                                                               LA: %1d S/H: %1d ILS: %1d LSN: %4d IdLQ: %1d Shd: %1d Id: %5d'...
                                                      ,LA,SH,ILS,LSN,IdLQ,Shd,Id));
                                    end

                                    pos = pos + 16;
                                end
                            else                                       % ILS = 1
                                for b=1:NbrOfIds
                                    ECC = BinToDec(Data(pos:pos+7),8);
                                    Id = BinToDec(Data(pos+8:pos+23),16);
                                    name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'ILS1Output.txt'];
                                    fp = fopen(name, 'w');
                                    fprintf( fp, '%d\n%d\n%d\n%d\n%d\n%d\n', LA,SH,ILS,NbrOfIds,Id,ECC);
                                    %tu bedzie wiecej rzeczy
                                    fclose( fp );
                                    if displaymore == 1
                                        disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d LA: %1d S/H: %1d ILS: %1d LSN: %4d IdLQ: %1d Shd: %1d ECC: %3d Id: %5d'...
                                                      ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,LA,SH,ILS,LSN,IdLQ,Shd,ECC,Id));
                                        displaymore = 0;
                                    else
                                        disp(sprintf('                                                               LA: %1d S/H: %1d ILS: %1d LSN: %4d IdLQ: %1d Shd: %1d ECC: %3d Id: %5d'...
                                                      ,LA,SH,ILS,LSN,IdLQ,Shd,ECC,Id));
                                    end

                                    pos = pos + 24;
                                end
                            end

                        else                                           % P/D = 1
                            for b=1:NbrOfIds
                                SId = BinToDec(Data(pos:pos+31),32);
                                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'PH1Output.txt'];
                                fp = fopen(name, 'w');
                                fprintf( fp, '%d\n%d\n%d\n%d\n%d\n', LA,SH,ILS,NbrOfIds,SId);
                                %tu bedzie wiecej rzeczy
                                fclose( fp );
                                if displaymore == 1
                                    disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d LA: %1d S/H: %1d ILS: %1d LSN: %4d IdLQ: %1d Shd: %1d SId: %10d'...
                                                 ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,LA,SH,ILS,LSN,IdLQ,Shd,SId));
                                    displaymore = 0;
                                else
                                    disp(sprintf('                                                               LA: %1d S/H: %1d ILS: %1d LSN: %4d IdLQ: %1d Shd: %1d SId: %10d'...
                                                 ,LA,SH,ILS,LSN,IdLQ,Shd,SId));
                                end

                                pos = pos + 32;
                            end
                        end

                    else                                               % Id list flag = 0
                        if displaymore == 1
                            disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d LA: %1d S/H: %1d ILS: %1d LSN: %4d'...
                                         ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,LA,SH,ILS,LSN));
                            displaymore = 0;
                        else
                            disp(sprintf('                                                               LA: %1d S/H: %1d ILS: %1d LSN: %4d'...
                                         ,LA,SH,ILS,LSN));
                        end
                        pos = pos + 16;
                    end
                end
                 
          % ------------------------------------------------------------------------------------------------------------------------------                
            case 8 % (FIG 0/8) Service component global definition
                pos = 9;
                showmore = 1; % show more information
                while pos<length(Data)
                    if PD == 1
                        SId = BinToDec(Data(pos:pos+31),32);
                        pos = pos + 32;
                    else
                        SId = BinToDec(Data(pos:pos+15),16);
                        pos = pos + 16;
                    end
                    
                    ExtFlag = Data(pos);
                    SCIdS = BinToDec(Data(pos+4:pos+7),4);
                    
                    if Data(pos+8) == 1                               % Check L/S Flag
                        SCId = BinToDec(Data(pos+12:pos+23),12);
                        name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'LS1Output.txt'];
                        fp = fopen(name, 'w');
                        fprintf( fp, '%d\n%d\n%d\n%d\n', SId,ExtFlag,SCIdS,SCId);
                        %tu bedzie wiecej rzeczy
                        fclose( fp );
                        if showmore == 1
                            disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d ExtFlag: %1d SCIdS: %2d SCId: %4d'...
                                         ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,ExtFlag,SCIdS,SCId));
                            showmore = 0;
                        else
                            disp(sprintf('                                                               SId: %5d ExtFlag: %1d SCIdS: %2d'...
                                         ,SId,ExtFlag,SCIdS,SCId));
                        end
                        
                        pos = pos + 32;
                    else
                        if Data(pos+9) == 1                          % MSC / FIC Flag
                            FIDCId = BinToDec(Data(pos+10:pos+15),6);
                            SCId = BinToDec(Data(pos+12:pos+23),12);
                            name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'FIC1Output.txt'];
                            fp = fopen(name, 'w');
                            fprintf( fp, '%d\n%d\n%d\n%d\n%d\n', SId,ExtFlag,SCIdS,SCId,FIDCId);
                            %tu bedzie wiecej rzeczy
                            fclose( fp );
                            if showmore == 1
                            	disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d ExtFlag: %1d SCIdS: %2d FIC FIDCId: %2d'...
                                              ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,ExtFlag,SCIdS,FIDCId));
                            	showmore = 0;
                            else
                            	disp(sprintf('                                                               SId: %5d ExtFlag: %1d SCIdS: %2d FIC FIDCId: %2d'...
                                              ,SId,ExtFlag,SCIdS,FIDCId));
                            end
                            
                        else
                            SubChId = BinToDec(Data(pos+10:pos+15),6);
                            name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'FIC1Output.txt'];
                            fp = fopen(name, 'w');
                            fprintf( fp, '%d\n%d\n%d\n%d\n', SId,ExtFlag,SCIdS,SubChId);
                            %tu bedzie wiecej rzeczy
                            fclose( fp );
                            if showmore == 1
                            	disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d ExtFlag: %1d SCIdS: %2d MSC in Stream mode SubChId: %2d'...
                                              ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,ExtFlag,SCIdS,SubChId));
                            	showmore = 0;
                            else
                            	disp(sprintf('                                                               SId: %5d ExtFlag: %1d SCIdS: %2d MSC in Stream mode SubChId: %2d'...
                                              ,SId,ExtFlag,SCIdS,SubChId));
                            end
                        end
                        pos = pos + 24;
                    end
                    if ExtFlag == 0; pos = pos - 8;end               % byte subtraction (without Rfa)
                end
                
          % ------------------------------------------------------------------------------------------------------------------------------                
            case 9 % (FIG 0/9) Country....
                LTO = 0.5* BinToDec(Data(12:16),5);
                if Data(11) == 1,LTO=LTO*-1;end
                ECC = BinToDec(Data(17:24),8);
                InterTableId = BinToDec(Data(25:32),8);
                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'Output.txt'];
                fp = fopen(name, 'w');
                fprintf( fp, '%d\n%d\n%d\n', LTO,ECC,InterTableId);
                %tu bedzie wiecej rzeczy
                fclose( fp );
                disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d LTO: %+3.1fh ECC: %3d InterTabId: %1d'...
                             ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,LTO,ECC,InterTableId));
                    
          % ------------------------------------------------------------------------------------------------------------------------------
            case 10 % (FIG 0/10) Date and Time
                MJD = BinToDec(Data(10:26),17);
                
              % [Y,M,D] = mJulianDate(MJD);
                 Y=0; M=0; D=0;
                 
                if FIGDataLength == 5                 % Short Form
                    hour = BinToDec(Data(30:34),5);
                    min = BinToDec(Data(35:40),6);
                    name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F' 'SOutput.txt'];
                    fp = fopen(name, 'w');
                    fprintf( fp, '%d\n%d\n%d\n', MJD,hour,min);
                    %tu bedzie wiecej rzeczy
                    fclose( fp );
                    disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d Date: %04d/%02d/%02d Time: %02d:%02d UTC'...
                                 ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,Y,M,D,hour,min));
                else                                 % Long Form
                    hour = BinToDec(Data(30:34),5);
                    min = BinToDec(Data(35:40),6);
                    sec = BinToDec(Data(41:46),6);
                    millsec = BinToDec(Data(47:56),10);
                    name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'LOutput.txt'];
                    fp = fopen(name, 'w');
                    fprintf( fp, '%d\n%d\n%d\n%d\n%d\n', MJD,hour,min,sec,millsec);
                    %tu bedzie wiecej rzeczy
                    fclose( fp );
                    disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d Date: %04d/%02d/%02d Time: %02d:%02d:%02:%03 UTC'...
                                 ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,Y,M,D,hour,min,sec,millsec));
                end
          
          % ------------------------------------------------------------------------------------------------------------------------------
          % HERE WE HAVE AN ERROR FOR RECORDINGS LONDON_MODE2_IQ.DAT
          % case 13 % (FIG 0/13) User application information, Standard S.124
            case 99 % (FIG 0/13) User application information, Standard S.124  % = SKIP IT!
                pos = 9;
                showmore = 1; % show more information
                while pos<length(Data)
                    if PD == 1;
                        SId = BinToDec(Data(pos:pos+31),30);
                        pos = pos + 32;
                    else
                        SId = BinToDec(Data(pos:pos+15),16);
                        pos = pos + 16;
                    end
                    
                    SCIdS = BinToDec(Data(pos:pos+3),4);
                    NbrOfApp = BinToDec(Data(pos+4:pos+7),4);
                    pos = pos + 8;
                    
                    for v=1:NbrOfApp
                        UsrAppType = BinToDec(Data(pos:pos+10),11);
                        UsrAppDataLength = BinToDec(Data(pos+11:pos+15),5);
                        
                        if UsrAppDataLength > 0
                            CAFlag = Data(pos+16);
                            XPADAppTy = BinToDec(Data(pos+19:pos+23),5);
                            DGFlag = Data(pos+24);
                            DSCTy = BinToDec(Data(pos+26:pos+31),6);

                            if showmore == 1
                                disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d SCIdS: %2d UsrAppType: %4d CAFlag: %1d XPApTy: %2d DGFlag: %1d DSCTy: %2d'...
                                             ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,SCIdS,UsrAppType,CAFlag,XPADAppTy,DGFlag,DSCTy));
                                showmore = 0;
                            else
                                disp(sprintf('                                                               SId: %5d SCIdS: %2d UsrAppType: %4d CAFlag: %1d XPApTy: %2d DGFlag: %1d DSCTy: %2d'...
                                             ,SId,SCIdS,UsrAppType,CAFlag,XPADAppTy,DGFlag,DSCTy));
                            end
                            
                        else
                            if showmore == 1
                                disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d SCIdS: %2d UsrAppType: %4d'...
                                             ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,SCIdS,UsrAppType));
                                showmore = 0;
                            else
                                disp(sprintf('                                                               SId: %5d SCIdS: %2d UsrAppType: %4d'...
                                                  ,SId,SCIdS,UsrAppType));
                            end
                        end
                            
                        pos = UsrAppDataLength*8 + 16 + pos;
                    end
                    
                end

          % ------------------------------------------------------------------------------------------------------------------------------
            case 17 % (FIG 0/17) Programm Type
                pos = 9; 
                showmore = 1; % show more information
                while pos<length(Data)
                
                    SId = BinToDec(Data(pos:pos+15),16);
                    SD = Data(pos+16);
                    PS = Data(pos+17);
                    LFlag = Data(pos+18);
                    CCFlag = Data(pos+19);
                    
                    
                    if LFlag == 1                         % Table from TS 101 765 [16] - implemented
                %       Sprache = LanguagesCodeTables(BinToDec(Data(pos+24:pos+31),8)); %TZ
                        Sprache = (BinToDec(Data(pos+24:pos+31),8));
                    else
                        Sprache = 0;
                    end
                    
                    IntCode = BinToDec(Data(pos+27+8*LFlag:pos+31+8*LFlag),5);
                    name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'Output.txt'];
                    fp = fopen(name, 'w');
                    fprintf( fp, '%d\n%d\n%d\n%d\n%d\n%d\n%d\n', SId,SD,PS,LFlag,CCFlag,Sprache,IntCode);
                    %tu bedzie wiecej rzeczy
                    fclose( fp );
                    if showmore == 1
                        disp([sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d S/D: %1d P/S: %1d CCFlag: %1d IntCode: %2d '...
                                      ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,SD,PS,CCFlag,IntCode) Sprache]);
                        showmore = 0;
                    else
                        disp([sprintf('                                                               SId: %5d S/D: %1d P/S: %1d CCFlag: %1d IntCode: %2d '...
                                      ,SId,SD,PS,CCFlag,IntCode) Sprache]);
                    end
                    
                    pos = pos + 32 + 8*LFlag + 8 * CCFlag;
                end
                
          % ------------------------------------------------------------------------------------------------------------------------------
            case 18 % (FIG 0/18) Announcement support
                pos = 9;
                showmore = 1; % show more information
                while pos<length(Data)
                    
                    SId = BinToDec(Data(pos:pos+15),16);
                    ASuFlags = BinToDec(Data(pos+16:pos+31),16);
                    NbrOfCluster = BinToDec(Data(pos+35:pos+39),5);
                    
                    pos = pos + 40;
                    name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'Output.txt'];
                    fp = fopen(name, 'w');
                    fprintf( fp, '%d\n%d\n%d\n', SId,ASuFlags,NbrOfCluster);
                    %tu bedzie wiecej rzeczy
                    fclose( fp );
                    if NbrOfCluster == 0
                        
                        if showmore == 1
                            disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d ASuFlags: %5d'...
                                         ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,ASuFlags));
                            showmore = 0;
                        else
                            disp(sprintf('                                                               SId: %5d ASuFlags: %5d'...
                                              ,SId,ASuFlags));
                        end
                    end
                    
                    for g=1:NbrOfCluster
                        ClusterId = BinToDec(Data(pos:pos+7),8);
                        if showmore == 1
                            disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d SId: %5d ASuFlags: %5d ClusterId: %3d'...
                                         ,FrameNr,BlockNr,Type,Extension,CN,OE,PD,SId,ASuFlags,ClusterId));
                            showmore = 0;
                        else
                            disp(sprintf('                                                               SId: %5d ASuFlags: %5d ClusterId: %3d'...
                                         ,SId,ASuFlags,ClusterId));
                        end
                        pos = pos + 8;
                    end
                    
                end
          
          % ------------------------------------------------------------------------------------------------------------------------------                
            otherwise
                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'Output.txt'];
                fp = fopen(name, 'w');
                fprintf( fp, '%d\n', Extension);
                %tu bedzie wiecej rzeczy
                fclose( fp );
                disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d C/N: %1d OE: %1d P/D: %1d',FrameNr,BlockNr,Type,Extension,CN,OE,PD));
                
        end
        
% ##############################################################################################################################
% ##############################################################################################################################
% ##############################################################################################################################
    
    case 1
        
        OE=Data(5);
        Charset=BinToDec(Data(1:4),4);
        switch Extension
            case 0 % (FIG 1/0) Ensemble label
                Label = BinToDec(Data(25:152),8);
                IdField = BinToDec(Data(9:24),16);
                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'Output.txt'];
                fp = fopen(name, 'w');
                fprintf( fp, '%d\n', OE,Charset,Label,IdField);
                %tu bedzie wiecej rzeczy
                fclose( fp );
                disp([sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d OE: %1d Charset:%2d EId: %5d Ensemble Label: ',FrameNr,BlockNr,Type,Extension,OE,Charset,IdField) Label]);
            case 1 % (FIG 1/1) Programm service label
                Label = BinToDec(Data(25:152),8);
                IdField = BinToDec(Data(9:24),16);
                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'Output.txt'];
                fp = fopen(name, 'w');
                fprintf( fp, '%d\n', OE,Charset,Label,IdField);
                %tu bedzie wiecej rzeczy
                fclose( fp );
                disp([sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d OE: %1d Charset:%2d SId: %5d Service Label: ',FrameNr,BlockNr,Type,Extension,OE,Charset,IdField) Label]);
            otherwise
                name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'Output.txt'];
                fp = fopen(name, 'w');
                fprintf( fp, '%d\n', Extension);
                %tu bedzie wiecej rzeczy
                fclose( fp );
                
                disp(sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d OE: %1d Charset:%2d',FrameNr,BlockNr,Type,Extension,OE,Charset));
        end
        
% ##############################################################################################################################
% ##############################################################################################################################
% ##############################################################################################################################

    otherwise
        name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'F'   'Output.txt'];
                fp = fopen(name, 'w');
                fprintf( fp, '%d\n', Type);
                %tu bedzie wiecej rzeczy
                fclose( fp );
                
         disp( sprintf('FrameNr: %2d FIB: %2d Type: %2d Extension: %2d', FrameNr, BlockNr, Type, Extension) );
         
% ##############################################################################################################################
% ##############################################################################################################################
% ##############################################################################################################################

end

%%WPISYWANIE DO PLIKÓW

name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'Input.txt'];
fp = fopen(name, 'w');
fprintf( fp, '%d\n', wyjscie);
fclose( fp );

name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'Size.txt'];
fp = fopen(name, 'w');
fprintf( fp, '%d\n', FIGDataLength);
fclose( fp );

name = [../data/ut/T' num2str(Type) 'E' num2str(Extension) 'Type.txt'];
fp = fopen(name, 'w');
fprintf( fp, '%d\n', Type);
fclose( fp );

wyjscie = [];



end

