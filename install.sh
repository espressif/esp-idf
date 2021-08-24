#!/usr/bin/env bash

set -e
set -u

export IDF_PATH=$(cd $(dirname $0); pwd)

echo "Detecting the Python interpreter"
. ${IDF_PATH}/tools/detect_python.sh

if [ "$#" -eq 0 ]; then
  TARGETS="all"
else
  TARGETS=$1
fi
echo "Installing ESP-IDF tools"
${ESP_PYTHON} ${IDF_PATH}/tools/idf_tools.py install --targets=${TARGETS}

echo "Installing Python environment and packages"
${ESP_PYTHON} ${IDF_PATH}/tools/idf_tools.py install-python-env

basedir="$(dirname $0)"
echo "All done! You can now run:"
echo ""
echo "  . ${basedir}/export.sh"
echo ""
