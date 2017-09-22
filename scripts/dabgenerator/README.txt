X-Pad encoding in DAB+ and DAB - odr-audioenc + odr-padenc

Most important is to have the files, from which we create the multiplex,
sampled with 48kHz frequency and to have them in the *.wav format.

Personally, we converted *.mp3 to *.wav with
http://audio.online-convert.com/convert-to-wav
without changing the bit resolution or audio channels.

TAKE CARE! All *.wav files found in the network by us
were samples with 44.1kHz frequency!

On this stage we need:
- proper software installed (script: odr_install.sh)
- *.wav files sampled with 48kHz frequency
- example_slides catalogue containing graphical images in jpeg or png format
- example_metadata.txt file in format:

##### parameters { #####
DL_PLUS=1
DL_PLUS_ITEM_TOGGLE=0 
DL_PLUS_ITEM_RUNNING=1
DL_PLUS_TAG=4 5 10
DL_PLUS_TAG=1 19 3
##### parameters } #####
Now: Alan Walker - Fade

Pattern:
DL_PLUS_FLAG=X Y Z
X - Type, 4 - performer, 1 - title
Y - number of character from which we start
Z - length of the element (title, performer) taking white spaces into consideration minus one

more information: http://wiki.opendigitalradio.org/ODR-PadEnc

X-Pad and audio encoding in DAB+

We create a fifo
	mkfifo /tmp/pad.fifo

In one terminal we run odr-padenc which writes to the fifo
	odr-padenc -o /tmp/pad.fifo -t disfigure_metadata.txt -d ./disfigure_slides -p 58 -s 0

In another one we run odr-audioenc which in turn reads from the fifo
	odr-audioenc -i disfigure.wav -b 88 -o disfigure.dabp -P /tmp/pad.fifo -p 58
	rm /tmp/pad.fifo

TAKE CARE! The terminal in which we run odr-padenc blocks until odr-audioenc reads from the fifo.
The order is essential: first we run odr-padenc in one terminal than odr-audioenc in the other.
After odr-audioenc reads from the fifo, we send a SIGINT to odr-padenc (Ctrl+C).
If we create a few files sequentially it is viatal to remove the fifo
which can still contain old data.

The process for DAB is very similar
X-Pad and audio encoding in DAB

	mkfifo /tmp/pad.fifo
	odr-padenc -o /tmp/pad.fifo -t halvorsen_metadata.txt -d ./halvorsen_slides -p 58 -s 0

In order to generate audio file with *.mp2 extension for DAB we add the '-a' option
	odr-audioenc -i Halvorsen.wav -b 128 -o Halvorsen.mp2 -a -P /tmp/pad.fifo -p 58
	rm /tmp/pad.fifo

Next, we use odr-dabmux program where, with a special file conf.mux, we generate an *.eti file,
exemplary file with configuration is in this catalogue.

In conf.mux file we can select the transmission mode. Beware of problems with mode 3, protection
types. Please, use the files provided.

Exemplary content of conf.mux file with description:

general {
    dabmode 1 // 1-4 avaliable
    nbframes 5000
}
remotecontrol { telnetport 0 }
ensemble {
    id 0x4fff
    ecc 0xec ; Extended Country Code
    local-time-offset auto
    international-table 1
    label "mmbtools"
    shortlabel "mmbtools"
}
services {
    srv-p1 { label "Prog1" }
    srv-p2 { label "Prog2" }
}
subchannels {
    sub-p1 {
        ; MPEG
        type audio
        inputfile "prog1.mp2"
        bitrate 128
        id 10 // channel number
        protection 5 // 1-5

    }
    sub-p2 {
        type dabplus
        inputfile "prog2.dabp"
        bitrate 88
        id 1 // channel number
        protection-profile EEP_A // there might also be EEP_B
        protection 1 // 1-4 (A), 1-4(B)
    }
}
components {
    comp-p1 {
        label Prog1
        service srv-p1
        subchannel sub-p1
    }
    comp-p2 {
    label Prog2
    service srv-p2
    subchannel sub-p2
    }
}
outputs { output1 "file://myfirst.eti?type=raw" }

NOW: we run
	odr-dabmux conf.mux

In result we get an *.eti file.
On its basis we create *.iq file with:
	odr-dabmod myfirst.eti -f myfirst.iq

With the help of '-m' parameter we can force the transmission mode (dabmodes 1-4)

The last step is to convert the samples with matlab script.

This is the script we can use to create *.raw file which we can play with sdrdab-cli.
The script is contained in catalogue called iqtoraw.m

Filename = fopen(‘example.iq', 'rb');
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

We play the file with
	./sdrdab-cli --open-file=data/exampleIQ.raw

More options: ./sdrdab-cli --help
