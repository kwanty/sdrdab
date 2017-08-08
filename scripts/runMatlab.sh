#!/bin/bash

source "scripts/common.sh"

# generate unittest I/O data
cd matlab/
matlab -nodesktop -r "all_generate; exit"
