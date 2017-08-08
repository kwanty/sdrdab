% synchronizer, RAW SNR, MODE I only
% Jaroslaw Bulat kwant@agh.edu.pl 20.06.2016
%
% in matlab notation:
% xxxxxxxxxxxxxxxxxxxxxxx 0000000000000000  xxxxxxxxxxxxxxxxx
% 1      	       lowsig lowzero highzero  highsig  fft_size
% todo? zero? DC?

clear all;
fft_size = 2048;
number_of_carriers = 1536;

lowsig = fft_size/2 - (fft_size-number_of_carriers)/2;
lowzero = lowsig+1;
highzero = lowzero+(fft_size-number_of_carriers-1);
highsig = highzero+1;
guard = round((fft_size-number_of_carriers)*0.1);

idxsig = [1:lowsig, highsig:fft_size];
signalidx = [1:lowsig-guard, highsig+guard:fft_size];
noiseidx = lowzero+guard:highzero-guard;

% lowsig-1
% highzero-lowzero
% fft_size-highsig
expectedResults=zeros(1,5);

%%%%%% zeroSNR  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
rng('default');
n = randn(1, fft_size)+1i*randn(1, fft_size);
n = n./sqrt(var(n(noiseidx)));
s = randn(1, fft_size)+1i*randn(1, fft_size);
s = s./sqrt(var(s(signalidx)));

targetSNR = 0;  %[dB]
X = n;
X(idxsig) = X(idxsig) + sqrt(10^(targetSNR/10))*s(idxsig);        % not very precise, consider conj

signal = sum(X(signalidx).*conj(X(signalidx)))/length(signalidx);
noise = sum(X(noiseidx).*conj(X(noiseidx)))/length(noiseidx);
snr = 10*log10(signal/noise-1);

expectedResults(1) = snr;
x = 1/sqrt(fft_size)*ifft(X);
fp = fopen('../data/ut/synchronizer_SNR_in_zeroSNR.txt', 'w');
for idx = 1:length(x)
    fprintf(fp, '%1.18f\n%1.18f\n', real(x(idx)), imag(x(idx)));
end
fclose(fp);


%%%%%% lowSNR  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
rng('default');
n = randn(1, fft_size)+1i*randn(1, fft_size);
n = n./sqrt(var(n(noiseidx)));
s = randn(1, fft_size)+1i*randn(1, fft_size);
s = s./sqrt(var(s(signalidx)));

targetSNR = 8;  %[dB]
X = n;
X(idxsig) = X(idxsig) + sqrt(10^(targetSNR/10))*s(idxsig);        % not very precise, consider conj

signal = sum(X(signalidx).*conj(X(signalidx)))/length(signalidx);
noise = sum(X(noiseidx).*conj(X(noiseidx)))/length(noiseidx);
snr = 10*log10(signal/noise-1);

expectedResults(2) = snr;
x = 1/sqrt(fft_size)*ifft(X);
fp = fopen('../data/ut/synchronizer_SNR_in_lowSNR.txt', 'w');
for idx = 1:length(x)
    fprintf(fp, '%1.18f\n%1.18f\n', real(x(idx)), imag(x(idx)));
end
fclose(fp);

%%%%%% highSNR  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
rng('default');
n = randn(1, fft_size)+1i*randn(1, fft_size);
n = n./sqrt(var(n(noiseidx)));
s = randn(1, fft_size)+1i*randn(1, fft_size);
s = s./sqrt(var(s(signalidx)));

targetSNR = 20;  %[dB]
X = n;
X(idxsig) = X(idxsig) + sqrt(10^(targetSNR/10))*s(idxsig);        % not very precise, consider conj

signal = sum(X(signalidx).*conj(X(signalidx)))/length(signalidx);
noise = sum(X(noiseidx).*conj(X(noiseidx)))/length(noiseidx);
snr = 10*log10(signal/noise-1);
expectedResults(3) = snr;
x = 1/sqrt(fft_size)*ifft(X);
fp = fopen('../data/ut/synchronizer_SNR_in_highSNR.txt', 'w');
for idx = 1:length(x)
    fprintf(fp, '%1.18f\n%1.18f\n', real(x(idx)), imag(x(idx)));
end
fclose(fp);

%%%%%% infSNR  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
rng('default');
X = randn(1, fft_size)+1i*randn(1, fft_size);
X = X./sqrt(var(X));
noiseidx = lowzero:highzero;
X(noiseidx)=0;

signal = sum(X(signalidx).*conj(X(signalidx)))/length(signalidx);
noise = sum(X(noiseidx).*conj(X(noiseidx)))/length(noiseidx);
snr = 20*log10(signal/noise-1);

expectedResults(4) = snr;
x = 1/sqrt(fft_size)*ifft(X);
fp = fopen('../data/ut/synchronizer_SNR_in_infSNR.txt', 'w');
for idx = 1:length(x)
    fprintf(fp, '%1.18f\n%1.18f\n', real(x(idx)), imag(x(idx)));
end
fclose(fp);

%%%%%% minusinfSNR  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
rng('default');
X = randn(1, fft_size)+1i*randn(1, fft_size);
X(signalidx) = X(signalidx)./sqrt(var(X(signalidx)));
X(noiseidx) = X(noiseidx)./sqrt(var(X(noiseidx)));

% signal = sum(X(signalidx).*conj(X(signalidx)))/length(signalidx);
% noise = sum(X(noiseidx).*conj(X(noiseidx)))/length(noiseidx);
% snr = abs(signal/noise-1);
% snr = 20*log10(snr);
snr = -inf;

expectedResults(5) = snr;
x = 1/sqrt(fft_size)*ifft(X);
fp = fopen('../data/ut/synchronizer_SNR_in_minusInfSNR.txt', 'w');
for idx = 1:length(x)
    fprintf(fp, '%1.18f\n%1.18f\n', real(x(idx)), imag(x(idx)));
end
fclose(fp);

%%%%% save results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fp = fopen('../data/ut/synchronizer_SNR_expectedResults.txt', 'w');
for idx = 1:length(expectedResults)
    fprintf(fp, '%1.18f\n%1.18f\n', expectedResults(idx));
end
fclose(fp);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% d = load('../snr_14.txt');
% prf = load('../sigPhaseRef_freq.txt');
% idx=1:2048;
% di = d(idx*2-1) + 1i*d(idx*2);
% figure, plot( idx, real(di), 'b.', idx, imag(di), 'r.');
% 
% DI = fft(di);
% figure, plot( 10*log10(DI.*conj(DI)));
% 
% figure, plot( idx, prf(idx*2-1), 'b.', idx, prf(idx*2), 'r.'); title('phase reference');
% 
