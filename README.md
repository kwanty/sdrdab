# Software Defined Radio - DAB+ (libsdrdab)

It is implementation of DAB/DAB+ software decoder. Input samples could be retrived from file or tuner (librtlsdr). Output sound could be save to file or played via GStreamer.

Developed by students of [AGH-UST](http://www.agh.edu.pl/en) in [Department of Telecommunications](http://kt.agh.edu.pl/en).

Project webpage [sdr.kt.agh.edu.pl](https://sdr.kt.agh.edu.pl)

## Build and Run

Tested on Ubuntu 14.x and later. Need: eclipse, gcc and a few standard libraries. libsdrdab has it's own implementation of librtlsdr. Do not need librtlsdr, do not conflict with installed librtlsdr.

* download code 
* unpack
* run: install_dependencies.sh
* start eclipse
* import all projects
* change build targets to: Debug or Release (only for sdrdab and optional for sdrdab-cli)
* build-all 

library is located in sdrdab/Debug/libsdrdab.so
optional cli-reference-implemenation is located in sdrdab-cli/Debug/sdrdab-cli

Installation is not supported so far.

## ToDo

### reDAB(+) encoder

* like [eti-tools](https://github.com/piratfm/eti-tools)
* supports USRP (real-time)
* supports simulation of various disturbances: AWGN, NBI, fading channels, rayleigh fading channels, etc...

### DAB(+) decoder

* feature/algorithms (unsorted):
* GUI (with vizualization: spectrum, SNR, errors, extra data (images), etc...)
* CLI add option for algorithm chagning e.g.: --resampling_fs={sinc_fast, linear, zero-order}, --convolutional={Viterbi, hard, xxx}, etc..
* TEQ - time equalizer
* FEQ - frequency equalizer
* resampling (fs) made by means of remodulate+FEQ+interpolation after FFT
* custom resampling (fs), only necessary parts (like FFT in MSC)
* resampling == own thread? CPU vector (SSE/NEON) optimization?
* implementation of convolutional code simpler than Viterbi decoder (hard-decision, BCJR, soft-decision)
* smart algorithm for convolutional decoder choise (based on SNR?), independent FIC and MSC?
* USRP supports (input signal), other hardware?
* extend input file support: float, double, uint16_t/int16_t, automatic (smart) detection
* decompress to raw samples (get rid of GStreamer and other framework), make resampling in time domain
* decoding extra data (images, etc…)
* ReedSolomon custom implementation (optimized to DAB)
* RS smart on/off (depends on previous errors, CRC and firecode)
* support Rayleigh fading channels
* memory optimization (smaller buffer, adaptive buffers?)
* FIC SNR improvement: CIF’s repeats (energy could be accumulated before Viterbi and thus improve SNR)
* Decode many (all) audio streams simultanously
* FIle2FIle should not skip frames from bigining of stream
* Extensive unittest: all possible bitstreams, types, modes, additional data and various configuration of multiplex
* Test and tune to mobile case (Rayleigh channel)


### build (unsorted)

* move from eclise to make/cmake + builders helper for Eclipse
* performance (time) test integrated with build (per method on release build)
* performance (erorr) test integrated with build - parameters: SNR, fc, fs
* ARM (RaspberyPI v2)
* ARM Android NDK/SDK
* x86 Windows
* x86 MacOS
* refractoring DataDecoder (too big)
* refractoring DataFeeder (too complex)
* refractoring AudioDecoder (too complex?)
* package for Linux (Ubuntu and other distros)
* all code (inclueding library) on LGPL license
* integration with vlc/mplayer/kino/kodi plugin/other media player

## copyright

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
