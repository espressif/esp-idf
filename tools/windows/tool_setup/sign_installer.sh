#!/bin/bash
#
# Script to sign the IDF Tools installer for Windows, built with build_installer.sh.
#

set -e
set -u

if [[ -z "${KEYFILE:-}" || -z "${CERTCHAIN:-}" ]]; then
    echo "To sign the installer, set the following environment variables:"
    echo "  KEYFILE - private key file"
    echo "  KEYPASSWORD - password for the private key file (optional, will prompt for password if not set)"
    echo "  CERTCHAIN - certificate chain file"
    exit 1
fi

umask 770  # for the process substitution FIFO

VERSION=`grep "#define MyAppVersion " idf_tool_setup.iss | cut -d ' ' -f3 | tr -d '"'`
echo "Installer version ${VERSION}"

IN_FILE="Output/esp-idf-tools-setup-unsigned.exe"
OUT_FILE="Output/esp-idf-tools-setup-${VERSION}.exe"

if [[ -n "${KEYPASSWORD:-}" ]]; then
    PASSARG="-readpass <(echo \"$KEYPASSWORD\")"
else
    PASSARG="-askpass"
fi

echo "Signing the installer (${IN_FILE})..."
# Note: The cert chain passed to -certs needs to contain the intermediate
# cert(s) as well, appended after the code signing cert, or Windows may see
# it as "Unknown Publisher"
#
# See https://stackoverflow.com/a/52637050 for full details
#
osslsigncode -certs ${CERTCHAIN} -key ${KEYFILE} \
             ${PASSARG} \
             -in ${IN_FILE} \
             -out ${OUT_FILE} \
             -h sha256 \
             -n "Espressif Systems (Shanghai) Co., Ltd." \
             -i "https://www.espressif.com/" \
             -ts http://timestamp.digicert.com

chmod 644 ${OUT_FILE}  # make up for the umask

echo "Generated ${OUT_FILE}"
