#!/bin/bash
#
# Created: 2017
# Author: Konrad Banka <aruko12@gmail.com>

set -e

function print_blacklist_file_content() {

    echo "#This file has been generated during installation."
    echo "#Any modifications may be rewritten when upgrading package."
    echo ""
    echo "#rtl-sdr might automatically use your tuner and prevent sdrdab from accessing it."
    echo ""
    echo "blacklist dvb_usb_rtl28xxu"

}

if [ "$1" == install ]; then
    if [ -f /etc/modprobe.d/blacklist-sdrdab-decoder.conf ]; then
        echo "Error, /etc/modprobe.d/blacklist-sdrdab-decoder.conf is present."
        echo "Check if you properly removed sdrdab package."
        exit 1
    fi
    print_blacklist_file_content > /etc/modprobe.d/blacklist-sdrdab-decoder.conf
elif [ "$1" == upgrade ]; then
    print_blacklist_file_content > /etc/modprobe.d/blacklist-sdrdab-decoder.conf
fi
