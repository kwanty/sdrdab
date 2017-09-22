#!/bin/bash
set -e
source "scripts/common.sh"

if [ $# -eq 0 ];then
    target="data-light"
else
    target=$1
fi

case $target in
    data-light)
        echo "downloading IQ (light)"
        echo ""
        echo ""
        # raw, clean sample, float format (dat), int8_t (raw), int8_t short sample
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/Record3_katowice_iq.dat -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/Record3_katowice_iq.raw -P data/ 

        # radio Krakow, low noise 30-40 dB
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/antena-1_dab_229072kHz_fs2048kHz_gain42_1.raw -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/antena-1_dab_229072kHz_fs2048kHz_gain42_1_long.raw -P data/

        # difficult data - big fc/fs offset
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/SDRSharp_20150102_125222Z_229077kHz_IQ.wav -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/SDRSharp_20150102_125222Z_229077kHz_IQ.raw -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/SDRSharp_20150604_145504Z_229070kHz_IQ_48dB.wav -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/SDRSharp_20150604_145504Z_229070kHz_IQ_48dB.raw -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/SDRSharp_20150604_221029Z_229070kHz_IQ_28dB.wav -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/SDRSharp_20150604_221029Z_229070kHz_IQ_28dB.raw -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/SDRSharp_20140728_132746Z_227360kHz_IQ.wav -P data/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/SDRSharp_20140728_132746Z_227360kHz_IQ.raw -P data/

        # scripts for PSD and clipping analysis (only RAW supported)
        mkdir -p data/scripts/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/scripts/antenaPSD.py -P data/scripts
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/scripts/acquire_rnd_gain.sh -P data/scripts
    ;;
        data)
        echo ""
        echo ""
        echo ":-("

    ;;
    ut)
        echo ""
        echo ""
        echo "downloading data for UnitTests"

        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/player_unittest_file.mp2 -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/player_unittest_file.aac -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/player_unittest_tags_expected.txt -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/FIGTestData.tar.gz -P data/ut/
        tar --overwrite -xzf data/ut/FIGTestData.tar.gz -C data/ut/
        rm data/ut/FIGTestData.tar.gz

        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/AudioKR_Rytm0_Nielepice_Azart_22dB.aac -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/AudioKR_Rytm3_Nielepice_MiniAntena_34dB.aac -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/AudioKR_Rytm1_Nielepice_MiniAntena_28dB.aac -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/AudioKR_Rytm4_Nielepice_MiniAntena_38dB.aac -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/AudioKR_Rytm2_Nielepice_MiniAntena_32dB.aac -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/AudioKR_Rytm5_Nielepice_MiniAntena_48dB.aac -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/AudioKR_Rytm6_Nielepice_28dB_WithJumps.aac -P data/ut/
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/AudioKR_Rytm6_Nielepice_48dB_WithJumps.aac -P data/ut/
        
        # matlab generated data
        wget -c -nv https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/ut/dataMatlab.tar.gz -P data/ut/
        tar --overwrite -xzf data/ut/dataMatlab.tar.gz -C data/ut/
        rm data/ut/dataMatlab.tar.gz        
        ;;
    *)
        echo ""
        echo ""
        echo "unsupported type of download"
esac