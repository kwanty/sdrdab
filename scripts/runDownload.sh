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
        # raw, clean sample, float format (dat), int8_t (raw), int8_t short sample (_short.raw)
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/Record3_katowice_iq.dat -P data/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/Record3_katowice_iq.raw -P data/ 
        #wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/Record3_katowice_iq_short.raw 

        # radio Kraków, low noise 30-40 dB (?)
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/antena-1_dab_229072kHz_fs2048kHz_gain42_1.raw -P data/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/antena-1_dab_229072kHz_fs2048kHz_gain42_1_long.raw -P data/
    ;;
        data)
        echo ""
        echo ""
        echo "download ALL not yet supported"
    ;;
    ut)
        echo ""
        echo ""
        echo "downloading data for UnitTests"

        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/player_unittest_file.mp2 -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/player_unittest_file.aac -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/player_unittest_tags_expected.txt -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/FIGTestData.tar.gz -P data/ut/
        tar --overwrite -xzf data/ut/FIGTestData.tar.gz -C data/ut/
        rm data/ut/FIGTestData.tar.gz

        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/AudioKR_Rytm0_Nielepice_Azart_22dB.aac -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/AudioKR_Rytm3_Nielepice_MiniAntena_34dB.aac -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/AudioKR_Rytm1_Nielepice_MiniAntena_28dB.aac -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/AudioKR_Rytm4_Nielepice_MiniAntena_38dB.aac -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/AudioKR_Rytm2_Nielepice_MiniAntena_32dB.aac -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/AudioKR_Rytm5_Nielepice_MiniAntena_48dB.aac -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/AudioKR_Rytm6_Nielepice_28dB_WithJumps.aac -P data/ut/
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/AudioKR_Rytm6_Nielepice_48dB_WithJumps.aac -P data/ut/
        
        # matlab generated data
        wget -c https://sdr.kt.agh.edu.pl/sdrdab-decoder/downloads/data/dataMatlab.tar.gz -P data/ut/
        tar --overwrite -xzf data/ut/dataMatlab.tar.gz -C data/ut/
        rm data/ut/dataMatlab.tar.gz        
        ;;
    *)
        echo ""
        echo ""
        echo "unsupported type of download"
esac
