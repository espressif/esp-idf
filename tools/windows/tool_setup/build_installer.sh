#!/bin/sh
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

cd `dirname $0`
pushd dl
wget --continue "https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip"
wget --continue "https://github.com/espressif/kconfig-frontends/releases/download/v4.6.0.0-idf-20180525/mconf-v4.6.0.0-idf-20180525-win32.zip"
wget --continue "https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip"
popd

rm -rf input/*
pushd input
unzip ../dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip
unzip ../dl/mconf-v4.6.0.0-idf-20180525-win32.zip
unzip ../dl/ninja-win.zip
popd

wine "C:\Program Files\Inno Setup 5\ISCC.exe" "`winepath -w ./idf_tool_setup.iss`"

