% Kwant 21.02.2015
% gen data for FFT
% data format: interleaved REAL and IMAG component

clear;
close all;

fprintf('\n*************** FFT generator started ***************\n\n');

rng( 1 );   % seed

x = randn(4096, 1)+1i*randn(4096,1);
x = single(x);              % single precission
X = fft(x);

fp = fopen( '../data/ut/fft_engine_vol1_input.txt', 'w' );
for l=1:length(x)
    fprintf( fp, '%1.18f\n', real(x(l)) );
    fprintf( fp, '%1.18f\n', imag(x(l)) );
end
fclose( fp );

fp = fopen( '../data/ut/fft_engine_vol1_expectedOutput.txt', 'w' );
for l=1:length(X)
    fprintf( fp, '%1.18f\n', real(X(l)) );
    fprintf( fp, '%1.18f\n', imag(X(l)) );
end
fclose( fp );

