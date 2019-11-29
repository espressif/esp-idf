#!/usr/bin/env bash

set -e
set -u

export IDF_PATH=$(cd $(dirname $0); pwd)
echo "#IDF PATH for ESP-IDF" >> ~/.zshrc
echo "IDF_PATH=$IDF_PATH" >> ~/.zshrc

echo "Installing ESP-IDF tools"
${IDF_PATH}/tools/idf_tools.py install

echo "Installing Python environment and packages"
${IDF_PATH}/tools/idf_tools.py install-python-env

basedir="$(dirname $0)"
echo "All done! You can now run:"
echo ""
echo "  . ${basedir}/export.sh"
echo ""
