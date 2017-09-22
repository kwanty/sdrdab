% Szymon Kurzepa
%
% v1: 11-02-2017
% v2: 14-04-2017 (Jaroslaw Bulat), extract special case
%
% Generate data for 3rd Reed-Solomon Unittest

clc, clear all

rng(1,'twister');     % error occured bufferoverflow in C++ implementaiont
% rng(6,'twister');   % error do not occured

tries = 100;
t = 5;
primitive = 285;

messages = randi([0, 255], tries, 110);

encoded = rsenc(gf(messages, 8, primitive), 120, 110, rsgenpoly(255, 245, primitive, 0));

cases = gf(zeros(tries*(t+2), 120), 8, primitive);

for i=1:tries*(t+2)
    no_errors = mod(i-1, t+2);
    no_try = floor((i-1)/(t+2) + 1);
    cases(i, :) = encoded(no_try, :);
%    positions = randi([1,120], 1, no_errors);          % require new matlab
    positions = randperm(120);
    positions(no_errors+1:end) = [];
    values = gf(randi([1 255], 1, no_errors),8);
    %temp = cases(i,:);
    %temp = double(temp.x);
    cases(i, positions) = cases(i, positions) + values;
    %evaluated = cases(i,:);
    %evaluated = double(evaluated.x);
    %if (nnz(temp ~= evaluated) ~= no_errors)
    %    pause
    %end
end

[corrected, status] = rsdec(cases, 120, 110, rsgenpoly(255, 245, primitive, 0));

cases = double(cases.x);
cases_orig = cases;
corrected = double(corrected.x);

% TODO: output should be truth not corrected polynomial (which is sometime
% wrong)!!!!
corrected = [corrected cases(:, 111:120)];
corrected_orig = corrected;
corrected = reshape(corrected, [1, numel(corrected)]);
dlmwrite('../../data/ut/rs_testData_3_out.txt', corrected, '\n')
dlmwrite('../../data/ut/rs_testData_3_case1_out.txt', corrected(637:7*100:end), '\n')

cases = reshape(cases, [1, numel(cases)]);
dlmwrite('../../data/ut/rs_testData_3_in.txt', cases, '\n')
dlmwrite('../../data/ut/rs_testData_3_case1_in.txt', cases(637:7*100:end), '\n')

% Matlab RS verification
if(0)
    for x=1:tries
        msg = messages(x,:);
        for idx=1:t+2
            msgCor = corrected_orig((x-1)*(t+2)+idx,1:110);
            stat = status((x-1)*(t+2)+idx);
            if idx~=stat+1 && idx<7         % status ~= t
                disp(sprintf('#### wrong rsdec: tries=%d, t=%d, status=%d\n', x, idx, stat ));
            end            
            if idx==7 && stat~=-1           % t==6, status==5 (should be -1)
                disp(sprintf('#### wrong rsdec: tries=%d, t=%d, status=%d\n', x, idx, stat ));
            end            
            if sum(abs(msg - msgCor))>0 && idx<7    % correction faild
                disp(sprintf('tries=%d, t=%d, status=%d\n', x, idx, stat ));
            end            
        end
    end    
end

% case 1: 636:
if(0)
    case_error=636+1;
    messages1 = messages(ceil(case_error/7),:)';
    encoded_double = double(encoded.x);
    encoded1 = encoded_double(ceil(case_error/7),:)';
    cases1 = cases_orig(case_error,:)';
    corrected1 = corrected_orig(case_error,:)';
    status1 = status(case_error);

    encoded1_diff=[messages1; zeros(10,1)] - encoded1;
    cases1_diff = [messages1; zeros(10,1)] - cases1;
    corrected1_diff = [messages1; zeros(10,1)] - corrected1;
    [[messages1; zeros(10,1)], encoded1, encoded1_diff, cases1, cases1_diff, corrected1, corrected1_diff]
    status1
end

