#!/usr/bin/env bash

EXCLUDE_UT_LIST_FILE="$( dirname -- "$0" )/exclude_check_ut_cmake_make_list.txt"

# While we support GNU Make & CMake together, check that unit tests support both
CMAKE_UT_PATHS=$( find ${IDF_PATH}/components/ -type f -name CMakeLists.txt | grep "/test/" | grep -v -F -f ${EXCLUDE_UT_LIST_FILE} )
MAKE_UT_PATHS=$( find ${IDF_PATH}/components/ -type f -name component.mk | grep "/test/" | grep -v -F -f ${EXCLUDE_UT_LIST_FILE} )

CMAKE_UT_PATHS="$(/usr/bin/dirname $CMAKE_UT_PATHS | sort -n)"
MAKE_UT_PATHS="$(/usr/bin/dirname $MAKE_UT_PATHS | sort -n)"

MISMATCH=$(comm -3 <(echo "$MAKE_UT_PATHS") <(echo "$CMAKE_UT_PATHS"))

if [ -n "$MISMATCH" ]; then
    echo "Some unit tests are not in both CMake and GNU Make:"
    echo "$MISMATCH"
    exit 1
fi

echo "Unit tests match"
exit 0
