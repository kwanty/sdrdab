#!/bin/bash

source "scripts/common.sh"

iqFile="Record3_katowice_iq.raw"

valgrind --leak-check=yes --suppressions=./scripts/valgrind_supps/gst.supp --suppressions=./scripts/valgrind_supps/nm.supp  ${BUILD_DIR_REL}/bin/sdrdab-cli  --open-file=${TESTDATA_DIR_REL}/${iqFile}
