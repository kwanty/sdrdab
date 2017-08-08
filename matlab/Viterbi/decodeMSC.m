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

function decodeMSC( DABFrameNr, DataMSC, SubChStartAdr, SubChSize, Audiokbps, Protection, UEP, trellis, RSpolynomial, SuperFrame2AAC, NBitsAfterTimeDepunct, DebugOn )

% New after division the program into two functions: decodeFIC(), decodeMSC()
global NSymbPerFrame NCarrPerSymb
global NFIBsPerFrame NCIFsPerFrame NSymbPerFIC  NFIBsPerCIF
global L1Range L2Range L3Range L4Range PI1 PI2 PI3 PI4 Padding
global DispSequense
global DataLastRound
global CIFCount SuperFrameSync DataEnergChain au_start num_aus lan fid

% Building convolutional codeword for each CIF (NCIFsPerFrame=4/1/1/2 for Mode=1/2/3/4)
NData = length(DataMSC); NCIF=55296;   % Ndata = (NSymbPerFrame-NCIFsPerFrame-1) * NCarrPerSymb*2bits = 221184 bits (for Mode=1)
MSC = zeros( NCIFsPerFrame, NCIF );    % NCIF = 864 * 64bits (1CU) = 55296 bits = one CIF = 24 msec
for r = 0 : (NCIFsPerFrame-1)          % put CIF codewords in rows; for Mode 1/2/3/4 = 4/1/1/2 CIFs (rows)
    MSC(r+1, :) = DataMSC( 1 + r*NCIF : NCIF + r*NCIF );
end

if( DebugOn==1 )
    disp('#################### MSC'); MSC(1,1:5),
    SizeMSC = size(MSC), % NCIFsPerFrame x 2*(NSymbPerFrame-NSymbPerFIC-1)*NCarrPerSymb/NCIsPerFrame
    ERROR = SizeMSC - [NCIFsPerFrame, 2*(NSymbPerFrame-NSymbPerFIC-1)*NCarrPerSymb/NCIFsPerFrame], pause
end

% Extracting data of one sub-channel requested by the user (CU=64bits = the smallest addressable unit)----
SubChData(1:NCIFsPerFrame, :) = MSC(:, 64*SubChStartAdr+1 : 64*SubChStartAdr + 64*SubChSize ); % from 4/1/1/2 CIFs

if( DebugOn==1 )
    disp('#################### SubChData'); SubChData(1,1:5),
    SizeSubChData = size(SubChData), % NCIFsPerFrame x 64*SubChSize
    ERROR = SizeSubChData - [NCIFsPerFrame, 64*SubChSize], pause
end
% Here we have a matrix with dimension NCIFsPerFrame x N SubChan bits in one CIF

% Time de-interleaving: output = rows: NCIFsPerFrame, columns: SubChSize*64bits(1CU) = 84*64 = 5376 (our example)
[TimeDeintSubChData, DataLastRound] = TimeDeinterleavingStream( SubChData, SubChSize, DataLastRound, DebugOn );

if( DebugOn==1 )
    disp('#################### TimeDeintSubChData'); TimeDeintSubChData(1,1:5),
    SizeTimeDeintSubChData = size(TimeDeintSubChData), % NCIFsPerFrame x 64*SubChSize
    ERROR = SizeTimeDeintSubChData - [NCIFsPerFrame, 64*SubChSize], pause
    SizeDataLastRound = size(DataLastRound),  % 16 x (4*SubChSize*(16+NCIFsPerFrame-1))
    ERROR = SizeDataLastRound - [16, (4*SubChSize*(16+NCIFsPerFrame-1))], pause
end

% ######################################  % Continue decoding only when the buffer
if( DABFrameNr * NCIFsPerFrame >  16 )  % of time-interleaved data is already filled
    % ######################################  % with 16 CIFs (1 CIF = 24ms = 55296 samples)
    
    % -----------------------------------------------------------------------------------------
    % DAB or DAB+
    % -----------------------------------------------------------------------------------------
    
    if UEP == true % DAB
        
        % (De)puncturing: different parts of CIFs are protected in different manner -------------------------------------
        %  Output = rows: NCIFsPerFrame, cols = 12312 (our example) WHY? GENERAL RULE?
        DataDep = [ depuncturing( TimeDeintSubChData(:,1:L1Range),            PI1 )...  % part 1: 4 rows x  836 cols
            depuncturing( TimeDeintSubChData(:,L1Range+1:L2Range),    PI2 )...  % part 2: 4 x 1176 =837...2012
            depuncturing( TimeDeintSubChData(:,L2Range+1:L3Range),    PI3 )...  % part 3: 4 x 3172 =2013...5184
            depuncturing( TimeDeintSubChData(:,L3Range+1:L4Range),    PI4 )...  % part 4: 4 x 180  =5185...5364
            depuncturing( TimeDeintSubChData(:,L4Range+1:L4Range+12), 8   ) ];  % tail:   4 x 12   =5365...5376
        
        if( DebugOn==1 )
            disp('#################### DataDepunctured'); DataDep(1,1:5),
            SizeDataDep = size(DataDep), % NCIFsPerFrame x NBitsAfterTimeDepunct
            ERROR = SizeDataDep - [ NCIFsPerFrame, NBitsAfterTimeDepunct ], pause
        end
        
        % Viterbi decoder, CIF after CIF, removing 4x redundancy: (xn0,xn1,xn2,xn3) (punctured) --> xn -----------------
        DataVit = zeros(length(DataDep(:,1)),length(DataDep(1,:))/4+1); % rows=NCIFsPerFrame, cols/4 - 4x stream reduction
        for f=1:length(DataDep(:,1)) % each CIF
            DataVit(f,:) = vitdec( [DataDep(f,:) 0 0 0 0], trellis, 1, 'cont', 'unquant');  % each 4 samples--> 1 sample
        end
        DataVit = DataVit(:, 2:end-6);      % tail removing (6 last numbers); in our example: 12312/4-6=3078-6=3072 columns
        
        if( DebugOn==1 )
            disp('#################### DataVit'); DataVit(1,1:5),
            SizeDataVit = size(DataVit), % NCIFsPerFrame x (NBitsAfterTimeDepunct/4-6)
            ERROR = SizeDataVit - [ NCIFsPerFrame, NBitsAfterTimeDepunct/4-6 ], pause
        end
        
        % Energy de-dispersal(XOR with generated PRBS sequence); for our example input has NCIFsPerFrame=4 rows, 3072 columns
        DataEnerg = zeros(size(DataVit));
        for m=1:length(DataEnerg(:,1))
            DataEnerg(m,:) = xor( DataVit(m,:), DispSequense(1:length(DataEnerg(1,:))) );    %
        end
        
        if( DebugOn==1 )
            disp('#################### DataEnerg'); DataEnerg(1,1:5),
            SizeDataEnerg = size(DataEnerg), % NCIFsPerFrame x (NBitsAfterTimeDepunct/4-6)
            ERROR = SizeDataEnerg - [ NCIFsPerFrame, NBitsAfterTimeDepunct/4-6 ], pause
        end
        
        % Audio data creation (from bits to bytes): scan by rows (CIF by CIF), write bits into 8-element rows,
        % inner product with vertical weights 2^p, tranpose to horizontal orientation
        Audio=( reshape( DataEnerg',8,[] )' * [128 64 32 16 8 4 2 1]' )';  % 1536 8-bit numbers = NCIFsPerFrame(4)*3072 bits / 8 bits
        
        if( DebugOn==1 ) disp('#################### Audio');
            SizeAudio = size(Audio),
            ERROR = SizeAudio - [ 1, NCIFsPerFrame*(NBitsAfterTimeDepunct/4-6)/8 ], pause
        end
        
        % Write to a file (bytes to a file) --------------------------------------------------------
        % if DABFrameNr >= 6
        disp('WWWWWWWWWWWWWWWWWWWWWWWWWWWW Audio MP2 written to HD!');
        fwrite(fid, Audio, 'uint8');
        % end
        
    else % DAB+
        persistent idx;
        if isempty(idx)
            idx = 0;
        else
            idx = idx +1;
        end
        % (De)puncturing ----------------------------------------------------------------------------
        % SubChSize =     24    48    96    84    72    76
        % 4x  1536  3072  6144  5376  4608  4992
        DataDep = [ depuncturing( TimeDeintSubChData(:,1:L1Range),            PI1)...         % 4x  1344  2880  5952  5184  4416  4800
            depuncturing( TimeDeintSubChData(:,L1Range+1:L2Range),    PI2)...         % 4x   180   180   180   180   180   180
            depuncturing( TimeDeintSubChData(:,L2Range+1:L2Range+12), 8) ];  % Tail   % 4x    12    12    12    12    12    12
        
        if( DebugOn==1 )
            disp('#################### DataDepunctured'); DataDep(1,1:5),
            SizeDataDep = size(DataDep), % NCIFsPerFrame x NBitsAfterTimeDepunct
            ERROR = SizeDataDep - [ NCIFsPerFrame, NBitsAfterTimeDepunct ], pause
        end
        
        % Viterbi Decoder ---------------------------------------------------------------------------
        DataVit = zeros(length(DataDep(:,1)),length(DataDep(1,:))/4+1); % Audio vector + Tail (6) + Matlab Viterbi (1)
        
        in_size = length(DataDep(1,:))
        if(idx ==10 )
            writeReal( DataDep(1,:), sprintf('../../data/ut/vitterbi_in_MSC_size10776_%d_%d.txt', idx, 0), in_size );
            writeReal( DataDep(2,:), sprintf('../../data/ut/vitterbi_in_MSC_size10776_%d_%d.txt', idx, 1), in_size );
            writeReal( DataDep(3,:), sprintf('../../data/ut/vitterbi_in_MSC_size10776_%d_%d.txt', idx, 2), in_size );
            writeReal( DataDep(4,:), sprintf('../../data/ut/vitterbi_in_MSC_size10776_%d_%d.txt', idx, 3), in_size );
        end        
        
        for f=1:length(DataDep(:,1))
            DataVit(f,:) = vitdec([DataDep(f,:) 0 0 0 0], trellis, 1, 'cont','unquant');
        end
        
        DataVit = DataVit(:,2:end-6);      % Tail removing
        out_size = length(DataVit(1,:))
        if(idx ==10 )
            writeReal( DataVit(1,:), sprintf('../../data/ut/vitterbi_out_MSC_size2688_%d_%d.txt', idx, 0), out_size );
            writeReal( DataVit(2,:), sprintf('../../data/ut/vitterbi_out_MSC_size2688_%d_%d.txt', idx, 1), out_size );
            writeReal( DataVit(3,:), sprintf('../../data/ut/vitterbi_out_MSC_size2688_%d_%d.txt', idx, 2), out_size );
            writeReal( DataVit(4,:), sprintf('../../data/ut/vitterbi_out_MSC_size2688_%d_%d.txt', idx, 3), out_size );
        end
        if( DebugOn==1 )
            disp('#################### DataVit'); DataVit(1,1:5),
            SizeDataVit = size(DataVit), % NCIFsPerFrame x (NBitsAfterTimeDepunct/4-6)
            ERROR = SizeDataVit - [ NCIFsPerFrame, NBitsAfterTimeDepunct/4-6 ], pause
        end
        
        % Energy dispersal ------------------------------------------------------------------------
        DataEnerg = zeros(size(DataVit));
        for m=1:length(DataEnerg(:,1))
            DataEnerg(m,:) = xor( DataVit(m,:), DispSequense(1:length(DataEnerg(1,:))) );
        end
        
        if( DebugOn==1 )
            disp('#################### DataEnerg'); DataEnerg(1,1:5),
            SizeDataEnerg = size(DataEnerg), % NCIFsPerFrame x (NBitsAfterTimeDepunct/4-6)
            ERROR = SizeDataEnerg - [ NCIFsPerFrame, NBitsAfterTimeDepunct/4-6 ], pause
        end
        
        % Super Frame Handling --------------------------------------------------------------------
        
        if(SuperFrame2AAC==0) % Super Frame decoding - by outside program DABp2AAC.EXE
            % Audio data creation (from bits to bytes) -------------------------------------------------
            Audio=( reshape( DataEnerg',8,[] )' * [128 64 32 16 8 4 2 1]' )';
            % Write to a file (bytes to a file) --------------------------------------------------------
            fwrite(fid, Audio, 'uint8');  % file extension ".dat"
        end
        
        if(SuperFrame2AAC==1) % Super Frame decoding - below
            
            % size( NBitsPerFrame*CIFCount+1 : NBitsPerFrame*(CIFCount+4) ), size(reshape(DataEnerg',1,[])), pause
            % DataEnergChain( NBitsPerFrame*CIFCount+1 : NBitsPerFrame*(CIFCount+4) ) = reshape(DataEnerg',1,[]);   % reshaping to one row
            % CIFCount, 1+NBitsPerFrame*CIFCount, NDE+NBitsPerFrame*CIFCount, pause
            
            DataEng = reshape(DataEnerg',1,[]);
            NBitsPerCIF = (NBitsAfterTimeDepunct/4-6);
            NBitsPerFrame = NCIFsPerFrame * NBitsPerCIF;
            DataEnergChain( 1 + NBitsPerCIF*CIFCount : NBitsPerFrame + NBitsPerCIF*CIFCount ) = DataEng(1:NBitsPerFrame);
            
            if( DebugOn==1 )
                disp('#################### DataEnergChain'); DataEnergChain(1,1:5),
                CIFCount
                SizeDataEnergChain = size( DataEnergChain ),
                ERROR = SizeDataEnergChain - [ 1, ceil(5/NCIFsPerFrame)*NCIFsPerFrame*(NBitsAfterTimeDepunct/4-6) ], pause
            end
            
            % Super Frame Synchronisation and transcoding to ADTS container
            % Additional delay of 5 CIFs = one DAB+ Super Frame. Together we have delay of 16+5 = 21 CIFs
            [ CIFCount, SuperFrameSync, DataEnergChain, au_start, num_aus ] = SuperFrameHandlingX( CIFCount, SuperFrameSync, DataEnergChain, au_start, Audiokbps, lan, num_aus, NBitsPerCIF, NCIFsPerFrame, RSpolynomial, fid);
            
        end    % end of Super Frame decoding
        
    end    % end of UEP/EEP
    
    % ###########################################
end    % end of if (TimeInt buffer is full)
% ###########################################
