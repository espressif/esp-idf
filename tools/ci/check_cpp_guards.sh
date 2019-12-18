#!/bin/bash
#
# This script finds all files which do not contain "#ifdef __cplusplus", except the ones listed in check_cpp_guards_exceptions.txt

set -euo pipefail

if [ -z "${IDF_PATH:-}" ]; then
    echo "Please run export.sh before invoking this script"
    exit 1
fi

##################### helper functions #####################

# check_one_header <filename>
#
# Function used to implement the logic of checking a single header.
# Returns 0 on success (as a normal bash function would).
# Ideally shouldn't print anything.
function check_one_header
{
    include_file=$1
    grep -q '#ifdef __cplusplus' ${include_file}
}

# get_include_dir_list
#
# Get the list of public include directories from compile_commands.json.
# Stores the result in INCLUDE_DIRS variable.
function get_include_dirs_list
{
    pushd $IDF_PATH/examples/get-started/blink
    idf.py reconfigure

    # Get the compiler command line from compile_commands.json
    # Poor man's replacement of `jq` in Python.
    extract_build_command_py='import json; print(json.load(open("build/compile_commands.json", "r"))[0]["command"])'
    build_command=$(python -c "${extract_build_command_py}")

    # build the list of include directories, only considering the directories in "components/"
    include_dirs=""
    for token in ${build_command}; do
        if [[ ! "${token}" =~ ^-I.*/components/.* ]]; then
            continue
        fi
        newline=$'\n'
        include_dirs="${include_dirs}${token##-I}${newline}"
    done

    export INCLUDE_DIRS=${include_dirs}
}

# check_include_dirs <include_dirs> <failed_headers_list_file>
#
# Check all header files in directories listed in include_dirs variable.
# Places the list of header files with some issues into <failed_headers_list_file> file.
# Calls check_one_header function to check a single header file.
function check_include_dirs
{
    local include_dirs="$1"
    local failed_headers_list_file="$2"

    rm -f ${failed_headers_list_file}
    touch ${failed_headers_list_file}

    for include_dir in ${include_dirs}; do
        echo "Processing include directory: ${include_dir}"
        include_files=$(find ${include_dir} -name '*.h')
        for include_file in ${include_files}; do
            printf "    Processing ${include_file}"
            if ! check_one_header ${include_file}; then
                printf " FAIL!\n"
                echo ${include_file##${IDF_PATH}/} >> ${failed_headers_list_file}
            else
                printf " OK\n"
            fi
        done
    done
}

# filter_exclusions <input_list_file> <exclusions_list_file> <filtered_list_file>
#
# Takes the list of files in <input_list_file> file, and removes all files which
# are listed in <exclusions_list_file> file.
# Saves the final (filtered) list in <filtered_list_file> file.
# The exclusions list file may contain comments and empty lines.
function filter_exclusions
{
    local input_list_file=$1
    local exclusions_list_file=$2
    local filtered_list_file=$3
    local temp_list=$(mktemp)

    cp ${input_list_file} ${filtered_list_file}
    touch ${temp_list}

    while read -r line; do
        if [[ "${line}" =~ ^\# || -z "${line}" ]]; then
            continue
        fi
        echo "Excluding ${line}"
        grep -vE "${line}" ${filtered_list_file} > ${temp_list} || true
        mv ${temp_list} ${filtered_list_file}
    done < ${exclusions_list_file}

    rm -f ${temp_list}
}

##################### main logic starts here #####################

EXCLUSIONS_LIST=${IDF_PATH}/tools/ci/check_cpp_guards_exceptions.txt
MISSING_GUARDS_LIST=missing.txt
FILTERED_LIST=filtered.txt
LINE=---------------------------------------------------------------


get_include_dirs_list
check_include_dirs "${INCLUDE_DIRS}" ${MISSING_GUARDS_LIST}
filter_exclusions ${MISSING_GUARDS_LIST} ${EXCLUSIONS_LIST} ${FILTERED_LIST}

if [ -s ${FILTERED_LIST} ]; then
    echo "The following files don't have C++ guards:"
    echo ""
    cat ${FILTERED_LIST}
    exit 1
fi

echo "No errors found"
