#!/usr/bin/env fish

set basedir $PWD

set -x IDF_PATH $basedir

echo "Installing ESP-IDF tools"
"$IDF_PATH"/tools/idf_tools.py install

echo "Installing Python environment and packages"
"$IDF_PATH"/tools/idf_tools.py install-python-env

echo "All done! You can now run:"
echo ""
echo "  . "$basedir"/export.fish"
echo ""