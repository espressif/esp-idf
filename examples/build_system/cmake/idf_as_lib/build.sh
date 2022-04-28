#!/usr/bin/env bash

set -euo pipefail
PARAM=""

# Retrive the target from the current filename, if no target specified,
# the variable will be empty
TARGET=$(echo $0 | cut -s -f2 -d- | cut -s -f1 -d.)
if [[ -n $TARGET ]]
then
    # Target is not null, specify the build parameters
    PARAM="-DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-${TARGET}.cmake -DTARGET=${TARGET} -GNinja"
fi

rm -rf build && mkdir build && cd build
cmake .. $PARAM
cmake --build .
