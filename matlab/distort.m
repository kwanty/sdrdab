% Help message
%
% DISTORT function created to set awgn, fs and fc drifts to clear recording
% usefull to distort recording in controlled way
% allows to check synchronization time in different scenarios
%
% distort(filename_r, filename_w, struct_table)
% Filename_w saved file uint8 0-255
% filename_w in .raw format
% filename_r in .dat format 'float' or .raw format 'uint8'
%
% You can create many different time intervals by adding structures to
% struct_table with variable settings
% struct_table = [data1 data2 ... dataN];
% dataN=struct('fs_drift',[],'fc_drift',[],'snr',[],'time_begin',[],'time_end',[]);
%
% fs_drift - set fs drift in Hz
% fc_drift - set fc drift in Hz
% snr - add awgn with snr level provided and measured
% time_begin - in which moment set distortion
% time_end - in which moment remove distortion
% fs is set to 2048e3 you can change it at the beggining of the function code
%
% Example:
% data1 = struct('fs_drift',[10],'fc_drift',[10],'snr',[10],'time_begin',[7],'time_end',[30]);
% data2 = struct('fs_drift',[10],'fc_drift',[10],'snr',[0],'time_begin',[10],'time_end',[25]);
% data3 = struct('fs_drift',[10],'fc_drift',[10],'snr',[0],'time_begin',[14],'time_end',[20]);
% enter_table=[data1 data2 data3];
% Filename_w=distort(example.dat, example.raw, enter_table);
%
% using fs_drift or fc_drift in the same interval more than once will cause
% summarize drifts
% in example above in interval 14-20 drift is set to 30, in intervals 10-14
% and 20-25 is set to 20 etc.
% in case of any questions reach me on email milosz.sliwinski96@gmail.com
%
% For more information, see <a href="matlab:
% web('http://www.mathworks.com')">the MathWorks Web site</a>.

if(0) %should be set to 0 unless you want to use test data
    %test data
    clear all
    close all
    time=10;
    
    filename_r = 'Record3_katowice_iq.dat'; %test value -- sdat=519045120 bytes
    filename_w = 'Record3_katowice_iq_test.raw'; %test value -- 129,761,280 bytes
    %data = struct('fs_drift',[],'fc_drift',[],'snr',[],'time_begin',[],'time_end',[]);
    %struct_table=[data data data];
    
    count=3;
    data1 = struct('fs_drift',[50],'fc_drift',[50],'snr',[10],'time_begin',[4],'time_end',[26]);
    data2 = struct('fs_drift',[50],'fc_drift',[50],'snr',[10],'time_begin',[8],'time_end',[22]);
    data3 = struct('fs_drift',[50],'fc_drift',[50],'snr',[10],'time_begin',[12],'time_end',[18]);
    enter_table=[data1 data2 data3];
    
    struct_table=[];
    for i=1:count
        if(floor(enter_table(i).time_end/time)-floor(enter_table(i).time_begin/time))
            k=floor(enter_table(i).time_end/time)-floor(enter_table(i).time_begin/time);
            %time_end=enter_table(i).time_begin+mod(-enter_table(i).time_begin,10);
            for j=0:k
                if(~j)
                    struct_element = struct('fs_drift',enter_table(i).fs_drift,'fc_drift',enter_table(i).fc_drift,'snr',enter_table(i).snr,'time_begin',enter_table(i).time_begin,'time_end',enter_table(i).time_begin+mod(-enter_table(i).time_begin,time));
                elseif(j && k-j)
                    struct_element = struct('fs_drift',enter_table(i).fs_drift,'fc_drift',enter_table(i).fc_drift,'snr',enter_table(i).snr,'time_begin',enter_table(i).time_begin+mod(-enter_table(i).time_begin,time)+(j-1)*time,'time_end',enter_table(i).time_begin+mod(-enter_table(i).time_begin,time)+j*time);
                elseif(~(k-j))
                    struct_element = struct('fs_drift',enter_table(i).fs_drift,'fc_drift',enter_table(i).fc_drift,'snr',enter_table(i).snr,'time_begin',enter_table(i).time_begin+mod(-enter_table(i).time_begin,time)+(j-1)*time,'time_end',enter_table(i).time_end);
                end %if
                struct_table = [struct_table struct_element];
            end %for
        end %if
    end %for
    
end %if() test data

%fs,fc drifts in Hz
function distort(filename_r, filename_w, enter_table)
good_data_flag = 0;
pattern1 = regexptranslate('wildcard','*.dat');
pattern2 = regexptranslate('wildcard','*.raw');
if(regexp(filename_r,pattern1))
    good_data_flag = 1;
    file_type='dat';
end %if
if(regexp(filename_r,pattern2))
    good_data_flag = 1;
    file_type='raw';
end %if
count=size(enter_table,2);
% table(1).fs_drift
%if value = 0 than there is no change
%max file lenght = 10s
time = 10; %in seconds
fs = 2048e3; %fs used
%each read sample is 'double' 64 (8)
len = fs*time; %~2e7

Filename = fopen(filename_r, 'rb');
Filename_w = fopen(filename_w, 'wb');
NSamples = [2, len]; %original NSamples=6e9
size_dat=dir(filename_r);
if file_type=='dat'
    file_size = size_dat.bytes/fs/time/8;
elseif file_type=='raw'
    file_size = size_dat.bytes/fs/time/8*4;
end %if
clear size_dat;
%check if time_end values meets file length
for i=1:count
    checking=floor(file_size*10);
    if(enter_table(i).time_end>checking)
        good_data_flag=0;
    end %if
end % for checking
struct_table=[];
%divide enter_table into smaller struct_tables
if(good_data_flag)
    for i=1:count
        if(floor(enter_table(i).time_end/time)-floor(enter_table(i).time_begin/time))
            k=floor(enter_table(i).time_end/time)-floor(enter_table(i).time_begin/time);
            %time_end=enter_table(i).time_begin+mod(-enter_table(i).time_begin,10);
            for j=0:k
                if(~j)
                    struct_element = struct('fs_drift',enter_table(i).fs_drift,'fc_drift',enter_table(i).fc_drift,'snr',enter_table(i).snr,'time_begin',enter_table(i).time_begin,'time_end',enter_table(i).time_begin+mod(-enter_table(i).time_begin,time));
                elseif(j && k-j)
                    struct_element = struct('fs_drift',enter_table(i).fs_drift,'fc_drift',enter_table(i).fc_drift,'snr',enter_table(i).snr,'time_begin',enter_table(i).time_begin+mod(-enter_table(i).time_begin,time)+(j-1)*time,'time_end',enter_table(i).time_begin+mod(-enter_table(i).time_begin,time)+j*time);
                elseif(~(k-j))
                    struct_element = struct('fs_drift',enter_table(i).fs_drift,'fc_drift',enter_table(i).fc_drift,'snr',enter_table(i).snr,'time_begin',enter_table(i).time_begin+mod(-enter_table(i).time_begin,time)+(j-1)*time,'time_end',enter_table(i).time_end);
                end %if
                struct_table = [struct_table struct_element];
            end %for
        end %if
    end %for
    
    
    counter=size(struct_table,2);
    file_size=floor(file_size);
    for i=0:1:file_size
        
        %20480000
        %16220160
        if file_type=='dat'
            %normalizing vector t and set mean value to 0
            t = fread(Filename, NSamples, 'float'); %327680000 - number of bytes read
            t = t-(mean(mean(t')));
            tmax = max(max(t'));
            tmin = min(min(t'));
            t = t./(max(tmax,-tmin));
            %one value, one sample
        elseif file_type=='raw'
            %normalizing vector t and set mean value to 0
            t = fread(Filename, NSamples, 'uint8'); %327680000/4 - number of bytes read
            t = t-(mean(mean(t')));
            tmax = max(max(t'));
            tmin = min(min(t'));
            t = t./(max(tmax,-tmin));
            %one value, one sample
        end
        %t(1,:) - real t(2,:) - imag
        %change fc
        for j=1:counter
            if(struct_table(j).fc_drift && ~floor(struct_table(j).time_begin/time-i))
                if(~mod(struct_table(j).time_end,time))
                    change_fragment=t(1,mod(struct_table(j).time_begin,time)*fs+1:10*fs)+1i*t(2,mod(struct_table(j).time_begin,time)*fs+1:10*fs);
                    %rotate now
                    rotated=[1:1:length(change_fragment)];
                    rotated=exp(1i*2*pi*struct_table(j).fc_drift/fs*rotated);
                    rotated=change_fragment.*rotated;
                    t(1,mod(struct_table(j).time_begin,time)*fs+1:10*fs)=real(rotated);
                    t(2,mod(struct_table(j).time_begin,time)*fs+1:10*fs)=imag(rotated);
                else
                    change_fragment=t(1,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs)+1i*t(2,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs);
                    %rotate now
                    rotated=[1:1:length(change_fragment)];
                    rotated=exp(1i*2*pi*struct_table(j).fc_drift/fs*rotated);
                    rotated=change_fragment.*rotated;
                    t(1,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs)=real(rotated);
                    t(2,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs)=imag(rotated);
                end %if
            end %if
        end %for
        %AWGN
        for j=1:counter
            if(struct_table(j).snr && ~floor(struct_table(j).time_begin/time-i))
                if(~mod(struct_table(j).time_end,time))
                    change_fragment=t(1,mod(struct_table(j).time_begin,time)*fs+1:10*fs)+1i*t(2,mod(struct_table(j).time_begin,time)*fs+1:10*fs);
                    change_fragment=awgn(change_fragment,struct_table(j).snr, 'measured');
                    t(1,mod(struct_table(j).time_begin,time)*fs+1:10*fs)=real(change_fragment);
                    t(2,mod(struct_table(j).time_begin,time)*fs+1:10*fs)=imag(change_fragment);
                else
                    change_fragment=t(1,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs)+1i*t(2,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs);
                    change_fragment=awgn(change_fragment,struct_table(j).snr, 'measured');
                    t(1,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs)=real(change_fragment);
                    t(2,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs)=imag(change_fragment);
                end %if
            end %if
        end %for
        %resample
        %interpol here
        for j=1:counter
            if(struct_table(j).fs_drift && ~floor(struct_table(j).time_begin/time-i))
                if(~mod(struct_table(j).time_end,time))
                    change_fragment=t(:,mod(struct_table(j).time_begin,time)*fs+1:10*fs);
                    %x=[1:1:length(change_fragment)];
                    %xq=[1:fs/(fs+fs_drift):length(change_fragment)];
                    %interp1(x,v,xq,method),
                    change_fragment = interp1([1:1:length(change_fragment)],change_fragment',[1:fs/(fs+struct_table(j).fs_drift):length(change_fragment)],'spline');
                    t=[t(:,1:mod(struct_table(j).time_begin,time)*fs+1) change_fragment' t(:,struct_table(j).time_end*fs+1:end)];
                else
                    change_fragment=t(:,mod(struct_table(j).time_begin,time)*fs+1:mod(struct_table(j).time_end,time)*fs);
                    %x=[1:1:length(change_fragment)];
                    %xq=[1:fs/(fs+fs_drift):length(change_fragment)];
                    %interp1(x,v,xq,method),
                    change_fragment = interp1([1:1:length(change_fragment)],change_fragment',[1:fs/(fs+struct_table(j).fs_drift):length(change_fragment)],'spline');
                    t=[t(:,1:mod(struct_table(j).time_begin,time)*fs+1) change_fragment' t(:,mod(struct_table(j).time_end,time)*fs+1:end)];
                end %if
            end %if
        end %for
        %define one column vector b
        b = zeros(1, 2*length(t));
        %odd values are real
        b(1:2:2*length(t))=t(1,:);
        %even values are imag
        b(2:2:2*length(t))=t(2,:);
        
        %convert normalized float values to uint in range 0-255
        b = 128 * b + 127;
        b = uint8(b);
        
        fwrite( Filename_w, b, 'uint8' );
    end %for
    fclose(Filename);
    fclose(Filename_w);
end %if(good_data_flag)
end %function