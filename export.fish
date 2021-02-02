# This script should be sourced, not executed.

function idf_export_main
    # The file doesn't have executable permissions, so this shouldn't really happen.
    # Doing this in case someone tries to chmod +x it and execute...
    set cmd (status current-command)
    if test $cmd != source -a $cmd != .
        echo "his script should be sourced, not executed:"
        echo . (realpath (status --current-filename)/..)
        return 1
    end

    if not set -q IDF_PATH
        echo "IDF_PATH must be set before sourcing this script"
        return 1
    end

    set oldpath = $PATH

    echo "Detecting the Python interpreter"
    source "$IDF_PATH"/tools/detect_python.fish

    echo "Adding ESP-IDF tools to PATH..."
    # Call idf_tools.py to export tool paths
    set -x IDF_TOOLS_EXPORT_CMD "$IDF_PATH"/export.fish
    set -x IDF_TOOLS_INSTALL_CMD "$IDF_PATH"/install.fish
    set idf_exports ("$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py export) || return 1
    eval "$idf_exports"

    echo "Checking if Python packages are up to date..."
    python "$IDF_PATH"/tools/check_python_dependencies.py || return 1

    # Allow calling some IDF python tools without specifying the full path
    # "$IDF_PATH"/tools is already added by 'idf_tools.py export'
    set IDF_ADD_PATHS_EXTRAS "$IDF_PATH"/components/esptool_py/esptool
    set IDF_ADD_PATHS_EXTRAS "$IDF_ADD_PATHS_EXTRAS":"$IDF_PATH"/components/espcoredump
    set IDF_ADD_PATHS_EXTRAS "$IDF_ADD_PATHS_EXTRAS":"$IDF_PATH"/components/partition_table
    set IDF_ADD_PATHS_EXTRAS "$IDF_ADD_PATHS_EXTRAS":"$IDF_PATH"/components/app_update
    set -x PATH "$IDF_ADD_PATHS_EXTRAS":"$PATH"

    set added_path_variables
    for entry in $PATH;
        if not contains $entry $oldpath
            set -a added_path_variables $entry
        end
    end
    if set -q added_path_variables[1]
        echo "Added the following directories to PATH:"
        for entry in $added_path_variables;
            echo $entry
        end
    else
        echo "All paths are already set."
    end

    # Clean up
    set -e added_path_variables
    set -e cmd
    set -e old_path
    set -e paths
    set -e path_prefix
    set -e path_entry
    set -e IDF_ADD_PATHS_EXTRAS
    set -e idf_exports
    set -e ESP_PYTHON

    # Not unsetting IDF_PYTHON_ENV_PATH, it can be used by IDF build system
    # to check whether we are using a private Python environment

    echo "Done! You can now compile ESP-IDF projects."
    echo "Go to the project directory and run:"
    echo ""
    echo "  idf.py build"
    echo ""
end

idf_export_main

set -e idf_export_main