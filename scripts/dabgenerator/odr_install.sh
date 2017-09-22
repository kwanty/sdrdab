#!/bin/bash
# This script installs opendigitalradio tools and dependencies.
# The odr family scripts are needed for the generation of synthetic multiplexes.
#
# In the vast majority it is a copy of a script which installs all needed tools
# under Debian, depleted of a few ifs in order to run under Ubuntu
# or other system derived from Debian.

echo
echo "This script will install ODR-DabMux, ODR-DabMod, ODR-AudioEnc, ODR-PadEnc"
echo "and all prerequisites to your machine."

echo "This program will use sudo to install components on your"
echo "system. Please read the script before you execute it, to"
echo "understand what changes it will do to your system !"
echo
echo "There is no undo functionality here !"
echo

if [ "$UID" == "0" ]
then
    echo "Do not run this script as root !"
    echo "Install sudo, and run this script as a normal user."
    exit 1
fi

which sudo
if [ "$?" == "0" ]
then
    echo "Press Ctrl-C to abort installation"
    echo "or Enter to proceed"

    read
else
    echo "Please install sudo first using"
    echo " apt-get -y install sudo"
    exit 1
fi

# Fail on error
set -e

if [ -d dab ]
then
    echo "ERROR: The dab directory already exists."
    echo "This script assumes a fresh initialisation,"
    echo "if you have already run it and wish to update"
    echo "the existing installation, please do it manually"
    echo "or erase the dab folder first."
    exit 1
fi

echo

echo "Updating debian package repositories"
sudo apt-get -y update

# PREREQUESTITES --------------------------------------
echo "Installing essential prerequestities"
sudo apt-get -y install build-essential git wget \
 sox alsa-tools alsa-utils \
 automake libtool mpg123 \
 libasound2 libasound2-dev \
 libjack-jackd2-dev jackd2 \
 ncdu vim ntp links cpufrequtils \
 libfftw3-dev \
 libcurl4-openssl-dev \
 libmagickwand-dev \
 libvlc-dev vlc-nox \
 libfaad2 libfaad-dev \
 python-mako python-requests

sudo apt-get -y build-dep uhd

# stuff to install from source
mkdir dab || exit
cd dab || exit

echo "Compiling UHD"
git clone http://github.com/EttusResearch/uhd.git
pushd uhd
git checkout release_003_009_001
mkdir build
cd build
cmake ../host
make
sudo make install
popd

echo "Downloading UHD device images"
sudo /usr/local/lib/uhd/utils/uhd_images_downloader.py

sudo apt-get -y install libzmq3-dev libzmq3

echo
echo "PREREQUESTITIES INSTALLED"
# END OF PREREQUESTITIES -------------------------------

echo "Fetching mmbtools-aux"
git clone https://github.com/mpbraendli/mmbtools-aux.git

echo "Fetching etisnoop"
git clone https://github.com/Opendigitalradio/etisnoop.git
pushd etisnoop
./bootstrap.sh
./configure
make
sudo make install
popd

echo "Compiling ODR-DabMux"
git clone https://github.com/Opendigitalradio/ODR-DabMux.git
pushd ODR-DabMux
./bootstrap.sh
./configure
make
sudo make install
popd

echo "Compiling ODR-DabMod"
git clone https://github.com/Opendigitalradio/ODR-DabMod.git
pushd ODR-DabMod
./bootstrap.sh
./configure --enable-debug
make
sudo make install
popd

echo "Compiling fdk-aac library"
git clone https://github.com/Opendigitalradio/fdk-aac.git
pushd fdk-aac
./bootstrap
./configure
make
sudo make install
popd

echo "Updating ld cache"
sudo ldconfig


echo "Compiling ODR-AudioEnc"
git clone https://github.com/Opendigitalradio/ODR-AudioEnc.git
pushd ODR-AudioEnc
./bootstrap
./configure --enable-jack --enable-vlc
make
sudo make install
popd

echo "Compiling ODR-PadEnc"
git clone https://github.com/Opendigitalradio/ODR-PadEnc.git
pushd ODR-PadEnc
./bootstrap
./configure
make
sudo make install
popd

