%@author Miroslaw Szewczyk mirsze@student.agh.edu.pl
%100%

function [ X ] = decodeNull( x )
null = [x(1280:2048); x(1: 769)];
maximum = max(x);
null = null./maximum;
for c = 1 : 24
    for p = 1 : 70
        suma = 0;
        for n = 1 : 1538
            if null(n) > 0.5
                null(n) = 1;
            else
                null(n) = 0;
            end
            if (A(n,p,c) * null(n)) == 1
                suma = suma + 1;
                if suma == 16
                    figure
                    plot(x);hold on
                    plot(A(:,p,c), 'r--');
                    X = [p c]
                    suma = 0;
                    pause
                    close
                end
            end
        end
    end
end
end
