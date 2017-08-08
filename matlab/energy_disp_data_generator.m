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

clear all;
%mode1
cifs=4;
fibs = 3;
%sizet= fibs * 256;
sizet = fibs * 256;
%FIC 3 * 256 bits
DataVitFIC=round(rand(cifs,sizet));
DatkaFIC=reshape(DataVitFIC',[],1);
%MSC (4 * 10848)-24
afterdepmin = (2694 - 6);
DataVitMSC=round(rand(cifs,afterdepmin));
DatkaMSC=reshape(DataVitMSC',[],1);

vectorFIC = EnergyDispGen(sizet);
vectorMSC = EnergyDispGen(afterdepmin);

for m=1:cifs
        DataEnergFIC(m,:) = xor( DataVitFIC(m,:), vectorFIC );
        DataEnergMSC(m,:) = xor( DataVitMSC(m,:), vectorMSC );     
end   
DataEnergFIC = reshape(DataEnergFIC',[],1);
DataEnergMSC = reshape(DataEnergMSC',[],1);

fileID = fopen('../data/ut/FICenergyInputForCpp.txt','wt');
fprintf(fileID,'%d\n',DatkaFIC);
fclose(fileID);

fileID = fopen('../data/ut/MSCenergyInputForCpp.txt','wt');
fprintf(fileID,'%d\n',DatkaMSC);
fclose(fileID);

% Datafinal=reshape(DataEnergFIC,8,[])';
% kx=size(DataEnergFIC)/8;
% for i=1:kx(1)
%     Datafinal2(i,:)=bi2de(Datafinal(i,:),'left-msb');
% end

fileID = fopen('../data/ut/FICenergyOutputFromMatlab.txt','wt');
fprintf(fileID,'%d\n',DataEnergFIC);
fclose(fileID);

fileID = fopen('../data/ut/MSCenergyOutputFromMatlab.txt','wt');
fprintf(fileID,'%d\n',DataEnergMSC);
fclose(fileID);

%%%%%%%%CONST DATA
ficOne = ones(cifs, sizet);
ficZero = zeros(cifs, sizet);
mscOne = ones(cifs, afterdepmin);
mscZero = zeros(cifs, afterdepmin);

for m=1:cifs
        DataEnergFICone(m,:) = xor( ficOne(m,:),vectorFIC  );
        DataEnergMSCone(m,:) = xor( mscOne(m,:), vectorMSC );     
end   

for m=1:cifs
        DataEnergFICzero(m,:) = xor( ficZero(m,:),vectorFIC  );
        DataEnergMSCzero(m,:) = xor( mscZero(m,:), vectorMSC );     
end

DataEnergFICone = reshape(DataEnergFICone',[],1);
DataEnergMSCone = reshape(DataEnergMSCone',[],1);
DataEnergFICzero = reshape(DataEnergFICzero',[],1);
DataEnergMSCzero = reshape(DataEnergMSCzero',[],1);

fileID = fopen('../data/ut/energyFIConeOUT.txt','wt');
fprintf(fileID,'%d\n',DataEnergFICone);
fclose(fileID);

fileID = fopen('../data/ut/energyFICzeroOUT.txt','wt');
fprintf(fileID,'%d\n',DataEnergFICzero);
fclose(fileID);

fileID = fopen('../data/ut/energyMSConeOUT.txt','wt');
fprintf(fileID,'%d\n',DataEnergMSCone);
fclose(fileID);

fileID = fopen('../data/ut/energyMSCzeroOUT.txt','wt');
fprintf(fileID,'%d\n',DataEnergMSCzero);
fclose(fileID);

ficOne = reshape(ficOne',[],1);
ficZero = reshape(ficZero',[],1);
mscOne = reshape(mscOne',[],1);
mscZero = reshape(mscZero',[],1);


fileID = fopen('../data/ut/energyFIConeIN.txt','wt');
fprintf(fileID,'%d\n',ficOne);
fclose(fileID);

fileID = fopen('../data/ut/energyFICzeroIN.txt','wt');
fprintf(fileID,'%d\n',ficZero);
fclose(fileID);

fileID = fopen('../data/ut/energyMSConeIN.txt','wt');
fprintf(fileID,'%d\n',mscOne);
fclose(fileID);

fileID = fopen('../data/ut/energyMSCzeroIN.txt','wt');
fprintf(fileID,'%d\n',mscZero);
fclose(fileID);

%%%%%%%%%%%vectors
vectorFIC=vectorFIC';
vectorMSC=vectorMSC';

fileID = fopen('../data/ut/FICenergyVector.txt','wt');
fprintf(fileID,'%d\n',vectorFIC);
fclose(fileID);

fileID = fopen('../data/ut/MSCenergyVector.txt','wt');
fprintf(fileID,'%d\n',vectorMSC);
fclose(fileID);
