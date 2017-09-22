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
%
% Code modified by:
% (c) Szymon Bar
% Krakow, Poland
% 03.04.2017
% szbar@student.agh.edu.pl
%
% Code modified/extended (to DAB+ more cases) by:
% (c) Mateusz Ziarko
% Krakow, Poland
% 03.04.2017
% mat.ziarko@gmail.com
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function datadecoder_generateDepuncturer

%generating MSC Data (DAB)

samples_data = '../data/ut/depuncturer_data_description_dab.txt'; % file to save test parameters in

kbps = [32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384];
pi5 = 8; % pi5 always equals 8, because it is a pi vector for the last 24 bits (ETSI EN 300 401, p. 131 bottom)

for rate = 1 : 14
    for protection = 1 : 5
        % some bitrates do not have certain protection levels, these cases are cut out
        if ( ((kbps(rate)==56 || kbps(rate)==112 || kbps(rate)==320) && protection==1) || (kbps(rate)==384 && protection==2) || (kbps(rate)==320 && protection==3) || (kbps(rate)==384 && protection==4) )
            continue
        end
        % get ranges, pi and padding for given bitrates and protection levels
        [range1, range2, range3, range4, pi1, pi2, pi3, pi4, pad] = UEPTab(kbps(rate), protection);
        populate = 1;
        
        % populate  all blocks with evenly spaced numbers with separation of 1 between two following numbers
        block1 = zeros(4,range1);
        block2 = zeros(4,range2-range1);
        block3 = zeros(4,range3-range2);
        block4 = zeros(4,range4-range3);
        block5 = zeros(4,12);
        
        for i = 1:4
            for j = 1:length(block1)
                block1(i,j) = populate;
                populate = populate + 1;
            end
        
            for j = 1:length(block2)
                block2(i,j) = populate;
                populate = populate + 1;
            end
        
            for j = 1:length(block3)
                block3(i,j) = populate;
                populate = populate + 1;
            end
        
            for j = 1:length(block4)
                block4(i,j) = populate;
                populate = populate + 1;
            end
        
            for j = 1:length(block5)
                block5(i,j) = populate;
                populate = populate + 1;
            end
        end
        
        padding = zeros(1, pad); % the padding will be zeros
        
        if (pi4 == 0)
            DataWithoutPad = [block1 block2 block3 block5];
            DataWithPad = [DataWithoutPad(1,1:end) padding DataWithoutPad(2,1:end) padding DataWithoutPad(3,1:end) padding DataWithoutPad(4,1:end) padding];
            DepuncturedData = [depuncturing(block1,pi1) depuncturing(block2,pi2) depuncturing(block3,pi3) depuncturing(block5,pi5)];
        else
            DataWithoutPad = [block1 block2 block3 block4 block5];
            DataWithPad = [DataWithoutPad(1,1:end) padding DataWithoutPad(2,1:end) padding DataWithoutPad(3,1:end) padding DataWithoutPad(4,1:end) padding];
            DepuncturedData = [depuncturing(block1,pi1) depuncturing(block2,pi2) depuncturing(block3,pi3) depuncturing(block4,pi4) depuncturing(block5,pi5)];
        end
        
        % create proper file names
        data_rate = num2str(kbps(rate));
        prot = num2str(protection);
        inFile = strcat('../data/ut/depuncturer_data_in_msc_kbps_',data_rate,'_prot_',prot,'_dab','.txt');
        outFile = strcat('../data/ut/depuncturer_data_out_msc_kbps_',data_rate,'_prot_',prot,'_dab','.txt');
        
        % file names for C++ unit tests (we run unittest from superior catalogue)
        inFileTest = strcat('./data/ut/depuncturer_data_in_msc_kbps_',data_rate,'_prot_',prot,'_dab','.txt');
        outFileTest = strcat('./data/ut/depuncturer_data_out_msc_kbps_',data_rate,'_prot_',prot,'_dab','.txt');
        
        % save test parameters to file
        fn = fopen(samples_data, 'a');
        fprintf(fn, '%s %s %d %d\n', inFileTest, outFileTest, kbps(rate), protection);
        fclose(fn);
        
        % save data to files
        fp = fopen(inFile,'w');
        fprintf( fp, '%d\n', DataWithPad );
        fclose( fp );

        fp = fopen(outFile,'w');
        for i=1:4
            fprintf( fp, '%d\n', DepuncturedData(i,:) );
        end
        fclose( fp );
    end
end

% generating MSC Data DAB+
% Protection type A (general)

PI3 = 8;

samples_data = '../data/ut/depuncturer_data_description_dabp.txt';
in = '../data/ut/depuncturer_data_in_msc';
out = '../data/ut/depuncturer_data_out_msc';
in_c = './data/ut/depuncturer_data_in_msc';
out_c = './data/ut/depuncturer_data_out_msc';

% Data acquired from ETSI 300 401 
% Based on Page 51 Table no 7 & Page 136 Table 33

protection = ['1', 'A'; '2', 'A'; '3', 'A'; '4', 'A'];
multiplier = [12, 8, 6, 4];



for i = 1 : 4
    % maximum data rate for n*8kbit/s is 1728, 1728/8 gives 216
    for n = 1:216 % all cases
    	% get PIs and Ranges for given protection levels and sub-channel sizes
        SubChSize = multiplier(i)*n;
        [Audiokbps, PI1, PI2, L1Range, L2Range] = EEPTab(protection(i,:), SubChSize);
        
        % fill the blocks with predictable data
        block1 = zeros(4, L1Range);
        block2 = zeros(4, L2Range - L1Range);
        block3 = zeros(4, 12);
        content = 1;
        
        for x = 1:4
            
            for y = 1:length(block1)
                block1(x,y) = content;
                content = content + 1;
            end
            
            for y = 1:length(block2)
                block2(x,y) = content;
                content = content + 1;
            end
            
            for y = 1:length(block3)
                block3(x,y) = content;
                content = content + 1;
            end
            
        end
        
        Data = [ block1 block2 block3 ];
        DepuncturedData = [depuncturing(block1, PI1) depuncturing(block2, PI2) depuncturing(block3, PI3)];        % set proper variables
        
        % set proper variables
        prot = strcat(protection(i,1), '-', protection(i,2));
        SubChSizeStr = num2str(SubChSize);
        c_prot = str2double(protection(i,1))-1; % protection in convention defined in C++ implementation
        
        % create filenames
        inFile = strcat(in,'_',prot,'_',SubChSizeStr,'_','dabp.txt');
        outFile = strcat(out,'_',prot,'_',SubChSizeStr,'_','dabp.txt');
        
        inFile_c = strcat(in_c,'_',prot,'_',SubChSizeStr,'_','dabp.txt');
        outFile_c = strcat(out_c,'_',prot,'_',SubChSizeStr,'_','dabp.txt');
       
        fn = fopen(samples_data, 'a');
        fprintf(fn, '%s %s %d %d %d\n', inFile_c, outFile_c, SubChSize, c_prot, 0);
        fclose(fn);	

        % save acquired data to files
        fp = fopen(inFile,'w');
        for a=1:4
            fprintf( fp, '%d\n', Data(a,:) );
        end
        fclose(fp);
        
        
        fp = fopen(outFile,'w');
        for a=1:4
            fprintf( fp, '%d\n', DepuncturedData(a,:) );
        end
        fclose(fp);
        
    end
    
end

% Protection type B (general)

% Data acquired from ETSI 300 401 
% Based on Page 51 Table no 8 & Page 136 Table 35

protection = ['1', 'B'; '2', 'B'; '3', 'B'; '4', 'B'];
multiplier = [27, 21, 18, 15];
PI3 = 8;

for i = 1 : 4
   % maximum data rate for n*32kbit/s is 1824, 1728/32 gives 57
   for n = 1:57 % all cases
        SubChSize = multiplier(i)*n;
        
        [Audiokbps, PI1, PI2, L1Range, L2Range] = EEPTab(protection(i,:), SubChSize);
        block1 = zeros(4, L1Range);
        block2 = zeros(4, L2Range - L1Range);
        block3 = zeros(4, 12);
        content = 1;
        
        for x = 1:4
            
            for y = 1:length(block1)
                block1(x,y) = content;
                content = content + 1;
            end
            
            for y = 1:length(block2)
                block2(x,y) = content;
                content = content + 1;
            end
            
            for y = 1:length(block3)
                block3(x,y) = content;
                content = content + 1;
            end
            
        end
        
        Data = [ block1 block2 block3 ];
        DepuncturedData = [depuncturing(block1, PI1) depuncturing(block2, PI2) depuncturing(block3, PI3)];
        
        % set proper variables
        prot = strcat(protection(i,1), '-', protection(i,2));
        c_prot = str2double(protection(i,1))-1; % protection in convention defined in C++ implementation
        SubChSizeStr = num2str(SubChSize);
        
        % create filenames
        inFile = strcat(in,'_',prot,'_',SubChSizeStr,'_','dabp.txt');
        outFile = strcat(out,'_',prot,'_',SubChSizeStr,'_','dabp.txt');
        
        inFile_c = strcat(in_c,'_',prot,'_',SubChSizeStr,'_','dabp.txt');
        outFile_c = strcat(out_c,'_',prot,'_',SubChSizeStr,'_','dabp.txt');
        
        fn = fopen(samples_data, 'a');
        fprintf(fn, '%s %s %d %d %d\n', inFile_c, outFile_c, SubChSize, c_prot, 1);
        fclose(fn);	
        
        % save acquired data to files
        fp = fopen(inFile,'w');
        for a=1:4
                fprintf( fp, '%d\n', Data(a,:) );
        end
        fclose(fp);
        
        fp = fopen(outFile,'w');
        for a=1:4
                fprintf( fp, '%d\n', DepuncturedData(a,:) );
        end
        fclose(fp);
    end
    
end

%GENERATING MSC DATA (DAB+)
block1 = randi(2.99e11,4,5184); pi1 = 8;
block2 = randi(2.99e11,4,180); pi2 = 7;
block3 = randi(2.99e11,4,12); pi3 = 8;
pad = randi(1e4,1,768);
%

DataWithoutPad = [block1 block2 block3];
DataWithPad = [DataWithoutPad(1,1:end) pad DataWithoutPad(2, 1:end) pad DataWithoutPad(3, 1:end) pad DataWithoutPad(4, 1:end)];
DepuncturedData = [depuncturing(block1,pi1) depuncturing(block2,pi2) depuncturing(block3,pi3)];

fp = fopen('../data/ut/depuncturer_data_in_msc_8_7_8_dabp.txt','w');
for i=1:length(DataWithPad)
   fprintf( fp, '%f\n', DataWithPad(i) );
end
fclose( fp );

fp = fopen('../data/ut/depuncturer_data_out_msc_8_7_8_dabp.txt','w');
for i=1:4
    for l=1:length(DepuncturedData)
        fprintf( fp, '%f\n', DepuncturedData(i,l) );
    end
end
fclose( fp );

% GENERATING FIC DATA
% for modes I, II, IV
populate = 1;

block1 = zeros(4,2016); pi1 = 16;
block2 = zeros(4,276); pi2 = 15;
block3 = zeros(4,12); pi3 = 8;

for i = 1:4
    for j = 1:length(block1)
        block1(i,j) = populate;
        populate = populate + 1;
    end
end

for i = 1:4
    for j = 1:length(block2)
        block2(i,j) = populate;
        populate = populate + 1;
    end
end

for i = 1:4
    for j = 1:length(block3)
        block3(i,j) = populate;
        populate = populate + 1;
    end
end

DataWithoutPad = [block1 block2 block3]; % in FIC there is no pad
DepuncturedData = [depuncturing(block1,pi1) depuncturing(block2,pi2) depuncturing(block3,pi3)];

fp = fopen('../data/ut/depuncturer_data_in_fic_16_15_8_modes_1_2_4.txt','w');
for i = 1:4
    for j = 1:length(DataWithoutPad)
        fprintf( fp, '%f\n', DataWithoutPad(i,j) );
    end
end
fclose( fp );

fp = fopen('../data/ut/depuncturer_data_out_fic_16_15_8_modes_1_2_4.txt','w');
for i = 1:4
    for j = 1:length(DepuncturedData)
        fprintf( fp, '%f\n', DepuncturedData(i,j) );
    end
end
fclose( fp );

% for mode III
populate = 1;

block1 = zeros(4,2784); pi1 = 16;
block2 = zeros(4,276); pi2 = 15;
block3 = zeros(4,12); pi3 = 8;

for i = 1:4
    for j = 1:length(block1)
        block1(i,j) = populate;
        populate = populate + 1;
    end
end

for i = 1:4
    for j = 1:length(block2)
        block2(i,j) = populate;
        populate = populate + 1;
    end
end

for i = 1:4
    for j = 1:length(block3)
        block3(i,j) = populate;
        populate = populate + 1;
    end
end

DataWithoutPad = [block1 block2 block3]; % in FIC there is no pad
DepuncturedData = [depuncturing(block1,pi1) depuncturing(block2,pi2) depuncturing(block3,pi3)];

fp = fopen('../data/ut/depuncturer_data_in_fic_16_15_8_mode_3.txt','w');
for i = 1:4
    for j = 1:length(DataWithoutPad)
        fprintf( fp, '%f\n', DataWithoutPad(i,j) );
    end
end
fclose( fp );

fp = fopen('../data/ut/depuncturer_data_out_fic_16_15_8_mode_3.txt','w');
for i = 1:4
    for j = 1:length(DepuncturedData)
        fprintf( fp, '%f\n', DepuncturedData(i,j) );
    end
end
fclose( fp );



end
