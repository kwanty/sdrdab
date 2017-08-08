#!/bin/bash

set -x

sudo sh -c "apt-get update; apt-get install -y \
    cmake \
    doxygen \
    rtl-sdr gqrx-sdr libgtest-dev libfftw3-dev libfftw3-bin libsndfile1 libsndfile1-dev libsamplerate0-dev libusb-1.0-0-dev \
    libgstreamer1.0-0 libgstreamer1.0-dev libgstreamer-plugins-bad1.0-0 libgstreamer-plugins-base1.0-0 libgstreamer-plugins-base1.0-dev gstreamer1.0-alsa gstreamer1.0-plugins-bad gstreamer1.0-plugins-base gstreamer1.0-plugins-base-apps gstreamer1.0-plugins-good gstreamer1.0-plugins-ugly gstreamer1.0-pulseaudio gstreamer1.0-tools \
    graphviz libreadline6 libreadline6-dev \
    qtbase5-dev \
"
