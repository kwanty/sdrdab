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

function [ PhaseRefSymb, sigPhaseRefSymb ] = PhaseRefSymbGen( Mode, PlotON )

% Generation of Phase Reference Symbol in Time & Freq, ETSI pp. 147-149

global NSymbPerFrame NSampPerFrame NSampPerSymb NSampPerNull Nfft NSampPerGuard

  
%#############

if( Mode == 1)
  
   % Table 39, page 148, only for mode=1
   %     kmin  kmax    k'  i  n 
   tab=[ -768  -737  -768  0  1;
         -736  -705  -736  1  2;
         -704  -673  -704  2  0;
         -672  -641  -672  3  1;
         -640  -609  -640  0  3;
         -608  -577  -608  1  2;
         -576  -545  -576  2  2;
         -544  -513  -544  3  3;
         -512  -481  -512  0  2;
         -480  -449  -480  1  1;
         -448  -417  -448  2  2;
         -416  -385  -416  3  3;
         -384  -353  -384  0  1;
         -352  -321  -352  1  2;
         -320  -289  -320  2  3;
         -288  -257  -288  3  3;
         -256  -225  -256  0  2;
         -224  -193  -224  1  2;
         -192  -161  -192  2  2;
         -160  -129  -160  3  1;
         -128   -97  -128  0  1;
          -96   -65   -96  1  3;
          -64   -33   -64  2  1;
          -32    -1   -32  3  2;
            1    32     1  0  3
           33    64    33  3  1;
           65    96    65  2  1;
           97   128    97  1  1;
          129   160   129  0  2;
          161   192   161  3  2;
          193   224   193  2  1;
          225   256   225  1  0;
          257   288   257  0  2;
          289   320   289  3  2;
          321   352   321  2  3;
          353   384   353  1  3;
          385   416   385  0  0;
          417   448   417  3  2;
          449   480   449  2  1;
          481   512   481  1  3;
          513   544   513  0  3;
          545   576   545  3  3;
          577   608   577  2  3;
          609   640   609  1  0;
          641   672   641  0  3;
          673   704   673  3  0;
          705   736   705  2  1;
          737   768   737  1  1 ];
      
   Nfft = 2048,
   Ncarriers = 768;
end

%#############

if( Mode == 2)
   % Table 40, page 148, only for mode=2
   %     kmin  kmax    k'  i  n 
   tab=[ -192  -161  -192  0  2;
         -160  -129  -160  1  3;
         -128   -97  -128  2  2;
          -96   -65   -96  3  2;
          -64   -33   -64  0  1;
          -32    -1   -32  1  2;
            1    32     1  2  0;
           33    64    33  1  2;
           65    96    65  0  2;
           97   128    97  3  1;
          129   160   129  2  0;
          161   192   161  1  3];
   Nfft = 512,
   Ncarriers = 192;
end

%#############

if( Mode == 3)
   % Table 41, page 148, only for mode=3
   %     kmin  kmax    k'  i  n 
   tab=[  -96   -65   -96  0  2;
          -64   -33   -64  1  3;
          -32    -1   -32  2  0;
            1    32     1  3  2;
           33    64    33  2  2;
           65    96    65  1  2];
   Nfft = 256,
   Ncarriers = 96;    
end

%#############

if( Mode == 4)
   % Table 42, page 149, only for mode=4
   %     kmin  kmax    k'  i  n 
   tab=[ -384  -353  -384  0  0;
         -352  -321  -352  1  1;
         -320  -289  -320  2  1;
         -288  -257  -288  3  2;
         -256  -225  -256  0  2;
         -224  -193  -224  1  2;
         -192  -161  -192  2  0;
         -160  -129  -160  3  3;
         -128   -97  -128  0  3;
          -96   -65   -96  1  1;
          -64   -33   -64  2  3;
          -32    -1   -32  3  2;
            1    32     1  0  0;
           33    64    33  3  1;
           65    96    65  2  0;
           97   128    97  1  2;
          129   160   129  0  0;
          161   192   161  3  1;
          193   224   193  2  2;
          225   256   225  1  2;
          257   288   257  0  2;
          289   320   289  3  1;
          321   352   321  2  3;
          353   384   353  1  0];
    Nfft = 1024,
    Ncarriers = 384;    
end

%#############

Kside = Ncarriers;
Kcentr = Ncarriers+1;
Kmax = 2*Ncarriers+1; 
[ K, L ] = size(tab);  % K=freq zones, L=freq params

% Table 43, page 148
% j=  0 1 2 3 4 5 6 7 8 9..........15  16............................31
  h = [ 0 2 0 0 0 0 1 1 2 0 0 0 2 2 1 1   0 2 0 0 0 0 1 1 2 0 0 0 2 2 1 1; ...  % h0,j
        0 3 2 3 0 1 3 0 2 1 2 3 2 3 3 0   0 3 2 3 0 1 3 0 2 1 2 3 2 3 3 0; ...  % h1,j
        0 0 0 2 0 2 1 3 2 2 0 2 2 0 1 3   0 0 0 2 0 2 1 3 2 2 0 2 2 0 1 3; ...  % h2,j
        0 1 2 1 0 3 3 2 2 3 2 1 2 1 3 2   0 1 2 1 0 3 3 2 2 3 2 1 2 1 3 2 ];    % h3,j

% Equation page 147: fi(k)=pi/2*(h(i+1,k-k'+1)+n) - i,k',n from table tab  
  PhaseRefSymb(Kcentr) = 0;
  for k = 1 : K                       % over freq zones
    for kk = tab(k,1) : tab(k,2)      % from-to freq indexes
      % fi(k)         =  pi/2 *( h( i       +1, kk-   k'   +1)     +n     );   % Eq. page 147
        fi(kk+Kcentr) = (pi/2)*( h( tab(k,4)+1, kk-tab(k,3)+1) + tab(k,5) );   %
        work(kk+Kcentr) = exp( j*fi(kk+Kcentr) );                              % Eg. page 147
    end    
  end
  PhaseRefSymb = zeros(1,Nfft);
  PhaseRefSymb(1:Kcentr) = work(Kcentr:Kmax);             % "low" frequencies
  PhaseRefSymb(end:-1:end-Kside+1) = work(Kside:-1:1);    %  only

  PhaseRefSymb = PhaseRefSymb.';

  sigref = ifft(PhaseRefSymb);
  sigPhaseRefSymb = [ sigref(end-NSampPerGuard+1:1:end); sigref ];  % adding Cyclic Prefix

  if(PlotON)
     disp('sigPhaseRefSymb(1:10)'); sigPhaseRefSymb(1:10),
     figure('Name','PHASE REFERENCE');
     subplot(211); stem(PhaseRefSymb); title('Spectrum of the Phase Ref Symbol'),
     subplot(212); stem(sigPhaseRefSymb); title('Time Waveform of Phase Ref Symbol'); pause
     subplot(111);
  end



    