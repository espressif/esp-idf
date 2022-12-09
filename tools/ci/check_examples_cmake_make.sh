#!/usr/bin/env bash

# While we support GNU Make & CMake together, check the same examples are present for both. But only for ESP32


echo "- Getting paths of CMakeLists and Makefiles"
IFS=
CMAKELISTS=$( find ${IDF_PATH}/examples/ -type f -name CMakeLists.txt | grep -v "/components/" | grep -v "/common_components/" | grep -v "/cxx/experimental/experimental_cpp_component/" | grep -v "/main/" | grep -v "/build_system/cmake/" | grep -v "/mb_example_common/" | sort -n)
MAKEFILES=$( find ${IDF_PATH}/examples/ -type f -name Makefile | grep -v "/build_system/cmake/" | sort -n)
echo "  [DONE]"


echo "- Building the ignore list"
IGNORE_LIST=
while read line
do
    STL=$(grep "set[(]SUPPORTED_TARGETS" $line)
    if test -n $STL # if specified SUPPORTED_TARGETS
    then
        WO_ESP32=$(grep -v -w "esp32" <<< $STL)
        if test -n "$WO_ESP32"  # if not specified esp32 at SUPPORTED_TARGETS
        then                    # then consider that the example should not have a Makefile
            PATH2IGNORE=$(/usr/bin/dirname $line)
            echo "  Adding to ignore $PATH2IGNORE"
            IGNORE_LIST="$IGNORE_LIST$PATH2IGNORE
"
        fi
    fi
done <<< $CMAKELISTS
IGNORE_LIST=$(grep -v -e '^$' <<< $IGNORE_LIST) # remove empty lines
echo -e "  [DONE] - Ignore list:\n$IGNORE_LIST\n"


echo "- Applying the Ignore list"
while read line
do
    echo $line
    CMAKELISTS=$(echo $CMAKELISTS | grep -v $line)
    MAKEFILES=$(echo $MAKEFILES | grep -v $line)
done <<< $IGNORE_LIST
echo "  [DONE]"


echo "- Getting paths of examples"
while read line
do
    new_path="$(/usr/bin/dirname $line)"
    CMAKE_EXAMPLE_PATHS="$CMAKE_EXAMPLE_PATHS$new_path
"
done <<< $CMAKELISTS
CMAKE_EXAMPLE_PATHS=$(grep -v -e '^$' <<< $CMAKE_EXAMPLE_PATHS) # remove empty lines

while read line
do
    new_path="$(/usr/bin/dirname $line)"
    MAKE_EXAMPLE_PATHS="$MAKE_EXAMPLE_PATHS$new_path
"
done <<< $MAKEFILES
MAKE_EXAMPLE_PATHS=$(grep -v -e '^$' <<< $MAKE_EXAMPLE_PATHS) # remove empty lines
echo "  [DONE]"

echo "- Analysing matches"
MISMATCH=$(comm -3 <(echo "$MAKE_EXAMPLE_PATHS") <(echo "$CMAKE_EXAMPLE_PATHS"))
if [ -n "$MISMATCH" ]; then
    echo "  [ERROR] Some examples are not in both CMake and GNU Make:"
    echo "$MISMATCH"
    exit 1
fi

echo "  [DONE] Example lists match"
exit 0
