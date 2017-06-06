#!/bin/bash
#
# Build all examples from the examples directory, out of tree to
# ensure they can run when copied to a new directory.
#
# Runs as part of CI process.
#
# Assumes CWD is an out-of-tree build directory, and will copy examples to individual subdirectories, one by one.
#
[ -z ${IDF_PATH} ] && echo "IDF_PATH is not set" && exit 1

export BATCH_BUILD=1
export V=0 # only build verbose if there's an error

EXAMPLE_NUM=1
RESULT=0
FAILED_EXAMPLES=""

RESULT_WARNINGS=22  # magic number result code for "warnings found"
LOG_WARNINGS=$(mktemp -t example_all.XXXX.log)

build_example () {
    local MAKE_FILE=$1
    shift

    local EXAMPLE_DIR=$(dirname "${MAKE_FILE}")
    local EXAMPLE_NAME=$(basename "${EXAMPLE_DIR}")

    echo "Building ${EXAMPLE_NAME} as ${EXAMPLE_NUM}..."
    mkdir -p "example_builds/${EXAMPLE_NUM}"
    cp -r "${EXAMPLE_DIR}" "example_builds/${EXAMPLE_NUM}"
    pushd "example_builds/${EXAMPLE_NUM}/${EXAMPLE_NAME}"
        # be stricter in the CI build than the default IDF settings
        export EXTRA_CFLAGS="-Werror -Werror=deprecated-declarations"
        export EXTRA_CXXFLAGS=${EXTRA_CFLAGS}

        # build non-verbose first
        local BUILDLOG=$(mktemp -t examplebuild.XXXX.log)
        (
            set -o pipefail  # so result of make all isn't lost when piping to tee
            set -e
            make clean defconfig
            make $* all 2>&1 | tee "${BUILDLOG}"
        ) || { RESULT=$?; FAILED_EXAMPLES+=" ${EXAMPLE_NAME}"; make V=1; } # verbose output for errors
    popd

    EXAMPLE_NUM=$(( $EXAMPLE_NUM + 1 ))

    if grep ": warning:" "${BUILDLOG}" 2>&1 >> "${LOG_WARNINGS}"; then
        [ $RESULT -eq 0 ] && RESULT=$RESULT_WARNINGS
        FAILED_EXAMPLES+=" ${EXAMPLE_NAME} (warnings)"
    fi

    grep -i error "${BUILDLOG}" 2>&1 >> "${LOG_WARNINGS}"

    rm -f "${BUILDLOG}"
}

find ${IDF_PATH}/examples/ -type f -name Makefile | \
while read fn
do
    build_example "$fn" $*
done

# show warnings
echo -e "\nFound issues:"
# pattern is: not 'error.o' and not '-Werror'
grep -v "error.o\|\-Werror" -- "${LOG_WARNINGS}"
rm -f "${LOG_WARNINGS}"

if [ $RESULT -eq $RESULT_WARNINGS ]; then
    echo "Build would have passed, except for warnings."
fi

[ $RESULT -eq 0 ] || echo "Failed examples: $FAILED_EXAMPLES"

echo -e "\nResult = $RESULT"

exit $RESULT
