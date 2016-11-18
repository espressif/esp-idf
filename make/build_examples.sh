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

EXAMPLE_NUM=1
RESULT=0

set -e

for example in ${IDF_PATH}/examples/*; do
    [ -f ${example}/Makefile ] || continue
    echo "Building ${example} as ${EXAMPLE_NUM}..."
    mkdir ${EXAMPLE_NUM}
    cp -r ${example} ${EXAMPLE_NUM}
    pushd ${EXAMPLE_NUM}/`basename ${example}`

   # be stricter in the CI build than the default IDF settings
   export EXTRA_CFLAGS="-Werror -Werror=deprecated-declarations"
   export EXTRA_CXXFLAGS=${EXTRA_CFLAGS}

   # build non-verbose first, only build verbose if there's an error
    (make clean defconfig && make all ) || (RESULT=$?; make V=1)
    popd
    EXAMPLE_NUM=$(( $EXAMPLE_NUM + 1 ))
done

exit $RESULT

