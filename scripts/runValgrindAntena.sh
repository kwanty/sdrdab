#!/bin/bash

source "scripts/common.sh"

iqFile="antena-1_dab_229072kHz_fs2048kHz_gain42_1_long.raw"

valgrind --leak-check=yes --suppressions=./scripts/valgrind_supps/gst.supp --suppressions=./scripts/valgrind_supps/nm.supp  ${BUILD_DIR_REL}/bin/sdrdab-cli  --open-file=${TESTDATA_DIR_REL}/${iqFile}