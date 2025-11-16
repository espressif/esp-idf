# This file should be sourced, not executed!
#
# This is a helper script for detecting Python executables in the PATH. It is intended to be used for determining
# which Python should be used with idf_tools.py for installing tools and exporting environment variables.
#
# 1. If environment variable ESP_PYTHON already set, script uses that python binary, otherwise, looks for python version same or greater than minimal required version on path
# 2. If required version of python is found it is assigned to environmental variable `ESP_PYTHON`
# 3. If required version of python is not found, script will fail

OLDEST_PYTHON_SUPPORTED_MAJOR=3
OLDEST_PYTHON_SUPPORTED_MINOR=10


if [[ -z "$ESP_PYTHON" ]]; then
    PYTHON_CANDIDATES=(python3 python python3.10 python3.11 python3.12 python3.13)
else
    echo "Reading ESP_PYTHON from environment: \"$ESP_PYTHON\""
    PYTHON_CANDIDATES=("$ESP_PYTHON")
fi
ESP_PYTHON=  # Unset it. Will assign after candidate check

for p_cmd in "${PYTHON_CANDIDATES[@]}"; do
    "$p_cmd" --version >/dev/null 2>&1 || continue
    echo "Checking \"$p_cmd\" ..."

    "$p_cmd" -c "import sys; exit(1) if sys.version_info.major < int(\"$OLDEST_PYTHON_SUPPORTED_MAJOR\") else exit(0);" || continue
    "$p_cmd" -c "import sys; exit(1) if sys.version_info.minor < int(\"$OLDEST_PYTHON_SUPPORTED_MINOR\") else exit(0);" || continue

    export ESP_PYTHON="$p_cmd"
    break
done
unset PYTHON_CANDIDATES

[[ -n "$ESP_PYTHON" ]] &&  "$ESP_PYTHON" --version 2>/dev/null || {
    echo "Python ${OLDEST_PYTHON_SUPPORTED_MAJOR}.${OLDEST_PYTHON_SUPPORTED_MINOR}+ is not installed! Please see the documentation for how to install it." >&2
    exit 1
}
echo "\"$ESP_PYTHON\" has been detected"
