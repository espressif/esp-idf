# This file should be sourced, not executed!
#
# This is a helper script for detecting Python executables in the PATH. It is intended to be used for determining
# which Python should be used with idf_tools.py for installing tools and exporting environment variables.
#
# 1. The script will set variable ESP_PYTHON to "python" if it is of version 3.
# 2. Otherwise, "python3" will be exported if it exists.
# 3. The script will fall-back to "python" as the last resort and fail if it doesn't exist.

ESP_PYTHON=python

for p_cmd in python python3
do
    echo "Checking \"$p_cmd\" ..."

    if [ "$($p_cmd -c "import sys; print(sys.version_info.major)")" = 3 ]; then
        ESP_PYTHON=$p_cmd
        break
    fi
done

$ESP_PYTHON --version || { echo "\"$ESP_PYTHON\" is not installed! Please see the documentation for how to install it."; exit 1; }

echo "\"$ESP_PYTHON\" has been detected"
