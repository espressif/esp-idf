#!/usr/bin/env bash
set -o errexit
set -o pipefail
set -o nounset

DEBUG_SHELL=${DEBUG_SHELL:-"0"}
[ "${DEBUG_SHELL}" = "1" ] && set -x

if [[ $# < 1 ]]; then
    echo "no target specified!" >&2
    exit -1
fi

cd $IDF_PATH/examples
ALL_EXAMPLES=$( find . -type f -name CMakeLists.txt | grep -v "/components/" | grep -v "/common_components/" | grep -v "/main/" | grep -v "/idf_as_lib/stubs/" | sed "s/\/CMakeLists.txt//g" | sort )
EXAMPLE_LIST=$( realpath --relative-to=. $ALL_EXAMPLES )

for EXAMPLE in $EXAMPLE_LIST
do
    SEARCHED=$( grep -E "SUPPORTED_TARGETS" $EXAMPLE/CMakeLists.txt | sed  "s/set\s*(\s*SUPPORTED_TARGETS//g" | sed "s/)//g" ) || true
    if [[ $SEARCHED == "" ]]; then
        #when SUPPORTED_TARGETS not set, allow all targets implicitly
        echo "$EXAMPLE"
    else
        for TARGET in $SEARCHED
        do
            if [[ $TARGET == $1 ]]; then
                echo "$EXAMPLE"
                break
            fi
        done
    fi
done
