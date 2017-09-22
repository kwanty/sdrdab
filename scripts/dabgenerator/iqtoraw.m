Filename = fopen('example.iq', 'rb');
NSamples = 9e9;
t = fread(Filename, [2, NSamples], 'float');
t = t-(mean(mean(t')));
tmax = max(max(t'));
tmin = min(min(t'));
t = t./(max(tmax,-tmin));

b = zeros(1, 2*length(t));
b(1:2:2*length(t))=t(1,:);
b(2:2:2*length(t))=t(2,:);

b = 128 * b + 127;
b = uint8(b);

Filename_w = fopen('example.raw', 'wb');
fwrite( Filename_w, b, 'uint8' );

fclose(Filename);
fclose(Filename_w);
