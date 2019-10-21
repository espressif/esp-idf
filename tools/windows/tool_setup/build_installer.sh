#!/bin/bash
#
# Script to build the IDF Tools installer for Windows with Inno Setup.
# This script should be executed inside wine-innosetup docker image.
#
# - Downloads all tools to install into the "dist/" directory
# - Downloads 7z and idf_versions.txt
# - Runs ISCC under wine to compile the installer itself

set -e
set -u

iscc_path=$(which iscc)
if [[ -z "$iscc_path" ]]; then
    echo "Inno setup compiler (iscc) not found. Are you running wine-innosetup Docker image?"
    exit 1
fi

if [[ -z "${IDF_PATH:-}" ]]; then
    export IDF_PATH=$(cd ../../../; pwd)
    echo "Assuming IDF_PATH: ${IDF_PATH}"
fi

echo "Downloading IDF Tools..."
mkdir -p idf_tools_tmp
export IDF_TOOLS_PATH=$PWD/idf_tools_tmp
$IDF_PATH/tools/idf_tools.py --non-interactive download --platform Windows-x86_64 all
$IDF_PATH/tools/idf_tools.py --tools-json tools_fallback.json --non-interactive download --platform Windows-x86_64 all
mkdir -p dist
cp idf_tools_tmp/dist/* dist/

echo "Downloading 7z..."
mkdir -p unzip
pushd unzip
wget --no-verbose -O 7z1900-extra.7z https://www.7-zip.org/a/7z1900-extra.7z
7zr e -y 7z1900-extra.7z
popd

echo "Downloading idf_versions.txt..."
wget --no-verbose -O idf_versions.txt https://dl.espressif.com/dl/esp-idf/idf_versions.txt

echo "Running ISCC..."
iscc idf_tool_setup.iss
