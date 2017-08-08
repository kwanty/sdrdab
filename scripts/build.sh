#!/bin/bash
# crate build directory and makefiles from cmake
# create plain makefiles and build system for eclipse
# v1: ???
# v2: 2016.04.02 build for eclipse (kwant)
# v3: 2016.05.04 integration eclipse and CLI build
#
# available targets:
#   ./build.sh eclipse_[Debug|Release\Unittest] -> create makefile for Eclipse and Debug|Release\Unittest profile
#   ./build.sh [Debug|Release|Unittest] -> cmake and make for CLI build
#   ./build.sh clean -> clean ./buld directory

source "scripts/common.sh"

set -e

target="$1"

# capitalize first letter
# ${1:1} takes the rest
# ${1:0:1} takes first letter and passes it through tr to capitalize it
if [ "$target" == "debug" -o \
     "$target" == "release" -o \
     "$target" == "scanbuild" -o \
     "$target" == "profiling" -o \
     "$target" == "unittest" ]
then
    target=$(echo ${1:0:1} | tr [a-z] [A-Z])${1:1}
fi

if [ ! -d "${BUILD_DIR_REL}" ]; then
    mkdir "${BUILD_DIR_REL}"
fi

if [ -z "$target" ]; then
    target=Debug
fi

case $target in
    clean)
        rm -rf ${BUILD_DIR_REL}/
        ;;
    Scanbuild|Debug|Profiling|Release|Unittest) # Debug profile from CLI
        wrapper=""
        if [ ${target} = "Scanbuild" ]; then
            wrapper="scan-build"
            makeOpts="-j4"
            cmakeOpts="-DCMAKE_BUILD_TYPE=Debug"
        fi
        if [ ${target} = "Debug" ];then
            makeOpts="-j4 VERBOSE=ON"
            cmakeOpts="-DCMAKE_BUILD_TYPE=Debug"
        fi
        if [ ${target} = "Release" ];then
            makeOpts="-j4"
            cmakeOpts="-DCMAKE_BUILD_TYPE=Release"
        fi
        if [ ${target} = "Unittest" ];then
            makeOpts="-j4"
            cmakeOpts="-DCMAKE_BUILD_TYPE=Debug -DMAKE_TESTS=yes"
        fi
        if [ ${target} = "Profiling" ]; then
            makeOpts="-j4"
            cmakeOpts="-DCMAKE_BUILD_TYPE=Debug -DPROFILING=yes"
        fi

        if [ ! -f "${BUILD_DIR_REL}/.target-${target}" ]; then
            rm -rf ${BUILD_DIR_REL}/
            mkdir ${BUILD_DIR_REL}
            $wrapper cmake -E chdir ${BUILD_DIR_REL}/ cmake ${cmakeOpts} ../
            touch ${BUILD_DIR_REL}/.target-${target}
        fi

        $wrapper make -C ${BUILD_DIR_REL}/ ${makeOpts}

        echo ""
        echo ""
        echo "Binary files can be found at: ${BUILD_DIR_REL}/bin/"
        ;;
    eclipse_*) # active configuration: Debug | Release | Unittest
        subTarget=${target:8}
        if [ ${subTarget} = "Unittest" ];then
            cmakeOpts="-DCMAKE_BUILD_TYPE=Debug -DMAKE_TESTS=yes"
        fi

        if [ ! -e "${BUILD_DIR_REL}/.eclipse_target_$subTarget" ] ; then
            rm -rf ${BUILD_DIR_REL}/
            mkdir ${BUILD_DIR_REL}
            cmake -E chdir ${BUILD_DIR_REL}/ cmake ${cmakeOpts} -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE:STRING=$subTarget
            touch "${BUILD_DIR_REL}/.eclipse_target_$subTarget"
        fi
        ;;
    *)
        echo ""
        echo ""
        echo "unrecognized build option !!!"
        echo "use:"
        echo "  sdrtool build [Debug|Release|Unittest]"
esac
