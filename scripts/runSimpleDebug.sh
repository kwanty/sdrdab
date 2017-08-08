#!/bin/bash
# kwant
# changelog 7.06.2015: 
# changelog 29.02.2016:
# * changed the binary path, according to the new buildsystem

source "scripts/common.sh"

TESTFILE="antena-1_dab_229072kHz_fs2048kHz_gain42_1_long.raw"

if [ $# -gt 0 ];then
    if [ "$1" == "antena" ];then
        TESTFILE="antena-1_dab_229072kHz_fs2048kHz_gain42_1_long.raw"
    fi
    if [ "$1" == "katowice" ];then
        TESTFILE="Record3_katowice_iq.raw"
    fi
fi

if [ "$2" = "-v" ];then
    vOpt="-v"
else
    vOpt=""
fi

if [ ! -f "${BUILD_DIR_REL}/bin/sdrdab-cli" ];then
    echo -e "executable \033[31m.${BUILD_DIR_REL}/bin/sdrdab-cli\e[0m not found"
    echo "please build it: ./sdrtool build Debug"
    exit 1
fi

${BUILD_DIR_REL}/bin/sdrdab-cli --open-file=${TESTDATA_DIR_REL}/${TESTFILE} $vOpt
