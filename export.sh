# This script should be sourced, not executed.

# Emergency backup option to use previous export.sh (export_legacy.sh) if the new export approach fails.
# To use it, set environmental variable like: export ESP_IDF_LEGACY_EXPORT=1
if [ -n "${ESP_IDF_LEGACY_EXPORT-}" ]; then
    . ./tools/legacy_exports/export_legacy.sh
    return $?
fi

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

shell_type="detect"

# shellcheck disable=SC2128,SC2169,SC2039,SC3054,SC3028 # ignore array expansion warning
if [ -n "${BASH_SOURCE-}" ]
then
    # shellcheck disable=SC3028,SC3054 # unreachable with 'dash'
    idf_path=$(dirname "${BASH_SOURCE[0]}")
    shell_type="bash"
elif [ -n "${ZSH_VERSION-}" ]
then
    # shellcheck disable=SC2296  # ignore parameter starts with '{' because it's zsh
    idf_path=$(dirname "${(%):-%x}")
    shell_type="zsh"
elif [ -n "${IDF_PATH-}" ]
then
    if [ -f "/.dockerenv" ]
    then
        echo "Using the IDF_PATH found in the environment as docker environment detected."
        idf_path=$IDF_PATH
    elif [ -n "${IDF_PATH_FORCE-}" ]
    then
        echo "Using the forced IDF_PATH found in the environment."
        idf_path=$IDF_PATH
    fi
fi

if [ ! -f "${idf_path}/tools/idf.py" ] ||
   [ ! -f "${idf_path}/tools/idf_tools.py" ] ||
   [ ! -f "${idf_path}/tools/activate.py" ]
then
    echo "Could not automatically detect IDF_PATH from script location. Please navigate to your ESP-IDF directory and run:"
    echo ". ./export.sh"
    if [ -n "${IDF_PATH-}" ]
    then
        echo
        echo "To use the IDF_PATH set in the environment, you can enforce it by setting 'export IDF_PATH_FORCE=1'"
    fi
    unset idf_path
    return 1
fi

. "${idf_path}/tools/detect_python.sh"

# Evaluate the ESP-IDF environment set up by the activate.py script.
idf_exports=$("$ESP_PYTHON" "${idf_path}/tools/activate.py" --export --shell $shell_type)
eval "${idf_exports}"
unset idf_path
return 0
