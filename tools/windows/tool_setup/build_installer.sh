#!/usr/bin/env bash
#
# Script to build the IDF Tools installer for Windows with Inno Setup.
# This script should be executed inside wine-innosetup docker image.
#
# - Downloads all tools to install into the "dist/" directory
# - Downloads 7z and idf_versions.txt
# - Runs ISCC under wine to compile the installer itself

set -x
set -e
set -u

INSTALLER_TYPE="${1-online}"
COMPRESSION="${2-lzma}"

# Default values for IDF installer passed in 'offline' version of build
IDF_GIT_VERSION="2.30.0.2"
IDF_GIT_VERSION_DIR="v2.30.0.windows.2"
IDF_PYTHON_VERSION="3.8.7"
IDF_PYTHON_WHEELS_VERSION="3.8-2021-01-21"

echo "Selected installer type: $INSTALLER_TYPE"
echo "Selected compresion: $COMPRESSION"
echo "Available installer types: online, offline, precached, draft"
echo "Available compressions: lzma, none"

# Configuration options passed to ISCC compiler
# OFFLINE [yes|no] - set installer to offline mode, nothing will be retrieved from
#  internet during installation
ISCC_PARAMS=""

function prepare_offline_branches()
{
    BUNDLE_DIR="releases/esp-idf-bundle"

    if [[ ! -d "$BUNDLE_DIR" ]]; then
        echo "Performing full clone."
        git clone --shallow-since=2020-01-01 --jobs 8 --recursive https://github.com/espressif/esp-idf.git "$BUNDLE_DIR"

        # Fix repo mode
        git -C "$BUNDLE_DIR" config --local core.fileMode false
        git -C "$BUNDLE_DIR" submodule foreach --recursive git config --local core.fileMode false
        # Allow deleting directories by git clean --force
        # Required when switching between versions which does not have a module present in current branch
        git -C "$BUNDLE_DIR" config --local clean.requireForce false
        git -C "$BUNDLE_DIR" reset --hard
        git -C "$BUNDLE_DIR" submodule foreach git reset --hard
    else
        git -C "$BUNDLE_DIR" fetch
    fi

    VERSIONS="idf_versions.txt"

    tac "$VERSIONS" | while read BRANCH; do
        pushd "$BUNDLE_DIR"

        if [[ -z "$BRANCH" ]]; then
            continue
        fi

        echo "processing branch: ($BRANCH)"
        git fetch origin tag "$BRANCH"
        git checkout "$BRANCH"

        # Pull changes only for branches, tags does not support pull
        #https://stackoverflow.com/questions/1593188/how-to-programmatically-determine-whether-the-git-checkout-is-a-tag-and-if-so-w
        git describe --exact-match HEAD || git pull

        git submodule update --init --recursive

        # Clean up left over submodule directories after switching to other branch
        git clean --force -d
        # Some modules are very persistent like cmok and needs 2nd round of cleaning
        git clean --force -d

        git reset --hard
        git submodule foreach git reset --hard

        if [[ $(git status -s | wc -l ) -ne 0 ]]; then
            echo "git status not empty. Repository is dirty. Aborting."
            git status
            exit 1
        fi

        $IDF_PATH/tools/idf_tools.py --tools-json tools/tools.json --non-interactive download --platform Windows-x86_64 all
        popd
    done

    # Remove symlinks which are not supported on Windws, unfortunatelly -c core.symlinks=false does not work
    find "$BUNDLE_DIR" -type l -print -delete;
}

function install_idf_package()
{
    TOOL_URL="$1"
    TOOL_FILE="$2"
    TOOL_VERSION="$3"

    if [[ ! -f "${TOOL_FILE}" ]]; then
        wget --no-verbose -O "${TOOL_FILE}" "${TOOL_URL}"
    fi

    if [[ ! -d "${TOOL_VERSION}" ]]; then
        mkdir -p "${TOOL_VERSION}"
        unzip -q "${TOOL_FILE}" -d "${TOOL_VERSION}"
    fi

}

function install_idf_python()
{
    install_idf_package \
        "https://dl.espressif.com/dl/idf-python/idf-python-$IDF_PYTHON_VERSION-embed-win64.zip" \
        "${IDF_TOOLS_PATH}/idf-python-${IDF_PYTHON_VERSION}-embed-win64.zip" \
        "tools/idf-python/${IDF_PYTHON_VERSION}"
}

function install_idf_python_wheels()
{
    install_idf_package \
        "https://dl.espressif.com/dl/idf-python-wheels/idf-python-wheels-$IDF_PYTHON_WHEELS_VERSION-win64.zip" \
        "${IDF_TOOLS_PATH}/idf-python-wheels-${IDF_PYTHON_WHEELS_VERSION}-win64.zip" \
        "tools/idf-python-wheels/${IDF_PYTHON_WHEELS_VERSION}"
}

function install_idf_git()
{
    IDF_FILE="Git-${IDF_GIT_VERSION}-64-bit.exe"
    if [[ -f "${IDF_FILE}" ]]; then
        return
    fi
    mkdir -p "${IDF_TOOLS_PATH}/dist/"
    wget -nc --no-verbose -O "${IDF_TOOLS_PATH}/dist/${IDF_FILE}" "https://github.com/git-for-windows/git/releases/download/${IDF_GIT_VERSION_DIR}/${IDF_FILE}" || echo "exists"
    if [[ `file -b "${IDF_TOOLS_PATH}/dist/${IDF_FILE}"` != "PE32 executable (GUI) Intel 80386, for MS Windows" ]]; then
        echo "Git installer is not valid Windows Executable"
        exit 1
    fi
}

function download_idf_versions()
{
    echo "Downloading idf_versions.txt..."
    wget --no-verbose -O idf_versions.txt https://dl.espressif.com/dl/esp-idf/idf_versions.txt
}

function build_with_wine()
{
    xvfb-run-wine /opt/wine/drive_c/Program\ Files\ \(x86\)/Inno\ Setup\ 6/ISCC.exe $ISCC_PARAMS idf_tool_setup.iss
}

# Check for ISCC on Windows WSL2
iscc_path=$(which ISCC.exe) || echo "ISCC.exe not found. If running in WSL2, install ISCC by following command: choco install innosetup"
if [[ -z "$iscc_path" ]]; then
    echo "Searching for iscc"
    iscc_path=$(which iscc)
    if [[ -z "$iscc_path" ]]; then
        echo "Inno setup compiler (iscc) not found. Are you running wine-innosetup Docker image?"
        exit 1
    fi
fi

if [ ! -d "unzip" ]; then
    echo "Downloading 7z..."
    mkdir -p unzip
    pushd unzip
    wget --no-verbose -O 7z1900-extra.7z https://www.7-zip.org/a/7z1900-extra.7z
    7zr e -y 7z1900-extra.7z
    popd
fi

if [[ -z "${IDF_PATH:-}" ]]; then
    export IDF_PATH=$(cd ../../../; pwd)
    echo "Assuming IDF_PATH: ${IDF_PATH}"
fi

export IDF_TOOLS_PATH="${PWD}/idf_tools_tmp_${INSTALLER_TYPE}"
mkdir -p "${IDF_TOOLS_PATH}"
echo "Using IDF_TOOLS_PATH specific for installer type: ${IDF_TOOLS_PATH}"

# Clean up production dist, data will be transferred from helper dist specific for installer type
if [[ -d "dist" ]]; then
    rm -rf dist
fi
mkdir -p dist

if [[ "$INSTALLER_TYPE" == "precached" ]]; then
    ISCC_PARAMS="/DOFFLINE=no /DCOMPRESSION=$COMPRESSION /DSOLIDCOMPRESSION=no /DPYTHONWHEELSVERSION= /DGITVERSION=${IDF_GIT_VERSION} /DGITVERSIONDIR=${IDF_GIT_VERSION_DIR}"
    download_idf_versions
    $IDF_PATH/tools/idf_tools.py --non-interactive download --platform Windows-x86_64 all
    $IDF_PATH/tools/idf_tools.py --tools-json tools_fallback.json --non-interactive download --platform Windows-x86_64 all
    cp $IDF_TOOLS_PATH/dist/* dist/
elif [[ "$INSTALLER_TYPE" == "online" ]]; then
    ISCC_PARAMS="/DOFFLINE=no /DCOMPRESSION=$COMPRESSION /DSOLIDCOMPRESSION=no /DPYTHONWHEELSVERSION= /DGITVERSION=${IDF_GIT_VERSION}  /DGITVERSIONDIR=${IDF_GIT_VERSION_DIR}"
    download_idf_versions
    install_idf_python
    rm -rf tools/idf-python-wheels
elif [[ "$INSTALLER_TYPE" == "offline" ]]; then
    # Turn off also solid compression - it causes delay in start time of installer.
    ISCC_PARAMS="/DOFFLINE=yes /DCOMPRESSION=$COMPRESSION /DSOLIDCOMPRESSION=no /DPYTHONWHEELSVERSION=$IDF_PYTHON_WHEELS_VERSION /DGITVERSION=${IDF_GIT_VERSION}  /DGITVERSIONDIR=${IDF_GIT_VERSION_DIR}"

    install_idf_git
    install_idf_python
    install_idf_python_wheels

    cp idf_versions_offline.txt idf_versions.txt
    prepare_offline_branches
    cp $IDF_TOOLS_PATH/dist/* dist/
elif [[ "$INSTALLER_TYPE" == "draft" ]]; then
    ISCC_PARAMS="/DOFFLINE=yes /DCOMPRESSION=$COMPRESSION /DSOLIDCOMPRESSION=no /DPYTHONWHEELSVERSION=$IDF_PYTHON_WHEELS_VERSION /DGITVERSION=${IDF_GIT_VERSION}  /DGITVERSIONDIR=${IDF_GIT_VERSION_DIR}"
else
    echo "Uknown type of installer: $INSTALLER_TYPE"
    exit 1
fi

# Check for cmdlinerunner
if [[ ! -f "cmdlinerunner/build/cmdlinerunner.dll" ]]; then
    echo "cmdlinerunner not found, downloading"
    wget --no-verbose -O $IDF_TOOLS_PATH/idf-cmdlinerunner-1.0.zip https://dl.espressif.com/dl/idf-cmdlinerunner/idf-cmdlinerunner-1.0.zip
    mkdir -p cmdlinerunner/build
    unzip -q $IDF_TOOLS_PATH/idf-cmdlinerunner-1.0.zip -d cmdlinerunner/build/
    rm $IDF_TOOLS_PATH/idf-cmdlinerunner-1.0.zip
fi

echo "Running ISCC..."
# https://jrsoftware.org/ishelp/index.php?topic=compilercmdline
echo "iscc $ISCC_PARAMS idf_tool_setup.iss"

# Check whether we should run wine in case of docker image
which xvfb-run-wine && \
    build_with_wine ||
    iscc $ISCC_PARAMS idf_tool_setup.iss

mv "Output/esp-idf-tools-setup-unsigned.exe" "Output/esp-idf-tools-setup-${INSTALLER_TYPE}-unsigned.exe"
