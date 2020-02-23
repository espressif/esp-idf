#!/bin/bash
#
# This script finds all *.h files in public include dirs and checks for
# * compilation issues
# * c++ header guards
# * need for sdkconfig.h inclusion (if CONFIG_ macros referenced)
#
# Script arguments:
# --verbose: enables printing details if compilation fails (verbose is off by default)
# --process-all-files: all files are checked and then excluded, by default the excluded files are skipped and not checked
# --include-maxdepth=X: maximum directory depth for finding include files in include dirs (by default not specified=infinity)
# --jobs=X: run the checker in X separate processes

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
    if [ ! -z "${VERBOSE:-}" ]; then
        file=$1
        echo " verbose..."
        echo "$LINE"
        grep -A5 "error:"
        echo "$LINE"
        echo -n "$file"
    fi
}

# log_details  <log_data>
#
# Logs details as function argument to stdout
function log_details()
{
    if [ ! -z "${VERBOSE:-}" ]; then
        printf "... $1 "
    fi
}


# preproc_one_header <filename>
#
# Function to preprocess a header with CC and CXX and compares result
# Returns
# 0 -> Never returns zero (for easier return value expansion)
# 1 -> Cannot compile and failed with injected SDKCONFIG #error (header FAILs)
# 2 -> Cannot compile, but failed with "#error" directive (header seems OK)
# 3 -> Cannot compile with another issue, logged if verbose (header FAILs)
# 4 -> Both preprocessors produce zero out (header file is OK)
# 5 -> Both preprocessors produce the same, non-zero output (header file FAILs)
# 6 -> Both preprocessors produce different, non-zero output with extern "C" (header seems OK)
# 7 -> Both preprocessors produce different, non-zero output without extern "C" (header fails)
function preproc_one_header() 
{
    local extra_flags=""
    local file=$1
    # check if the header (after removing comments) references CONFIG_XXX macros
    if ${CC} -fpreprocessed -dD -P -E  ${file} ${FLAGS_INCLUDE_DIRS} 2>/dev/null | grep -q -wo -E 'CONFIG_[A-Z0-9_]+'; then
        # enable sdkconfig.h inclusion checker
        extra_flags="-DIDF_CHECK_SDKCONFIG_INCLUDED"
    fi
    if ! cpp_out=`${CXX} -w -P -E --std=c++17 -include ${file} ${extra_flags} ${TEST_MAIN_C} ${FLAGS_INCLUDE_DIRS} 2>&1`; then
        if [[ "$cpp_out" =~ "#error" ]]; then
            if [[ "$cpp_out" =~ "#error CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED" ]]; then
                # failed for sdkconfig.h issue
                return 1
            fi;
            # header passed in general (error directive)
            return 2
        fi;
        # Logging the compilation issue
        echo "$cpp_out" | log_error $file
        return 3
    fi;
    if ! c99_out=`${CC} -w -P -E --std=c99 -include ${file} ${extra_flags} ${TEST_MAIN_C} ${FLAGS_INCLUDE_DIRS} 2>/dev/null`; then
        # Should not happen, fail with compilation issue
        return 3
    fi;
    if diff_out=`diff  <(echo "$cpp_out" ) <(echo "$c99_out")`; then
        # checks zero output: contains spaces only if lines < 5 (for performace)
        lines=`echo "$cpp_out" | wc -l`
        if [ $lines -lt 5 ] && [ -z "${cpp_out// }" ]; then 
            return 4
        fi
        return 5
    elif echo $diff_out | grep -q 'extern "C"'; then
        return 6
    fi
    return 7
}

# check_one_header <filename>
#
# Function used to implement the logic of checking a single header.
# Returns 0 on success (as a normal bash function would).
# - returns 1 if suspects C++ guard missing
# - returns 2 if compile issue (other then error directive)
# - returns 3 if CONFIG_XXX variable used but sdkconfig.h not included
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
    1*)    log_details "sdkconfig issue";            return 3 ;;
    2*)    log_details "compile failed with #error"; return 0 ;;
    3*)    log_details "another compile issue";      return 2 ;;
    4*)    log_details "zero output";                return 0 ;;
    5*)    log_details "the same non-zero out";      return 1 ;;
    [6-7]*)      log_details "different and non-zero: check again without includes";
        # Removing includes to check if the header itself contains a code
        temp=$(mktemp)
        sed '/#include/d; /#error/d' ${include_file} > ${temp}
        case `preproc_one_header ${temp} || echo $?` in
        [1-3]*) log_details "ok, compile issue";                          return 0 ;;
        4*)     log_details "ok, zero output";                            return 0 ;;
        5*)     log_details "the same non-zero out";                      return 1 ;;
        6*)     log_details "different, non-zero, with extern \"C\"";     return 0 ;;
        7*)     log_details "different, non-zero, no extern \"C\"";       return 1 ;;
        esac
        rm -f ${temp}
    esac
}

# create_test_main <dir>
#
# Generates a compilation unit to fail if sdkconfig.h inclusion to be checked
# Exports the full path to the file as TEST_MAIN_C
function create_test_main
{
    echo "#if defined(IDF_CHECK_SDKCONFIG_INCLUDED) && ! defined(IDF_SDKCONFIG_INCLUDED)
    #error CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED
    #endif" > ${1}/compile.c
    export TEST_MAIN_C=${1}/compile.c
}

# get_include_dir_list <project_dir>
#
# Get the list of public include directories from compile_commands.json.
# Stores the result in INCLUDE_DIRS variable.
function get_include_dirs_list
{
    local proj_dir=${1}
    local sdkinclude_dir=${proj_dir}/build/config
    pushd $proj_dir
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

    # Append a config value used to check if the sdkconfig included
    echo "#define IDF_SDKCONFIG_INCLUDED" >> ${sdkinclude_dir}/sdkconfig.h
    export INCLUDE_DIRS=${include_dirs}
    flags_include_dirs+=" -I ${sdkinclude_dir}"
    export FLAGS_INCLUDE_DIRS=${flags_include_dirs}
}

# Utility convertion functions used for passing load sequences to processes (in form of one character)
#
# Returns ordinal number of supplied character
ord() { LC_CTYPE=C printf %d "'$1"; }
# Returns a character represented by ASCII code
chr() { printf "\\$(printf "%o" $1)"; }

# check_headers_in_process <process_index> <list_of_includes> <list_of_failed_includes>
#
# Checks one include files at a time in a batch of headers referenced by indices from stdin
# This function may be called from separate processes (used simple load balancing)
# Periodically checks files from <list_of_includes>
# Outputs failed files to  <list_of_failed_includes>
function check_headers_in_process
{
    local proc="$1"
    local list_of_includes="$2"
    local list_of_failed_includes="$3"

    list=(`cat ${list_of_includes}`)
    size=${#list[@]}
    let batch=$size/$JOBS_LOAD_CHAR_SIZE+1

    # Reads one char (due to atomicity) from stdin
    # -ord(char) signifies an index to the list of files to be processed
    while IFS= read -n1 char; do
        batch_index=$(ord $char)
        let run=$batch_index-$JOBS_LOAD_CHAR_FROM || true
        for i in `seq 1 $batch`; do
            let file_index=$run*$batch+$i-1 || true
            if [ "$file_index" -lt "$size" ]; then
	            # log_details "$proc: Processing $file_index (batch=$batch, run=$run)\n"
                include_file=${list[$file_index]}
                rel_inc_file=${include_file##${IDF_PATH}}
                printf "Processing ${rel_inc_file}"
                check_one_header ${include_file} && ret=0 || ret=$?
                case "$ret" in
                0*) printf " OK\n" ;;
                1*) printf " FAIL (missing c++ guard)\n" ;;
                2*) printf " FAIL (cannot compile)\n" ;;
                3*) printf " FAIL (missing include of sdkconfig.h)\n" ;;
                esac
                if [[ ! "$ret" == 0 ]]; then
                    echo ${rel_inc_file} >> ${list_of_failed_includes}
                fi
            fi
        done
    done < $WORK_ITEM_PIPE;
}

# check_include_dirs <include_dirs> <list_of_includes> <list_of_failed_includes>
#
# Check all header files in directories listed in include_dirs variable.
# Places the list of header files where suspects missing C++ guard to <missing_guard_list_file> file.
# Places the list of header files where compilation issues found to <compile_error_list_file> file.
# Calls check_one_header function to check a single header file.
function check_include_dirs
{
    local include_dirs="$1"
    local list_of_includes="$2"
    local list_of_failed_includes="$3"
    local include_dir_maxdepth=${INCLUDE_DIR_MAXDEPTH:-}
    local grep_expr=$(mktemp)

    rm -f ${list_of_includes} ${list_of_failed_includes}
    touch ${list_of_includes} ${list_of_failed_includes}

    if [ -z "${PROCESS_ALL_INCLUDES:-}" ]; then
        # generate grep expr: 1) strip comments, 2) generate '<exclude1>\|<exclude2>\|...'
        exclusions=`cat $EXCLUSIONS_LIST | sed '/^#/d'`
        echo $exclusions | sed 's/ /\\|/g' > ${grep_expr}
    fi;

    for include_dir in ${include_dirs}; do
        echo "Processing include directory: ${include_dir}"
        include_files=$(find ${include_dir} ${include_dir_maxdepth} -name '*.h')
        for include_file in ${include_files}; do
            if [ -z "${PROCESS_ALL_INCLUDES:-}" ] && echo "$include_file" | grep -q -f ${grep_expr}; then
                continue
            fi
            echo ${include_file} >> ${list_of_includes}
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

EXCLUSIONS_LIST=${IDF_PATH}/tools/ci/check_public_headers_exceptions.txt
FAILED_INCLUDE_LIST=failed.txt
TOTAL_INCLUDE_LIST=include_list.txt
FILTERED_LIST=filtered.txt
LINE=---------------------------------------------------------------
CC=${CC:-gcc}
CXX=${CXX:-g++}
TEST_PROJECT_DIR=${IDF_PATH}/examples/get-started/blink
MULTI_JOBS="1"
# jobs, load balancing vars, temp outputs
JOBS_LOAD_CHAR_FROM=64
JOBS_LOAD_CHAR_TO=90
let JOBS_LOAD_CHAR_SIZE=$JOBS_LOAD_CHAR_TO-$JOBS_LOAD_CHAR_FROM+1
TEMP_OUT_FILE=/tmp/proc_out.txt.
WORK_ITEM_PIPE=/tmp/work_item_pipe

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
    if [[ "$i" =~ --jobs=* ]]; then
        MULTI_JOBS="${i##--jobs=}"
        echo "$0: Checking headers within multiple jobs: $MULTI_JOBS"
    fi
done

[[ -p $WORK_ITEM_PIPE ]] || mkfifo $WORK_ITEM_PIPE

echo $LINE
# Get include directories
get_include_dirs_list ${TEST_PROJECT_DIR}
# Generate the main file to compiled while preincluding a header under test
create_test_main ${TEST_PROJECT_DIR}/build
# Get the list of include files to be processed
check_include_dirs "${INCLUDE_DIRS}" ${TOTAL_INCLUDE_LIST} ${FAILED_INCLUDE_LIST}

# Launching separate processes (if $MULTI_JOBS defined) to check includes
#
# poor man's approach to load balancing:
# 1) sending work items (chars A-Z for atomicity) to a named pipe
# 2) spawning nr of processes to pick the work items from the pipe
# 3) every process reads one char at a time to analyze a batch of includes (batch=total_includes/nr_of_work_items)
# 4) waiting till all processes finish

# running the jobs and marking the pid
for ((i=1; i<${MULTI_JOBS}; i++)); do
    check_headers_in_process $i ${TOTAL_INCLUDE_LIST} ${FAILED_INCLUDE_LIST} > ${TEMP_OUT_FILE}${i} &
    pids[${i}]=$!
done
# one job outputs directly to stdout (to show the script is alive)
check_headers_in_process 0 ${TOTAL_INCLUDE_LIST} ${FAILED_INCLUDE_LIST} &
pid0=$!

# sending sequence of work items
for i in `seq $JOBS_LOAD_CHAR_FROM $JOBS_LOAD_CHAR_TO`; do
    printf $(chr $i )
done > $WORK_ITEM_PIPE

# waiting for the processes to finish and print their outputs
for ((i=1; i<${MULTI_JOBS}; i++)); do
    log_details "\nwaiting for process nr: $i\n"
    wait ${pids[$i]} || true
    cat ${TEMP_OUT_FILE}${i}
    rm -f ${TEMP_OUT_FILE}${i}
done
# and the last one
wait $pid0 || true
rm -f $WORK_ITEM_PIPE

# Filter out the known issues and report results
cat ${FAILED_INCLUDE_LIST} | filter_exclusions ${EXCLUSIONS_LIST} ${FILTERED_LIST}
echo $LINE

if [ -s ${FILTERED_LIST} ]; then
    echo "The following files failed:"
    echo ""
    cat ${FILTERED_LIST}
    exit 1
fi

echo "No errors found"
