#!/bin/bash

source "scripts/common.sh"
doxygen ${SCRIPTS_DIR_REL}/_doxygen/sdrdab.doxy
doxygen ${SCRIPTS_DIR_REL}/_doxygen/sdrdab-cli.doxy
doxygen ${SCRIPTS_DIR_REL}/_doxygen/sdrdab-gui.doxy
