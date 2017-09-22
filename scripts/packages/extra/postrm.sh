#!/bin/bash
#
# Created: 2017
# Author: Konrad Banka <aruko12@gmail.com>

set -e

if [ "$1" == remove ]; then
    rm -f /etc/modprobe.d/blacklist-sdrdab-decoder.conf
fi
