# A CMake script to run size tool commands supporting OUTPUT_FORMAT and
# OUTPUT_JSON environment variables from within ninja or make or another
# cmake-based build runner.
#
# It is recommended to NOT USE this CMake script if you have the option of
# running the tool directly. This script exists only for use inside CMake builds.
cmake_minimum_required(VERSION 3.16)

# Main purpose of this script: we can't expand these environment variables in the main IDF CMake build,
# because we want to expand them at CMake target build time not at CMake configuration time
# (so they can change without needing a CMake re-run)

set(IDF_SIZE_CMD ${IDF_SIZE_TOOL})

if(DEFINED ENV{SIZE_OUTPUT_FORMAT})
    list(APPEND IDF_SIZE_CMD "--format=$ENV{SIZE_OUTPUT_FORMAT}")
elseif(DEFINED OUTPUT_JSON AND OUTPUT_JSON)
    list(APPEND IDF_SIZE_CMD "--format=json")
endif()

if(DEFINED IDF_SIZE_MODE)
    list(APPEND IDF_SIZE_CMD ${IDF_SIZE_MODE})
endif()

list(APPEND IDF_SIZE_CMD ${MAP_FILE})

execute_process(COMMAND ${IDF_SIZE_CMD}
    RESULT_VARIABLE result
    )

if(${result})
    message(FATAL_ERROR "${IDF_SIZE_TOOL} failed")
endif()
