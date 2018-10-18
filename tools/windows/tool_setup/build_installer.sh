#!/bin/bash
#
# Setup script to build Windows tool installer with Inno Setup
#
# Designed to be run on Linux (with wine) but could be adapted to run under MSYS2 on Windows
# pretty easily...
#
# - Downloads (if necessary) all tools to install to the "dl/" directory
# - Deletes the "input" directory contains and copies everything under there
# - Runs ISCC under wine to compile the installer itself
set -e

if [ -z "${KEYPASSWORD}" ]; then
    echo "KEYPASSWORD should be set"
    exit 1
fi

if [ "$1" != "--no-download" ]; then

    mkdir -p dl input

    cd `dirname $0`
    pushd dl
    wget --continue "https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip"
    wget --continue "https://github.com/espressif/binutils-esp32ulp/releases/download/v2.28.51-esp32ulp-20180809/binutils-esp32ulp-win32-2.28.51-esp32ulp-20180809.zip"
    wget --continue "https://github.com/espressif/openocd-esp32/releases/download/v0.10.0-esp32-20180920/openocd-esp32-win32-0.10.0-esp32-20180920.zip"
    wget --continue "https://github.com/espressif/kconfig-frontends/releases/download/v4.6.0.0-idf-20180525/mconf-v4.6.0.0-idf-20180525-win32.zip"
    wget --continue "https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip"
    popd

    rm -rf input/*
    pushd input
    unzip ../dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip
    unzip ../dl/mconf-v4.6.0.0-idf-20180525-win32.zip
    unzip ../dl/binutils-esp32ulp-win32-2.28.51-esp32ulp-20180809.zip
    unzip ../dl/openocd-esp32-win32-0.10.0-esp32-20180920.zip
    unzip ../dl/ninja-win.zip
    popd
fi

wine "C:\Program Files\Inno Setup 5\ISCC.exe" "`winepath -w ./idf_tool_setup.iss`"

# sign the installer with osslsigncode, parsing the version number out of the
# installer config

VERSION=`grep "^AppVersion=" idf_tool_setup.iss | cut -d'=' -f2`

echo "Signing installer..."

# Note: The cert chain passed to -certs needs to contain the intermediate
# cert(s) as well, appended after the code signing cert, or Windows may see
# it as "Unknown Publisher"
#
# See https://stackoverflow.com/a/52637050 for full details
#
umask 770  # for the process substitution FIFO

osslsigncode -certs ./keys/certchain.pem -key ./keys/key.pem \
             -readpass <(echo "$KEYPASSWORD") \
             -in Output/esp-idf-tools-setup-unsigned.exe \
             -out Output/esp-idf-tools-setup-${VERSION}.exe \
             -h sha256 \
             -n "Espressif Systems (Shanghai) Pte. Ltd." \
             -i "https://www.espressif.com/" \
             -ts http://timestamp.digicert.com

chmod 644 Output/esp-idf-tools-setup-${VERSION}.exe  # make up for the umask
