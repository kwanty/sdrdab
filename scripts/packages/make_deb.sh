#!/bin/bash
#
# Created: 2017
# Author: Konrad Banka <aruko12@gmail.com>
#
# Package creating script, for more information, please refer to README

set -ue
set -o pipefail

function log_echo() {
    echo >&2 "$*"
}

function clean_deb_folders {
    log_echo "INFO: Cleaning abandoned working directory"
    rm -rf "$PACKAGE_DIR"
    log_echo "INFO: Done"
}

function create_data_package {
    log_echo "INFO: Creating data package"
    {
        mkdir -p "$(dirname $MOVE_BINARIES)"
        install -s "$MATCH_BINARIES" "$MOVE_BINARIES"

#Removed in favor of postrm/preinst scripts
#        mkdir -p $(dirname $MOVE_MODPROBE)
#        cp $MATCH_MODPROBE $MOVE_MODPROBE
#        chmod 644 $MOVE_MODPROBE

        mkdir -p "$(dirname $MOVE_DOCS)"
        cat "$MATCH_DOCS" | gzip -n -c9 > "$MOVE_DOCS"
        chmod 644 "$MOVE_DOCS"

        mkdir -p "$(dirname $MOVE_COPYRIGHT)"
        cp "$MATCH_COPYRIGHT" "$MOVE_COPYRIGHT"
        chmod 644 "$MOVE_COPYRIGHT"

# Removed but stays for future usage
#        mkdir -p $(dirname $MOVE_LIBRARIES)
#        install $MATCH_LIBRARIES $MOVE_LIBRARIES
    }
    log_echo "INFO: Done"
}

function create_control_package {
    log_echo "INFO: Creating control package"
    {
        mkdir -p "$CONTROL_DIR"
        create_md5sum_file
        create_control_file
        create_preinst_script
        create_postrm_script
    }
    log_echo "INFO: Done"
}

function create_preinst_script {
    cp "$PREINST_SCRIPT" "$CONTROL_DIR/preinst"
    chmod 755 "$CONTROL_DIR/preinst"
}

function create_postrm_script {
    cp "$POSTRM_SCRIPT" "$CONTROL_DIR/postrm"
    chmod 755 "$CONTROL_DIR/postrm"
}
function create_control_file {
    log_echo "INFO: Creating control file"
    {
        echo "Package: $CTL_PACKAGE"
        echo "Version: $CTL_VERSION"
        echo "Priority: $CTL_PRIORITY"
        echo "Section: $CTL_SECTION"
        echo "Maintainer: $CTL_MAINTAINER"
        echo "Architecture: $CTL_ARCHITECTURE"
        echo "Homepage: $CTL_HOMEPAGE"
        echo "Depends: ${CTL_DEPENDS}"
        echo "Description: ${CTL_DESCRIPTION}"
        cat "${CTL_LONG_DESCRIPTION_FILE}"
    } > "$CONTROL_DIR/control"
    log_echo "INFO: Done"
}

function create_md5sum_file {
    log_echo "INFO: Creating checksum file"
    {
        pushd "$PACKAGE_DIR"
        for directory in $(ls)
        do
            [ "$directory" == DEBIAN ] && continue
            find "$directory" -type f -exec md5sum {} \; >> "$CONTROL_DIR/md5sums"
        done
        chmod 644 "$CONTROL_DIR/md5sums"
        popd
    }
    log_echo "INFO: Done"
}

function read_dependencies_ldd {
    #log_echo "INFO: Updating packages database"
    #(set +e; yes | apt-file update; >&2 echo "INFO: Done")
    log_echo "INFO: Generating dependencies list, it may last a while"
    for binary in $(ls $MATCH_BINARIES)
    do
        log_echo "INFO: Dependencies for $binary"
        ldd "$binary" | awk '{ print $1 }' | xargs -P "$(grep -c processor /proc/cpuinfo)" -I {} bash -c "apt-file search {} | head -n 1 | cut -d : -f 1" | sort | uniq
        log_echo "INFO: Done"
    done
}


function parse_input {
    log_echo "INFO: Parsing input"
    USE_LDD=false
    CLEAN=false
    PREPARE=false
    CREATE=false
    TEST=false
    while [ "$#" -ge 1 ] 
    do
        case $1 in
            --dynamic_dependencies)
                USE_LDD=true
                ;;
            --config)
                CONFIG="$2"
                shift
                ;;
            --root)
                PROJECT_ROOT="$2"
                shift
                ;;
            --clean)
                CLEAN=true
                ;;
            --prepare)
                PREPARE=true
                ;;
            --create)
                CREATE=true
                ;;
            --test)
                TEST=true
                ;;
            --default)
                CLEAN=true
                PREPARE=true
                CREATE=true
                ;;
            --help)
                cat "$(dirname $0)/README"
                exit 0
                ;;
            *)
                log_echo "ERROR: Unexpected argument: $1"
                log_echo "INFO: Please refer to README"
                return 1
                ;;
        esac
        shift
    done
    log_echo "INFO: Done"
}

parse_input $@

set +u
if [ -z "$PROJECT_ROOT" ]; then
    log_echo "INFO: Using current directory as project root."
    PROJECT_ROOT="$(pwd -P)"
fi

if [ -z "$CONFIG" ]; then
    log_echo "WARNING: No config file supplied!"
    log_echo "INFO: Try: $0 --config CONFIG_FILE ${@:1}"
    log_echo "INFO: Not aborting"
else
    set -u
    source "$CONFIG"
fi
set -u

if [[ "$USE_LDD" == true ]]; then
    read_dependencies_ldd
    exit 0
fi

if [[ "$CLEAN" == true ]]; then
    clean_deb_folders
fi

if [[ "$PREPARE" == true ]]; then
    mkdir -p "$PACKAGE_DIR"
    log_echo "INFO: Working in $PACKAGE_DIR"
    create_data_package
    create_control_package
    find "$PACKAGE_DIR" -type d -execdir chmod 755 {} \;
fi

if [[ "$CREATE" == true ]]; then
    fakeroot dpkg -b "$PACKAGE_DIR" "$RESULT_DIR"
fi

if [[ "$TEST" == true ]]; then
    lintian $RESULT_DIR/*.deb
fi
