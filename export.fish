# This script should be sourced, not executed.

# `idf_tools.py export --deactivate` create statement, with keyword unset, but fish shell support only `set --erase variable`
function unset
    set --erase $argv
end

function __main
    set script_dir (dirname (realpath (status -f)))
    if not set -q IDF_PATH
        set -gx IDF_PATH $script_dir
        echo "Setting IDF_PATH to '$IDF_PATH'"
    end

    if test "$IDF_PATH" != "$script_dir"
        # Change IDF_PATH is important when there are 2 ESP-IDF versions in different directories.
        # Sourcing this script without change, would cause sourcing wrong export script.
        echo "Resetting IDF_PATH from '$IDF_PATH' to '$script_dir'"
        set IDF_PATH "$script_dir"
    end

    set oldpath = $PATH

    echo "Detecting the Python interpreter"
    source "$IDF_PATH"/tools/detect_python.fish

    echo "Checking Python compatibility"
    "$ESP_PYTHON" "$IDF_PATH"/tools/python_version_checker.py

    echo "Checking other ESP-IDF version."
    set idf_deactivate ("$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py export --deactivate) || return 1
    eval "$idf_deactivate"

    echo "Adding ESP-IDF tools to PATH..."
    # Call idf_tools.py to export tool paths
    set -gx IDF_TOOLS_EXPORT_CMD "$IDF_PATH"/export.fish
    set -gx IDF_TOOLS_INSTALL_CMD "$IDF_PATH"/install.fish
    # Allow calling some IDF python tools without specifying the full path
    # "$IDF_PATH"/tools is already added by 'idf_tools.py export'
    set IDF_ADD_PATHS_EXTRAS "$IDF_PATH"/components/espcoredump
    set IDF_ADD_PATHS_EXTRAS "$IDF_ADD_PATHS_EXTRAS":"$IDF_PATH"/components/partition_table
    set IDF_ADD_PATHS_EXTRAS "$IDF_ADD_PATHS_EXTRAS":"$IDF_PATH"/components/app_update

    set idf_exports ("$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py export --add_paths_extras="$IDF_ADD_PATHS_EXTRAS") || return 1
    eval "$idf_exports"
    set -x PATH "$IDF_ADD_PATHS_EXTRAS":"$PATH"

    echo "Checking if Python packages are up to date..."
    "$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py check-python-dependencies || return 1

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

    set uninstall ("$ESP_PYTHON" "$IDF_PATH"/tools/idf_tools.py uninstall --dry-run) || return 1
    if test -n "$uninstall"
        echo ""
        echo "Detected installed tools that are not currently used by active ESP-IDF version."
        echo "$uninstall"
        echo "For free up even more space, remove installation packages of those tools. Use option '$ESP_PYTHON $IDF_PATH/tools/idf_tools.py uninstall --remove-archives'."
        echo ""
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
    set -e uninstall
    set -e script_dir
    set -e idf_deactivate


    # Not unsetting IDF_PYTHON_ENV_PATH, it can be used by IDF build system
    # to check whether we are using a private Python environment

    echo "Done! You can now compile ESP-IDF projects."
    echo "Go to the project directory and run:"
    echo ""
    echo "  idf.py build"
    echo ""
end

__main

set click_version (python -c 'import click; print(click.__version__.split(".")[0])')
if test $click_version -lt 8
    eval (env _IDF.PY_COMPLETE=source_fish idf.py)
else
    eval (env _IDF.PY_COMPLETE=fish_source idf.py)
end

functions -e __main
