# This script should be sourced, not executed.

function idf_export_main() {
    # The file doesn't have executable permissions, so this shouldn't really happen.
    # Doing this in case someone tries to chmod +x it and execute...
    if [[ -n "${BASH_SOURCE}" && ( "${BASH_SOURCE[0]}" == "${0}" ) ]]; then
        echo "This script should be sourced, not executed:"
        echo ". ${BASH_SOURCE[0]}"
        return 1
    fi

    if [[ -z "${IDF_PATH}" ]]
    then
        # If using bash, try to guess IDF_PATH from script location
        if [[ -n "${BASH_SOURCE}" ]]
        then
            script_name="$(readlink -f $BASH_SOURCE)"
            export IDF_PATH="$(dirname ${script_name})"
        else
            echo "IDF_PATH must be set before sourcing this script"
            return 1
        fi
    fi

    old_path=$PATH

    echo "Adding ESP-IDF tools to PATH..."
    # Call idf_tools.py to export tool paths
    export IDF_TOOLS_EXPORT_CMD=${IDF_PATH}/export.sh
    export IDF_TOOLS_INSTALL_CMD=${IDF_PATH}/install.sh
    idf_exports=$(${IDF_PATH}/tools/idf_tools.py export) || return 1
    eval "${idf_exports}"

    echo "Checking if Python packages are up to date..."
    python ${IDF_PATH}/tools/check_python_dependencies.py || return 1


    # Allow calling some IDF python tools without specifying the full path
    # ${IDF_PATH}/tools is already added by 'idf_tools.py export'
    IDF_ADD_PATHS_EXTRAS="${IDF_PATH}/components/esptool_py/esptool"
    IDF_ADD_PATHS_EXTRAS="${IDF_ADD_PATHS_EXTRAS}:${IDF_PATH}/components/espcoredump"
    IDF_ADD_PATHS_EXTRAS="${IDF_ADD_PATHS_EXTRAS}:${IDF_PATH}/components/partition_table/"
    export PATH="${IDF_ADD_PATHS_EXTRAS}:${PATH}"

    if [[ -n "$BASH" ]]
    then
        path_prefix=${PATH%%${old_path}}
        paths="${path_prefix//:/ }"
        if [ -n "${paths}" ]; then
            echo "Added the following directories to PATH:"
        else
            echo "All paths are already set."
        fi
        for path_entry in ${paths}
        do
            echo "  ${path_entry}"
        done
    else
        echo "Updated PATH variable:"
        echo "  ${PATH}"
    fi

    # Clean up
    unset old_path
    unset paths
    unset path_prefix
    unset path_entry
    unset IDF_ADD_PATHS_EXTRAS
    unset idf_exports
    # Not unsetting IDF_PYTHON_ENV_PATH, it can be used by IDF build system
    # to check whether we are using a private Python environment

    echo "Done! You can now compile ESP-IDF projects."
    echo "Go to the project directory and run:"
    echo ""
    echo "  idf.py build"
    echo ""
}

idf_export_main

unset idf_export_main
