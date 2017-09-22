# Software Defined Radio - DAB+ (libsdrdab), version 3.0 beta

It is implementation of DAB/DAB+ software decoder. Input samples could be retrived from file or tuner (librtlsdr). Output sound could be save to file or played via GStreamer.

Developed by students of [AGH-UST](http://www.agh.edu.pl/en) in [Department of Telecommunications](http://kt.agh.edu.pl/en). 

Project webpage [sdr.kt.agh.edu.pl](https://sdr.kt.agh.edu.pl)


## Build and Run

Tested on Ubuntu 14.x, 16.x and other. Need: gcc and a few standard libraries. libsdrdab has it's own implementation of librtlsdr. Do not need librtlsdr, do not conflict with installed librtlsdr.

* download code 
* unpack
* install dependencies: ./sdrtool install-deps
* build: ./sdrtool build Release
* data: ./sdrtool download-data-light					# optional for IQ raw data
* run: 
*  ./build/bin/sdrdab-cli 						# from SDR USB tuner
*  ./build/bin/sdrdab-cli --open-file=./data/*.raw			# default is --file-type=raw
*  ./build/bin/sdrdab-cli --open-file=./data/*.raw --file-type=uint8	# or --file-type=raw
*  ./build/bin/sdrdab-cli --open-file=./data/*.dat --file-type=float


library is located in ./build/src/sdrdab/libsdrdab.a
cli-reference-implemenation is located in ./build/bin/sdrdab-cli

Installation is not supported so far. Experimental package building is supportet via ./sdrtool

## UnitTests

UnitTests may be useful for developing purpose. Steps necessary to perform it:

* ./sdrtool download-ut     # download raw data
* ./sdrtool matlab          # generate input and output for unittests (require Matlab(C) version 2006+)
* ./sdrtool build Unittest  # build library with unittests
* ./sdrtool run-unittests   # perform all unittests

### Matlab

Unittests includes DAB/DAB+ decoder (in Matlab) written by Michael Hoin [ZHAW Zürcher Hochschule für Angewandte Wissenschaften](https://www.zhaw.ch/) and extended by Tomasz Zielinski [AGH-UST Telecommunications Department](http://kt.agh.edu.pl/en). This code is used to generate input and output (truth) data for unittests. You need [Matlab](https://in.mathworks.com/products/matlab.html) to perform unittests.

### Data

It is possible to download some data (./sdrtool download-*) necessary for unittest and helpful for testing library. It consists of recordings come from publicly available and uncoded signal. Copyrights to pieces registered at that time belong to their authors. Recordings were not modified (apart from format conversion). Recordings can only be used in order to test the library; in particular further publishing or public reproducing is not allowed. 

## ToDo

### DAB(+) decoder - feature/algorithms (unsorted):

* debug AGC
* TEQ - time equalizer
* resampling (fs) made by means of remodulate+FEQ+interpolation after FFT
* custom resampling (fs), only necessary parts (like FFT in MSC), remove RingBuffer
* resampling == own thread? CPU vector (SSE/NEON) optimization?
* implementation of convolutional code simpler than Viterbi decoder (hard-decision, BCJR)
* smart algorithm for convolutional decoder choise (based on SNR?), independent FIC and MSC?
* USRP supports (input signal), other hardware?
* extend input file support: automatic (smart) detection
* decompress to raw samples (get rid of GStreamer and other framework), make resampling in time domain
* decoding extra data (images, etc…)
* RS smart on/off (depends on previous errors, CRC and firecode)
* support Rayleigh fading channels
* memory optimization (smaller buffer, adaptive buffers?)
* improve system delay (scheduler optimization), system should maintain 200-500 ms delay
* FIC SNR improvement: CIF’s repeats (energy could be accumulated before Viterbi and thus improve SNR)
* decode many (all) audio streams simultaneously
* FIle2FIle should not skip frames from beginning of stream
* test and tune to mobile case (Rayleigh channel)
* rewrite GUI (with vizualization: spectrum, SNR, errors, extra data (images), etc...)
* merge arm-development into development (extra CMake, stats generator, performance improvements, Neon instructions support, psd and snr estimation)
* performance (time) test integrated with build (per method on release build)
* performance (erorr) test integrated with build - parameters: SNR, fc, fs
* ARM (RaspberyPI v2)
* ARM Android NDK/SDK
* x86 Windows
* x86 MacOS: more test, build is already done
* refactoring DataFeeder (too complex)
* refactoring AudioDecoder (too complex?)
* all code (including library) on LGPL license
* integration with vlc/mplayer/kino/kodi plugin/other media player
* recover broken UnitTest
* improve Synchronizer (chose algorithm, remove or repair dependencies for remodulation)
* test synchronizer for weak signal (+-10 levels for 8-bit input sample)
* show information from NULL decoding (transmitter ID)
* software MIMO (check if it's possible to use two or more RTL dongle, synchronize and use MIMO algorithms)
* calculate SNR from PR (3rd algorithm for SNR calculation)
* analyze algorithm for SNR calculation from Demodulator
* improve fs and fc drifts
* implement time signatures/counter for signal in all classes (allows for precise synchronization)
* auto-scanning, search for all allowed frequency for DAB/DAB+ signal
* add CRC verification for XPAD (text)
* improve XPAD decoding (images)


### build (unsorted)

* performance (time) test integrated with build (per method on release build)
* performance (error) test integrated with build - parameters: SNR, fc, fs
* DataFeeder refractoring (too complex)
* AudioDecoder refractoring (too complex)
* package for Linux (Ubuntu and other distros)
* all code (including library) on LGPL license
* ARM (RaspberyPI v2)
* ARM Android NDK/SDK
* x86 Windows
* x86 MacOS: more test, build is already done
* integration with vlc/mplayer/kino/kodi plugin/other media players
* merge arm-development (2015) into development (extra CMake, stats generator, performance improvements, Neon instructions support, psd and snr estimation)


### DAB(+) encoder

* like [eti-tools](https://github.com/piratfm/eti-tools)
* supports USRP (real-time)
* supports simulation of various disturbances: AWGN, NBI, fading channels, rayleigh fading channels, etc...

## copyright

2017:
* Tomasz P. Zieliński (tzielin@agh.edu.pl)
* Jarosław Bułat (kwant@agh.edu.pl)          <---- contact person
* Konrad Bańka
* Szymon Bar
* Marcin Chroń
* Szymon Flakus
* Dominik Koza
* Piotr Kmiecik
* Krystian Majer
* Bartłomiej Ogorzałek
* Michał Piech
* Grzegorz Skołyszewski
* Mirosław Szewczyk
* Miłosz Śliwiński
* Marek Winiarski
* Mateusz Ziarko

2016:
* Tomasz P. Zieliński (tzielin@agh.edu.pl)
* Jarosław Bułat (kwant@agh.edu.pl)          <---- contact person
* Jakub Bernady
* Szymon Czubak
* Szymod Dawidow
* Alicja Gęgotek
* Joanna Gniadek
* Michał Jurczak
* Paweł Kazimierowicz
* Przemysław Piątek
* Anna Radoszek
* Marek Rzepecki
* Mikołaj Suliga
* Bartosz Stopa
* Marcin Trebunia
* Piotr Wierzbicki
* Adrian Włosiak
* Łukasz Włosowicz
* Mateusz Wnętrzak
* Kacper Żuk

2015:
* Tomasz P. Zieliński (tzielin@agh.edu.pl)
* Jarosław Bułat (kwant@agh.edu.pl)          <---- contact person
* Michał Babiuch (babiuch.michal@gmail.com)
* Ernest Biela
* Szymon Dąbrowski
* Piotr Jaglarz
* Adrian Karbowiak
* Sebastian Leśniak
* Rafał Palej
* Kacper Patro
* Michał Rybczyński
* Dawid Rymarczyk
* Michał Rzepka (mrzepka@student.agh.edu.pl)
* Krzysztof Szczęsny (kaszczesny@gmail.com)
* Wojciech Szmyd
* Paweł Szulc (pawel_szulc@onet.pl)
* Jan Twardowski
* Łukasz Wysogląd
* Kacper Zych (kacperzych@wp.pl)


## Disclaimer

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
