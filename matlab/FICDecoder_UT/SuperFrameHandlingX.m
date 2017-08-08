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
% Super Frame Handling
% -----------------------------------------------------------------------------------------

function [ CIFCount, SuperFrameSync, DataEnergChain, au_start, num_aus] = SuperFrameHandling( CIFCount, SuperFrameSync, DataEnergChain, au_start, Audiokbps, lan, num_aus, NBitsPerCIF, NCIFsPerFrame, RSpolynomial, fid)

persistent dac_rate sbr_flag;
persistent adts_dacsbr adts_chanconf; 

CIFCount = CIFCount + NCIFsPerFrame;  % BY£O: + 4

while CIFCount >= 5                        % 5 * 24ms(1CIF) per SuperFrame
   %  CIFCount, pause
    
    if( SuperFrameSync == true ) % WHEN SUPERFRAME IS ALREADY SYNCHRONIZED =============================================
       
       % disp('SuperFrame Synchro OK!'); pause
         
       % dac_rate, sbr_flag, num_aus, au_start, pause
       % Audio = ( reshape(DataEnergChain',8,[])' * [128 64 32 16 8 4 2 1]' )';  % from bits to bytes
       % fwrite(fid, Audio(1:480), 'uint8');  % file extension ".dat"
        
           if(1) % Reed-Solomon correction ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                   
                % Now do the RS correction upon DataEnergChain data
                % data from DataEnergChain(1) to DataEnergChain(5*NbitsPerFrame)
                % organize them into matrix 
               
                % ASF = 5 DAB frames times
                % Audio Super Frame organization example for Mode=1 and 32 kbps:
                % 5 times 768 bits (96 bytes) = 480 bytes
                % devided into 4 rows of 120 bytes each (data are put in consecutive columns up-down left-to-right)
                % In first 110 columns we have original data, in last 10 columns we have FEC data for each row.
                % RS(120, 110, t=5) - correction of up to 5 rand erroneous bytes in a received word of 110 bytes
                % Matlab: [row of 110bytes] = RSDEC([row of 120bytes],120,110,poly), poly=x^8 + x^4 + x^2 + x^2 + 1
                
                % Reed-Solomon decoder -  ETSI 2010, pp. 15                               
                       
                % disp('XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX');
                  
                  DECDecIn = (reshape(DataEnergChain(1:5*NBitsPerCIF)',8,[])' * [128 64 32 16 8 4 2 1]' )';
                  Nlen = length(DECDecIn); Ncols=120; Nrows=Nlen/Ncols; 
                  DECMatDecIn = reshape(DECDecIn,Nrows,Ncols);
                  
               %  size(DataEnergChain(1:5*NBitsPerCIF)), size(DECDecIn), size(DECMatDecIn), pause
                  
                  DECMatRS = rsdec( gf( [ zeros(Nrows,135) DECMatDecIn ], 8), 255, 245, RSpolynomial);
                  DECMatRS = DECMatRS(1:Nrows,136:245);
                  DECDec = reshape( DECMatRS, 1, Nrows*110);
                  DECDec = [ DECDec zeros(1,10*Nrows) ];

                  DECDecOut = double( DECDec.x );
                  for k=1:length(DECDecOut)
                      DataEnergChainOut(1+(k-1)*8 : 8+(k-1)*8) = DecToBin( DECDecOut(k), 8 );
                  end

                % size(DataEnergChainOut), size(DataEnergChain),   % pause
                % DataEnergChain(1:10), DataEnergChainOut(1:10),   % pause
                % DataEnergChain(N-9:N), DataEnergChainOut(N-9:N), % pause
                % for k=1:N
                %     if( abs(DataEnergChain(k) - DataEnergChainOut(k)) == 1) k, pause, end
                % end
                
                  NBitsPerInfo = round(110/120*NBitsPerCIF);
                  if( max(abs(DataEnergChain(1:5*NBitsPerInfo)-DataEnergChainOut(1:5*NBitsPerInfo))) ~= 0)
                      disp( 'RS CORRECTION WAS DONE' ); pause,
                  end
                  DataEnergChain(1:5*NBitsPerCIF) = DataEnergChainOut(1:5*NBitsPerCIF);

            end % of Reed-Solomon +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
               
       
         for r=1:num_aus-1                                                              % access unit (AU) start adresses
             au_start(r+1) = BinToDec( DataEnergChain( 25+12*(r-1) : 25+12*r-1 ), 12 ); % start from 25bit, each ADDR has 12bits
         end
     %   au_start, pause
         for r=1:num_aus                                                                % checking CRC in each AU
             ModCoded = DataEnergChain( au_start(r)*8+1 : au_start(r+1)*8);             % AU extraction
             if CRC16(ModCoded) == 1                                                    % CRC checking
               % disp('CRC OK'), num_aus, pause            % OK
             else
                 SuperFrameSync = false;
               % disp('CRC NOT'), num_aus, pause           % ERROR
             end
         end
         
         for r=1:num_aus             
             au_size = au_start(r+1) - au_start(r) - 2;  % AU size in bytes, without CRC; -2 bytes?
             adts_size = au_size + 7;                    % frame size = au_size bytes of AU + 7 bytes of the adts-header
             adts_size_bin = DecToBin(adts_size, 13);    % frame size written binary
              
           % adts_header = 7 bytes * 8 bits = 56 bits    
             adts_header        =  zeros(1,56);
             adts_header(1:12)  =  ones(1,12);      % syncword
             adts_header(13)    =  [ 1 ];           % 0=MPEG-4, 1=MPEG-2
             adts_header(14:15) =  [ 0 0 ];         % 00=MPEG-4, layer=MPEG-2
             adts_header(16)    =  [ 1 ];           % 1 = no CRC, 0=CRC protection of adst_header is present (+2 bytes in the end) 
             adts_header(17:18) =  [ 0 0 ];         % ? MPEG-4 audio object type minus 1
             adts_header(19:22) =  adts_dacsbr;     % MPEG-4 sampling frequency index (15 not allowed); [ 0 1 1 0] ; 
             adts_header(23)    =  [ 0 ];           % 1=private stream
             adts_header(24:26) =  adts_chanconf;   % MPEG-4 channel configuration; [ 0 0 1 ]
             adts_header(27)    =  0;               % 1=originality
             adts_header(28)    =  0;               % 1=home
             adts_header(29)    =  1;               % 1=copyright stream
             adts_header(30)    =  1;               % 1=copywright start
             adts_header(31:43) =  adts_size_bin;   % frame length = 7 bytes of the adts header + au_size bytes of AU 
             adts_header(44:54) =  ones(1,11);      % unknown yet
             adts_header(55:56) =  [ 0 0 ];         % frames count in one packet
           % + 2 bytes of CRC of the header if required

             AU = DataEnergChain( au_start(r)*8+1 : au_start(r+1)*8 - 16 );           % cut out 16-bit CRC word on the end
             adts_bin = [ adts_header(1:56) AU ];                                     % append ADTS header
             adts_dec = ( reshape(adts_bin',8,[])' * [128 64 32 16 8 4 2 1]' )';      % from bits to bytes
             fwrite(fid, adts_dec, 'uint8');                                          % file extension ".dat"
             
         end

       % NBitsPerCIF, 5 DAB frames
         DataEnergChain = circshift( DataEnergChain, [0 -5*NBitsPerCIF] );    % shift the data by 5 DAB frames, e.g. 5*768bits = 3840 bits
         CIFCount = CIFCount - 5;                                             % decrement the DAB frame counter by 5

    else % DO THE SUPERFRAME SYNCHRONIZATION ========================================================================
        
      % disp('DO SUPERFRAME SYNCHRO'), pause
      
        DataEnergChain(1:16);                            % FIRE_CODE for synchronization
        dac_rate = DataEnergChain(18);                   % digital analog converter (DAC) (sampling frequency of audio)
        sbr_flag = DataEnergChain(19);                   % spectral band replication (SBR)
        switch BinToDec( [dac_rate sbr_flag], 2 )        % decoding of AUs parameters
            case 0    % 00b, dac=0, sbr=0 
                num_aus = 4;                             % number of Access Units (AUs)
                au_start = zeros(1,num_aus+1);           % matrix of staring addresses of all AUs
                au_start(1) = 8;                         % address of the first AU
                adts_dacsbr = [  0 1 0 1 ];              % freq index for ADTS header
            case 1    % 01b, dac=0, sbr=1
                num_aus = 2;                             % number of AUs
                au_start = zeros(1,num_aus+1);           % matrix for AU addresses
                au_start(1) = 5;                         % address of 1-st AU
                adts_dacsbr = [ 1 0 0 0 ];               % freq index for ADTS header
            case 2    % 10b, dac=1, sbr=0
                num_aus = 6;                             % number of AUs
                au_start = zeros(1,num_aus+1);           % matrix for AU addresses
                au_start(1) = 11;                        % address of 1-st AU
                adts_dacsbr = [ 0 0 1 1 ];               % freq index for ADTS header
            case 3    % 11b, dac=1, sbr=1
                num_aus = 3;                             % number of AUs
                au_start = zeros(1,num_aus+1);           % matrix for AU addresses
                au_start(1) = 6;                         % address of 1-st AU
                adts_dacsbr = [ 0 1 1 0 ];               % freq index for ADTS header
        end
        aac_channel_mode = DataEnergChain(20);           % 0=mono, 1=stereo
        ps_flag  = DataEnergChain(21);                   % parametric stereo (PS)
        if(ps_flag==1) adts_chanconf = [ 0 1 0 ]; end    % channel index for ADTS header
        if(ps_flag==0) adts_chanconf = [ 0 0 1 ]; end    % channel index for ADTS header
        mpeg_surround_config = DataEnergChain(22:24);    % 
 
     %  num_aus,
        for r=1:num_aus-1                                % addresses of the AUs from 2:last
            au_start(r+1) = BinToDec( DataEnergChain( 25+12*(r-1) : 25+12*r-1 ), 12 );  % start from 25bit, each ADDR has 12bits
        end
        au_start(num_aus+1) = Audiokbps/8*110;           % adress of the last+1 AU = super frame size
        
      % We are looking for Super Frame start. Therefore we are checking the following CONDITION:
      
        if( ((au_start(1))<au_start(2)) && (au_start(1)<au_start(num_aus+1)) && (au_start(2)<au_start(num_aus+1)) )
            ModCoded = DataEnergChain( au_start(1)*8+1 : au_start(2)*8 );    % first AU, 8bits
            if CRC16(ModCoded) == 1                                          % CRC checking
               SuperFrameSync = true;                                        % CRC OK, SuperFrame is found
            else   
               DataEnergChain = circshift(DataEnergChain,[0 -NBitsPerCIF]);  % CRC BAD, shift the data by 1 DAB frame, e.g. 768 bits
               CIFCount = CIFCount - 1;
            end
        else
            DataEnergChain = circshift(DataEnergChain,[0 -NBitsPerCIF]);     % shift the data by 1 DAB frame, e.g. 768 bits
            CIFCount = CIFCount - 1;                                         % decrement the DAB frame counter by 1
        end
        
    end % of if( SuperFrameSync == true )

end  % of while( CIFCount >= 5 )