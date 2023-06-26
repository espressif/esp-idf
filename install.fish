#!/usr/bin/env fish

set basedir (realpath (dirname (status -f)))

set -x IDF_PATH $basedir

echo "Detecting the Python interpreter"
source "$IDF_PATH"/tools/detect_python.fish

echo "Checking Python compatibility"
"$ESP_PYTHON" "$IDF_PATH"/tools/python_version_checker.py

for option in $argv
    switch "$option"
        case -h --help
            "$ESP_PYTHON" "$IDF_PATH"/tools/install_util.py print_help fish
            exit
    end
end

set TARGETS ("$ESP_PYTHON" "$IDF_PATH"/tools/install_util.py extract targets $argv) || exit 1

echo "Installing ESP-IDF tools"
"$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py install --targets=$TARGETS
or exit 1

set FEATURES ("$ESP_PYTHON" "$IDF_PATH"/tools/install_util.py extract features $argv) || exit 1

echo "Installing Python environment and packages"
"$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py install-python-env --features=$FEATURES

echo "All done! You can now run:"
echo ""
echo "  . "$basedir"/export.fish"
echo ""
