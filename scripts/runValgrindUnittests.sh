#!/bin/bash

source "scripts/common.sh"

valgrind --leak-check=yes --suppressions=./scripts/valgrind_supps/gst.supp --suppressions=./scripts/valgrind_supps/nm.supp  ${BUILD_DIR_REL}/bin/sdrdab-ut