#!/bin/bash
# 
# ./test/*.cc refactored with:
# sed -i 's/.\/test\/testdata\//.\/data\/ut\//g' *.cc



set -e
source "scripts/common.sh"

if [ -f "${BUILD_DIR_REL}/bin/sdrdab-ut" ]; then
    ${BUILD_DIR_REL}/bin/sdrdab-ut "$@"
else
    echo "Perhaps you should build UT first, shouldn't ya?"
    echo ""
    echo "type: ./sdrtool build Unittest"
fi