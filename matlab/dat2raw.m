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

clear all
close all

if(0)
    Filename = fopen('../data/Kielce_mode2_iq.dat', 'rb');
    NSamples = 6e9;
    
    t = fread( Filename, [2, NSamples], 'float' );
    t = t-(mean(mean(t')));
    tmax = max(max(t'));
    tmin = min(min(t'));
    t = t./(max(tmax,-tmin));
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/Kielce_mode2_iq.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    
    fclose(Filename);
    fclose(Filename_w);
end


if(0)
    Filename = fopen('../data/Kielce_mode4_iq.dat', 'rb');
    NSamples = 6e9;
    
    t = fread( Filename, [2, NSamples], 'float' );
    t = t-(mean(mean(t')));
    tmax = max(max(t'));
    tmin = min(min(t'));
    t = t./(max(tmax,-tmin));
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/Kielce_mode4_iq.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    
    fclose(Filename);
    fclose(Filename_w);
end


if(0)
    Filename = fopen('../data/London_mode1_iq.dat', 'rb');
    NSamples = 6e9;
    
    t = fread( Filename, [2, NSamples], 'float' );
    t = t-(mean(mean(t')));
    tmax = max(max(t'));
    tmin = min(min(t'));
    t = t./(max(tmax,-tmin));
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/London_mode1_iq.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    
    fclose(Filename);
    fclose(Filename_w);
end


if(0)
    Filename = fopen('../data/London_mode3_iq.dat', 'rb');
    NSamples = 6e9;
    
    t = fread( Filename, [2, NSamples], 'float' );
    t = t-(mean(mean(t')));
    tmax = max(max(t'));
    tmin = min(min(t'));
    t = t./(max(tmax,-tmin));plo
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/London_mode3_iq.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    
    fclose(Filename);
    fclose(Filename_w);
end


if(1)
    Filename = fopen('../data/London_mode3p_iq.dat', 'rb');
    NSamples = 6e9;
    
    t = fread( Filename, [2, NSamples], 'float' );
    t = t-(mean(mean(t')));
    tmax = max(max(t'));
    tmin = min(min(t'));
    t = t./(max(tmax,-tmin));
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/London_mode3p_iq.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    
    fclose(Filename);
    fclose(Filename_w);
end

if(0)
    Filename = fopen('../data/Record1_Swiss.dat', 'rb');
    NSamples = 6e9;
    
    t = fread( Filename, [2, NSamples], 'float' );
    t = t-(mean(mean(t')));
    tmax = max(max(t'));
    tmin = min(min(t'));
    t = t./(max(tmax,-tmin));
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/Record1_Swiss.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    
    fclose(Filename);
    fclose(Filename_w);

    % resample to 2.048 MHz
    fs = 2.048e6;
    fs_file = 2.0e6
    t = resample(t, fs/1e3, fs_file/1e3); 
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/Record1_Swiss_2048kHz.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    fclose(Filename_w);
end


if(0)
    Filename = fopen('../data/Record2_Swiss.dat', 'rb');
    NSamples = 6e9;
    
    t = fread( Filename, [2, NSamples], 'float' );
    t = t-(mean(mean(t')));
    tmax = max(max(t'));
    tmin = min(min(t'));
    t = t./(max(tmax,-tmin));
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/Record2_Swiss.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    
    fclose(Filename);
    fclose(Filename_w);
    
    % resample to 2.048 MHz
    fs = 2.048e6;
    fs_file = 2.0e6
    t = resample(t, fs/1e3, fs_file/1e3); 
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(1,:);
    b(2:2:2*length(t))=t(2,:);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/Record2_Swiss_2048kHz.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    fclose(Filename_2);    
end


if(0)
    t = wavread('../data/SDRSharp_20140728_132746Z_227360kHz_IQ.wav');
    t = t-(mean(mean(t)));
    
    tmax = max(max(t));
    tmin = min(min(t));
    t = t./(max(tmax,-tmin));
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(:,1);
    b(2:2:2*length(t))=t(:,2);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/SDRSharp_20140728_132746Z_227360kHz_IQ.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    fclose(Filename_w);    
end


if(0)
    t = wavread('../data/SDRSharp_20150102_125222Z_229077kHz_IQ.wav');
    t = t-(mean(mean(t)));
    
    tmax = max(max(t));
    tmin = min(min(t));
    t = t./(max(tmax,-tmin));
    
    b = zeros(1, 2*length(t));
    b(1:2:2*length(t))=t(:,1);
    b(2:2:2*length(t))=t(:,2);
    
    b = 128 * b + 127;
    b = uint8(b);
    
    Filename_w = fopen('../data/SDRSharp_20150102_125222Z_229077kHz_IQ.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    fclose(Filename_w);    
end


if(0)
    t = wavread('../data/SDRSharp_20150604_145504Z_229070kHz_IQ_48dB.wav');
    t = t-(mean(mean(t)));
    
    tmax = max(max(t));
    tmin = min(min(t));
    t = t./(max(tmax,-tmin));
    t = 128 * t + 127;
    t = uint8(t);
    
    b = uint8(zeros(1, 2*length(t)));
    b(1:2:2*length(t))=t(:,1);
    b(2:2:2*length(t))=t(:,2);
    
    Filename_w = fopen('../data/SDRSharp_20150604_145504Z_229070kHz_IQ_48dB.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    fclose(Filename_w);    
end


if(0)
    t = wavread('../data/SDRSharp_20150604_221029Z_229070kHz_IQ_28dB.wav');
    t = t-(mean(mean(t)));
    
    tmax = max(max(t));
    tmin = min(min(t));
    t = t./(max(tmax,-tmin));
    t = 128 * t + 127;
    t = uint8(t);
    
    b = uint8(zeros(1, 2*length(t)));
    b(1:2:2*length(t))=t(:,1);
    b(2:2:2*length(t))=t(:,2);
    
    Filename_w = fopen('../data/SDRSharp_20150604_221029Z_229070kHz_IQ_28dB.raw', 'wb');
    fwrite( Filename_w, b, 'uint8' );
    fclose(Filename_w);    
end


% conversion verification
%
if(0)
    NSamples = 1e9;
    filename = '../data/Record1_Swiss.raw';
    fp = fopen(filename, 'rb');
    x = fread( fp, [2, NSamples], 'uint8' );
    fclose( fp );
    
    figure,
    plot( x' ); hold on;
end

