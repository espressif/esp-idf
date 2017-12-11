#!/bin/bash
#
# Build all examples from the examples directory, out of tree to
# ensure they can run when copied to a new directory.
#
# Runs as part of CI process.
#
# Assumes PWD is an out-of-tree build directory, and will copy examples
# to individual subdirectories, one by one.
#
#
# Without arguments it just builds all examples
#
# With one argument <JOB_NAME> it builds part of the examples. This is a useful for
#   parallel execution in CI.
#   <JOB_NAME> must look like this:
#               <some_text_label>_<num>
#   It scans .gitlab-ci.yaml to count number of jobs which have name "<some_text_label>_<num>"
#   It scans the filesystem to count all examples
#   Based on this, it decides to run qa set of examples.
#

# -----------------------------------------------------------------------------
# Safety settings (see https://gist.github.com/ilg-ul/383869cbb01f61a51c4d).

if [[ ! -z ${DEBUG} ]]
then
  set -x # Activate the expand mode if DEBUG is anything but empty.
fi

set -o errexit # Exit if command failed.
set -o pipefail # Exit if pipe failed.
set -o nounset # Exit if variable not set.

# Remove the initial space and instead use '\n'.
IFS=$'\n\t'

# -----------------------------------------------------------------------------

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit 1
}

[ -z ${IDF_PATH} ] && die "IDF_PATH is not set"

echo "build_examples running in ${PWD}"

# only 0 or 1 arguments
[ $# -le 1 ] || die "Have to run as $(basename $0) [<JOB_NAME>]"

export BATCH_BUILD=1
export V=0 # only build verbose if there's an error

shopt -s lastpipe # Workaround for Bash to use variables in loops (http://mywiki.wooledge.org/BashFAQ/024)
RESULT=0
FAILED_EXAMPLES=""

RESULT_WARNINGS=22  # magic number result code for "warnings found"
LOG_WARNINGS=${PWD}/build_warnings.log

if [ $# -eq 0 ]
then
    START_NUM=0
    END_NUM=999
else
    JOB_NAME=$1

    # parse text prefix at the beginning of string 'some_your_text_NUM'
    # (will be 'some_your_text' without last '_')
    JOB_PATTERN=$( echo ${JOB_NAME} | sed -n -r 's/^(.*)_[0-9]+$/\1/p' )
    [ -z ${JOB_PATTERN} ] && die "JOB_PATTERN is bad"

    # parse number 'NUM' at the end of string 'some_your_text_NUM'
    JOB_NUM=$( echo ${JOB_NAME} | sed -n -r 's/^.*_([0-9]+)$/\1/p' )
    [ -z ${JOB_NUM} ] && die "JOB_NUM is bad"

    # count number of the jobs
    NUM_OF_JOBS=$( grep -c -E "^${JOB_PATTERN}_[0-9]+:$" "${IDF_PATH}/.gitlab-ci.yml" )
    [ -z ${NUM_OF_JOBS} ] && die "NUM_OF_JOBS is bad"

    # count number of examples
    NUM_OF_EXAMPLES=$( find ${IDF_PATH}/examples/ -type f -name Makefile | wc -l )
    [ -z ${NUM_OF_EXAMPLES} ] && die "NUM_OF_EXAMPLES is bad"

    # separate intervals
    #57 / 5 == 12
    NUM_OF_EX_PER_JOB=$(( (${NUM_OF_EXAMPLES} + ${NUM_OF_JOBS} - 1) / ${NUM_OF_JOBS} ))
    [ -z ${NUM_OF_EX_PER_JOB} ] && die "NUM_OF_EX_PER_JOB is bad"

    # ex.: [0; 12); [12; 24); [24; 36); [36; 48); [48; 60)
    START_NUM=$(( ${JOB_NUM} * ${NUM_OF_EX_PER_JOB} ))
    [ -z ${START_NUM} ] && die "START_NUM is bad"

    END_NUM=$(( (${JOB_NUM} + 1) * ${NUM_OF_EX_PER_JOB} ))
    [ -z ${END_NUM} ] && die "END_NUM is bad"
fi

build_example () {
    local ID=$1
    shift
    local MAKE_FILE=$1
    shift

    local EXAMPLE_DIR=$(dirname "${MAKE_FILE}")
    local EXAMPLE_NAME=$(basename "${EXAMPLE_DIR}")

    echo "Building ${EXAMPLE_NAME} as ${ID}..."
    mkdir -p "example_builds/${ID}"
    cp -r "${EXAMPLE_DIR}" "example_builds/${ID}"
    pushd "example_builds/${ID}/${EXAMPLE_NAME}"
        # be stricter in the CI build than the default IDF settings
        export EXTRA_CFLAGS="-Werror -Werror=deprecated-declarations"
        export EXTRA_CXXFLAGS=${EXTRA_CFLAGS}

        # build non-verbose first
        local BUILDLOG=${PWD}/examplebuild.${ID}.log
        (
            make MAKEFLAGS= clean &&
            make MAKEFLAGS= defconfig &&
            make all &&
            make print_flash_cmd | tail -n 1 > build/download.config
        ) &> >(tee -a "${BUILDLOG}") || {
            RESULT=$?; FAILED_EXAMPLES+=" ${EXAMPLE_NAME}"
            make MAKEFLAGS= V=1 clean defconfig && make V=1 # verbose output for errors
        }
    popd

    if grep ": warning:" "${BUILDLOG}" 2>&1 >> "${LOG_WARNINGS}"; then
        [ $RESULT -eq 0 ] && RESULT=$RESULT_WARNINGS
        FAILED_EXAMPLES+=" ${EXAMPLE_NAME} (warnings)"
    fi

    grep -i error "${BUILDLOG}" 2>&1 >> "${LOG_WARNINGS}" || :

    rm -f "${BUILDLOG}"
}

EXAMPLE_NUM=0

find ${IDF_PATH}/examples/ -type f -name Makefile | sort | \
while read FN
do
    if [[ $EXAMPLE_NUM -lt $START_NUM || $EXAMPLE_NUM -ge $END_NUM ]]
    then
        EXAMPLE_NUM=$(( $EXAMPLE_NUM + 1 ))
        continue
    fi
    echo ">>> example [ ${EXAMPLE_NUM} ] - $FN"

    build_example "${EXAMPLE_NUM}" "${FN}"

    EXAMPLE_NUM=$(( $EXAMPLE_NUM + 1 ))
done

# show warnings
echo -e "\nFound issues:"
# pattern is: not 'error.o' and not '-Werror'
grep -v "error.o\|\-Werror" -- "${LOG_WARNINGS}" || echo -e "\tNone"
rm -f "${LOG_WARNINGS}"

if [ $RESULT -eq $RESULT_WARNINGS ]; then
    echo "Build would have passed, except for warnings."
fi

[ $RESULT -eq 0 ] || echo "Failed examples: $FAILED_EXAMPLES"

echo -e "\nReturn code = $RESULT"

exit $RESULT
