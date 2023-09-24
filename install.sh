#!/usr/bin/env bash

set -e
set -u

basedir=$(dirname "$0")
IDF_PATH=$(cd "${basedir}"; pwd -P)
export IDF_PATH

echo "Detecting the Python interpreter"
. "${IDF_PATH}/tools/detect_python.sh"

echo "Checking Python compatibility"
"${ESP_PYTHON}" "${IDF_PATH}/tools/python_version_checker.py"

while getopts ":h" option; do
    case $option in
        h)
            "${ESP_PYTHON}" "${IDF_PATH}/tools/install_util.py" print_help sh
            exit;;
        \?)
            ;;
    esac
done

TARGETS=$("${ESP_PYTHON}" "${IDF_PATH}/tools/install_util.py" extract targets "$@")

echo "Installing ESP-IDF tools"
"${ESP_PYTHON}" "${IDF_PATH}/tools/idf_tools.py" install --targets="${TARGETS}"

FEATURES=$("${ESP_PYTHON}" "${IDF_PATH}/tools/install_util.py" extract features "$@")

echo "Installing Python environment and packages"
"${ESP_PYTHON}" "${IDF_PATH}/tools/idf_tools.py" install-python-env --features="${FEATURES}"

echo "All done! You can now run:"
echo ""
echo "  . ${basedir}/export.sh"
echo ""
