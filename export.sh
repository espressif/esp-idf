# This script should be sourced, not executed.

# shellcheck disable=SC2128,SC2169,SC2039,SC3054 # ignore array expansion warning
if [ -n "${BASH_SOURCE-}" ] && [ "${BASH_SOURCE[0]}" = "${0}" ]
then
    echo "This script should be sourced, not executed:"
    # shellcheck disable=SC2039,SC3054  # reachable only with bash
    echo ". ${BASH_SOURCE[0]}"
    exit 1
fi

# Attempt to identify the ESP-IDF directory
idf_path="."

# shellcheck disable=SC2128,SC2169,SC2039,SC3054,SC3028 # ignore array expansion warning
if test -n "${BASH_SOURCE-}"
then
    # shellcheck disable=SC3028,SC3054 # unreachable with 'dash'
    idf_path=$(dirname "${BASH_SOURCE[0]}")
elif test -n "${ZSH_VERSION-}"
then
    # shellcheck disable=SC2296  # ignore parameter starts with '{' because it's zsh
    idf_path=$(dirname "${(%):-%x}")
elif test -n "${IDF_PATH-}"
then
    idf_path=$IDF_PATH
fi

if [ ! -f "${idf_path}/tools/idf.py" ] ||
    [ ! -f "${idf_path}/tools/idf_tools.py" ] ||
    [ ! -f "${idf_path}/activate.py" ]
then
    # Echo command here is not used for printing to the terminal, but as non-empty return value from function.
    echo "Could not detect IDF_PATH. Please set it before sourcing this script:"
    echo "  export IDF_PATH=(add path here)"
    unset idf_path
    return 1
fi

# TODO Maybe we can use "command -v" to check just for python and python3
. "${idf_path}/tools/detect_python.sh"

# Evaluate the ESP-IDF environment set up by the activate.py script.
idf_exports=$("$ESP_PYTHON" "${idf_path}/activate.py" --export)
eval "${idf_exports}"
unset idf_path
return 0
