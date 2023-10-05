# This script should be sourced, not executed.

__realpath() {
    wdir="$PWD"; [ "$PWD" = "/" ] && wdir=""
    arg=$1
    case "$arg" in
        /*) scriptdir="${arg}";;
        *) scriptdir="$wdir/${arg#./}";;
    esac
    scriptdir="${scriptdir%/*}"
    echo "$scriptdir"
}


__verbose() {
    [ -n "${IDF_EXPORT_QUIET-}" ] && return
    echo "$@"
}

__script_dir(){
    # shellcheck disable=SC2169,SC2169,SC2039,SC3010,SC3028  # unreachable with 'dash'
    if [ "$(uname -s)" = "Darwin" ]; then
        # convert possibly relative path to absolute
        script_dir="$(__realpath "${self_path}")"
        # resolve any ../ references to make the path shorter
        script_dir="$(cd "${script_dir}" || exit 1; pwd)"
    else
        # convert to full path and get the directory name of that
        script_name="$(readlink -f "${self_path}")"
        script_dir="$(dirname "${script_name}")"
    fi
    if [ "$script_dir" = '.' ]
    then
       script_dir="$(pwd)"
    fi
    echo "$script_dir"
}

__is_dir_esp_idf(){
    if [ ! -f "$1/tools/idf.py" ] || [ ! -f "$1/tools/idf_tools.py" ]
    then
        # Echo command here is not used for printing to the terminal, but as non-empty return value from function.
        echo "THIS DIRECTORY IS NOT ESP-IDF"
    fi
}

__main() {
    # The file doesn't have executable permissions, so this shouldn't really happen.
    # Doing this in case someone tries to chmod +x it and execute...

    # shellcheck disable=SC2128,SC2169,SC2039,SC3054 # ignore array expansion warning
    if [ -n "${BASH_SOURCE-}" ] && [ "${BASH_SOURCE[0]}" = "${0}" ]
    then
        echo "This script should be sourced, not executed:"
        # shellcheck disable=SC2039,SC3054  # reachable only with bash
        echo ". ${BASH_SOURCE[0]}"
        return 1
    fi

    # If using bash or zsh, try to guess IDF_PATH from script location.
    self_path=""
    # shellcheck disable=SC2128  # ignore array expansion warning
    if [ -n "${BASH_SOURCE-}" ]
    then
        self_path="${BASH_SOURCE}"
    elif [ -n "${ZSH_VERSION-}" ]
    then
    # shellcheck disable=SC2296  # ignore parameter starts with '{' because it's zsh
        self_path="${(%):-%x}"
    fi

    script_dir="$(__script_dir)"
    # Since sh or dash shells can't detect script_dir correctly, check if script_dir looks like an IDF directory
    is_script_dir_esp_idf=$(__is_dir_esp_idf "${script_dir}")

    if [ -z "${IDF_PATH-}" ]
    then
        # IDF_PATH not set in the environment.

        if [ -n "${is_script_dir_esp_idf}" ]
        then
            echo "Could not detect IDF_PATH. Please set it before sourcing this script:"
            echo "  export IDF_PATH=(add path here)"
            return 1
        fi
        export IDF_PATH="${script_dir}"
        echo "Setting IDF_PATH to '${IDF_PATH}'"
    else
        # IDF_PATH came from the environment, check if the path is valid
        # Set IDF_PATH to script_dir, if script_dir looks like an IDF directory
        if [ ! "${IDF_PATH}" = "${script_dir}" ] && [ -z "${is_script_dir_esp_idf}" ]
        then
            # Change IDF_PATH is important when there are 2 ESP-IDF versions in different directories.
            # Sourcing this script without change, would cause sourcing wrong export script.
            echo "Resetting IDF_PATH from '${IDF_PATH}' to '${script_dir}' "
            export IDF_PATH="${script_dir}"
        fi
        # Check if this path looks like an IDF directory
        is_idf_path_esp_idf=$(__is_dir_esp_idf "${IDF_PATH}")
        if [ -n "${is_idf_path_esp_idf}" ]
        then
            echo "IDF_PATH is set to '${IDF_PATH}', but it doesn't look like an ESP-IDF directory."
            echo "If you have set IDF_PATH manually, check if the path is correct."
            return 1
        fi

        # The varible might have been set (rather than exported), re-export it to be sure
        export IDF_PATH="${IDF_PATH}"
    fi

    old_path="$PATH"

    echo "Detecting the Python interpreter"
    . "${IDF_PATH}/tools/detect_python.sh"

    echo "Checking Python compatibility"
    "$ESP_PYTHON" "${IDF_PATH}/tools/python_version_checker.py"

    __verbose "Checking other ESP-IDF version."
    idf_deactivate=$("$ESP_PYTHON" "${IDF_PATH}/tools/idf_tools.py" export --deactivate) || return 1
    eval "${idf_deactivate}"

    __verbose "Adding ESP-IDF tools to PATH..."
    # Call idf_tools.py to export tool paths
    export IDF_TOOLS_EXPORT_CMD=${IDF_PATH}/export.sh
    export IDF_TOOLS_INSTALL_CMD=${IDF_PATH}/install.sh
    # Allow calling some IDF python tools without specifying the full path
    # ${IDF_PATH}/tools is already added by 'idf_tools.py export'
    IDF_ADD_PATHS_EXTRAS="${IDF_PATH}/components/espcoredump"
    IDF_ADD_PATHS_EXTRAS="${IDF_ADD_PATHS_EXTRAS}:${IDF_PATH}/components/partition_table"
    IDF_ADD_PATHS_EXTRAS="${IDF_ADD_PATHS_EXTRAS}:${IDF_PATH}/components/app_update"

    idf_exports=$("$ESP_PYTHON" "${IDF_PATH}/tools/idf_tools.py" export "--add_paths_extras=${IDF_ADD_PATHS_EXTRAS}") || return 1
    eval "${idf_exports}"
    export PATH="${IDF_ADD_PATHS_EXTRAS}:${PATH}"

    __verbose "Checking if Python packages are up to date..."
    "$ESP_PYTHON" "${IDF_PATH}/tools/idf_tools.py" check-python-dependencies || return 1

    if [ -n "$BASH" ]
    then
        path_prefix="${PATH%%"${old_path}"}"
        # shellcheck disable=SC2169,SC2039  # unreachable with 'dash'
        if [ -n "${path_prefix}" ]; then
            __verbose "Added the following directories to PATH:"
        else
            __verbose "All paths are already set."
        fi
        old_ifs="$IFS"
        IFS=":"
        for path_entry in ${path_prefix}
        do
            __verbose "  ${path_entry}"
        done
        IFS="$old_ifs"
        unset old_ifs
    else
        __verbose "Updated PATH variable:"
        __verbose "  ${PATH}"
    fi

    uninstall=$("$ESP_PYTHON" "${IDF_PATH}/tools/idf_tools.py" uninstall --dry-run) || return 1
    if [ -n "$uninstall" ]
    then
        __verbose ""
        __verbose "Detected installed tools that are not currently used by active ESP-IDF version."
        __verbose "${uninstall}"
        __verbose "To free up even more space, remove installation packages of those tools. Use option '${ESP_PYTHON} ${IDF_PATH}/tools/idf_tools.py uninstall --remove-archives'."
        __verbose ""
    fi

    __verbose "Done! You can now compile ESP-IDF projects."
    __verbose "Go to the project directory and run:"
    __verbose ""
    __verbose "  idf.py build"
    __verbose ""
}

__cleanup() {
    unset old_path
    unset paths
    unset path_prefix
    unset path_entry
    unset IDF_ADD_PATHS_EXTRAS
    unset idf_exports
    unset idf_deactivate
    unset ESP_PYTHON
    unset SOURCE_ZSH
    unset SOURCE_BASH
    unset WARNING_MSG
    unset uninstall
    unset is_idf_path_esp_idf
    unset is_script_dir_esp_idf

    unset __realpath
    unset __main
    unset __verbose
    unset __enable_autocomplete
    unset __cleanup
    unset __is_dir_esp_idf

    # Not unsetting IDF_PYTHON_ENV_PATH, it can be used by IDF build system
    # to check whether we are using a private Python environment

    return "$1"
}


__enable_autocomplete() {
    click_version="$(python -c 'import click; print(click.__version__.split(".")[0])')"
    if [ "${click_version}" -lt 8 ]
    then
        SOURCE_ZSH=source_zsh
        SOURCE_BASH=source_bash
    else
        SOURCE_ZSH=zsh_source
        SOURCE_BASH=bash_source
    fi
    if [ -n "${ZSH_VERSION-}" ]
    then
        autoload -Uz compinit && compinit -u
        eval "$(env _IDF.PY_COMPLETE=$SOURCE_ZSH idf.py)" || echo "WARNING: Failed to load shell autocompletion for zsh version: $ZSH_VERSION!"
    elif [ -n "${BASH_SOURCE-}" ]
    then
        WARNING_MSG="WARNING: Failed to load shell autocompletion for bash version: $BASH_VERSION!"
        # shellcheck disable=SC3028,SC3054,SC2086,SC2169  # code block for 'bash' only
        [ ${BASH_VERSINFO[0]} -lt 4 ] && { echo "$WARNING_MSG"; return; }
        eval "$(env LANG=en _IDF.PY_COMPLETE=$SOURCE_BASH idf.py)"  || echo "$WARNING_MSG"
    fi
}

__main && __enable_autocomplete
__cleanup $?
