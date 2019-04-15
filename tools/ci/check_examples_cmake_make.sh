#!/bin/bash

# While we support GNU Make & CMake together, check the same examples are present for both

CMAKE_EXAMPLE_PATHS=$( find ${IDF_PATH}/examples/ -type f -name CMakeLists.txt | grep -v "/components/" | grep -v "/common_components/" | grep -v "/main/" | grep -v "/build_system/cmake/")
MAKE_EXAMPLE_PATHS=$( find ${IDF_PATH}/examples/ -type f -name Makefile | grep -v "/build_system/cmake/")

CMAKE_EXAMPLE_PATHS="$(/usr/bin/dirname $CMAKE_EXAMPLE_PATHS | sort -n)"
MAKE_EXAMPLE_PATHS="$(/usr/bin/dirname $MAKE_EXAMPLE_PATHS | sort -n)"

MISMATCH=$(comm -3 <(echo "$MAKE_EXAMPLE_PATHS") <(echo "$CMAKE_EXAMPLE_PATHS"))

if [ -n "$MISMATCH" ]; then
    echo "Some examples are not in both CMake and GNU Make:"
    echo "$MISMATCH"
    exit 1
fi

echo "Example lists match"
exit 0
