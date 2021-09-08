#!/usr/bin/env fish

set basedir $PWD

set -x IDF_PATH $basedir

echo "Detecting the Python interpreter"
source "$IDF_PATH"/tools/detect_python.fish

if not set -q argv[1]
    set TARGETS "all"
else
    set TARGETS $argv[1]
end
echo "Installing ESP-IDF tools"
"$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py install --targets=$TARGETS
or exit 1

echo "Installing Python environment and packages"
"$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py install-python-env

echo "All done! You can now run:"
echo ""
echo "  . "$basedir"/export.fish"
echo ""