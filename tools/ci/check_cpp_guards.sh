#!/bin/bash
#
# This script finds all *.h files in public include dirs and checks for
# * compilation issues
# * c++ header guards
#
# The script fails if another a new header fails and not listed in check_cpp_guards_exceptions.txt
#
# Script arguments:
# --verbose: enables printing details if compilation fails (verbose is off by default)
# --process-all-files: all files are checked and then excluded, by default the excluded files are skipped and not checked
# --include-maxdepth=X: maximum directory depth for finding include files in include dirs (by default not specified=infinity)

set -euo pipefail

if [ -z "${IDF_PATH:-}" ]; then
    echo "Please run export.sh before invoking this script"
    exit 1
fi

##################### helper functions #####################

# log_error  <filename>
#
# Logs details on filename processing when the include fails
function log_error()
{
    if [ ! -z "$VERBOSE" ]; then
        file=$1
        echo " verbose..."
        echo "$LINE"
        grep -A5 "error:"
        echo "$LINE"
        echo -n "$file"
    fi
}

# preproc_one_header <filename>
#
# Function to preprocess a header with CC and CXX and compares result
# Returns
# 0 -> Never returns zero (for easier return value expansion)
# 1 -> Cannot compile, but failed with "#error" directive (header seems OK)
# 2 -> Cannot compile with another issue, logged if verbose (header FAILs)
# 3 -> Both preprocessors produce zero out (header file is OK)
# 4 -> Both preprocessors produce the same, non-zero output (header file FAILs)
# 5 -> Both preprocessors produce different, non-zero output with extern "C" (header seems OK)
# 6 -> Both preprocessors produce different, non-zero output without extern "C" (header seems OK)
function preproc_one_header() 
{
    file=$1
    if ! cpp_out=`xtensa-esp32-elf-g++ -w -P -E --std=c++17 ${file} ${FLAGS_INCLUDE_DIRS} 2>&1`; then
        if [[ "$cpp_out" =~ "#error" ]]; then
            return 1
        fi;
        # Logging the compilation issue issue
        echo "$cpp_out" | log_error $file
        return 2
    fi;
    if ! c99_out=`xtensa-esp32-elf-gcc -w -P -E --std=c99 ${file}  ${FLAGS_INCLUDE_DIRS} 2>/dev/null`; then
        # Should not happen, fail with compilation issue
        return 2
    fi;
    if diff_out=`diff  <(echo "$cpp_out" ) <(echo "$c99_out")`; then
        # checks zero output: contains spaces only if lines < 5 (for performace)
        lines=`echo "$cpp_out" | wc -l`
        if [ $lines -lt 5 ] && [ -z "${cpp_out// }" ]; then 
            return 3
        fi
        return 4
    elif echo $diff_out | grep -q 'extern "C"'; then
        return 5
    fi
    return 6
}

# check_one_header <filename>
#
# Function used to implement the logic of checking a single header.
# Returns 0 on success (as a normal bash function would).
# Ideally shouldn't print anything (if verbose, preproc_one_header outputs compilation errors)
function check_one_header
{
    include_file=$1
    # Procedure:
    # 1) Preprocess the include file with C preprocessor and with CPP preprocessor
    #   - Pass the test if the preprocessor outputs are the same and whitespaces only (#define only header)
    #   - Fail the test if the preprocessor outputs are the same (but with some code)
    #   - If outputs different, continue with 2)
    # 2) Strip out all include directives to generate "temp.h"
    # 3) Preprocess the temp.h the same way in (1)
    #   - Pass the test if the preprocessor outputs are the same and whitespaces only (#include only header)
    #   - Fail the test if the preprocessor outputs are the same (but with some code)
    #   - If outputs different, pass the test
    #
    preproc_one_header ${include_file} || ret=$?
    case $ret in
    1*)         return 0 ;; # Pass if #error
    2*)         return 2 ;; # Fail if other compile issue
    3*)         return 0 ;; # Pass if zero out
    4*)         return 1 ;; # Fail if the same non-zero out
    [5-6]*)         # Process further if different and non-zero
        sed '/#include/d; /#error/d' ${include_file} > temp.h
        case `preproc_one_header temp.h || echo $?` in
        [1-3]*) return 0 ;; # Pass if compile issues (have stripped includes)
        4*)     return 1 ;; # Fail if the same non-zero out
        5*)     return 0 ;; # Pass if different and non-zero and with extern "C"
        6*)     return 1 ;; # Fail if different and non-zero, but without extern "C"
        esac
    esac
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
        if [[ "${token}" =~ ^-I.* ]]; then
            flags_include_dirs+=" $token"
        fi
        if [[ ! "${token}" =~ ^-I.*/components/.* ]]; then
            continue
        fi
        newline=$'\n'
        include_dirs="${include_dirs}${token##-I}${newline}"
    done

    export INCLUDE_DIRS=${include_dirs}
    flags_include_dirs+=" -I $IDF_PATH/examples/get-started/blink/build/config"
    export FLAGS_INCLUDE_DIRS=${flags_include_dirs}
}

# check_include_dirs <include_dirs> <missing_guard_list_file> <compile_error_list_file>
#
# Check all header files in directories listed in include_dirs variable.
# Places the list of header files where suspects missing C++ guard to <missing_guard_list_file> file.
# Places the list of header files where compilation issues found to <compile_error_list_file> file.
# Calls check_one_header function to check a single header file.
function check_include_dirs
{
    local include_dirs="$1"
    local missing_guard_list_file="$2"
    local compile_error_list_file="$3"
    local include_dir_maxdepth=${INCLUDE_DIR_MAXDEPTH:-}
    local grep_expr=$(mktemp)

    rm -f ${missing_guard_list_file} ${compile_error_list_file}
    touch ${missing_guard_list_file} ${compile_error_list_file}

    if [ -z "$PROCESS_ALL_INCLUDES" ]; then
        # generate grep expr: 1) strip comments, 2) generate '<exclude1>\|<exclude2>\|...'
        exclusions=`cat $EXCLUSIONS_LIST | sed '/^#/d'`
        echo $exclusions | sed 's/ /\\|/g' > ${grep_expr}
    fi;

    for include_dir in ${include_dirs}; do
        echo "Processing include directory: ${include_dir}"
        include_files=$(find ${include_dir} ${include_dir_maxdepth} -name '*.h')
        for include_file in ${include_files}; do
            if [ -z "$PROCESS_ALL_INCLUDES" ] && echo "$include_file" | grep -q -f ${grep_expr}; then
                printf "    Excluding ${include_file}\n"
                continue
            fi
            printf "    Processing ${include_file}"
            check_one_header ${include_file} && ret=0 || ret=$?
            case "$ret" in
            0*) printf " OK\n" ;;
            1*) 
                echo ${include_file##${IDF_PATH}/} >> ${missing_guard_list_file}
                printf " FAIL (missing c++ guard)\n" ;;
            2*) 
                echo ${include_file##${IDF_PATH}/} >> ${compile_error_list_file}
                printf " FAIL (cannot compile)\n" ;;
            esac
        done
    done
    rm -f ${grep_expr}
}

# filter_exclusions <exclusions_list_file> <filtered_list_file>
#
# Takes the list of files from stdin, and removes all files which
# are listed in <exclusions_list_file> file.
# Saves the final (filtered) list in <filtered_list_file> file.
# The exclusions list file may contain comments and empty lines.
function filter_exclusions
{
    local exclusions_list_file=$1
    local filtered_list_file=$2
    local grep_expr=$(mktemp)

    # generate grep expr: 1) strip comments, 2) generate '<exclude1>\|<exclude2>\|...'
    exclusions=`cat $exclusions_list_file | sed '/^#/d'`
    echo $exclusions | sed 's/ /\\|/g' > ${grep_expr}

    # pass the expression as file (input goes from stdin)
    grep -vf ${grep_expr}  > ${filtered_list_file} || true
    rm -f ${grep_expr}
}

##################### main logic starts here #####################

EXCLUSIONS_LIST=${IDF_PATH}/tools/ci/check_cpp_guards_exceptions.txt
MISSING_GUARDS_LIST=missing.txt
CANNOT_COMPILE_LIST=cannot_compile.txt
FILTERED_LIST=filtered.txt
LINE=---------------------------------------------------------------
VERBOSE=""
PROCESS_ALL_INCLUDES=""

# Process command line arguments
for i in $@; do
    if [[ "$i" =~ "--verbose" ]]; then
        echo "$0: Enabling verbose output"
        VERBOSE="1"
    fi
    if [[ "$i" =~ "--process-all-files" ]]; then
        echo "$0: Processing all files"
        PROCESS_ALL_INCLUDES="1"
    fi
    if [[ "$i" =~ --include-maxdepth=* ]]; then
        INCLUDE_DIR_MAXDEPTH="-maxdepth ${i##--include-maxdepth=}"
        echo "$0: Searching for includes with: $INCLUDE_DIR_MAXDEPTH"
    fi
done

echo $LINE
get_include_dirs_list
check_include_dirs "${INCLUDE_DIRS}" ${MISSING_GUARDS_LIST} ${CANNOT_COMPILE_LIST}
cat ${MISSING_GUARDS_LIST} ${CANNOT_COMPILE_LIST} | filter_exclusions ${EXCLUSIONS_LIST} ${FILTERED_LIST}
echo $LINE

if [ -s ${FILTERED_LIST} ]; then
    echo "The following files failed:"
    echo ""
    cat ${FILTERED_LIST}
    exit 1
fi

echo "No errors found"
