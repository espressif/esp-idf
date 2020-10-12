# A CMake script to run esptool commands from within ninja or make
# or another cmake-based build runner
#
# (Needed to expand environment variables, for backwards compatibility.)
#
# It is recommended to NOT USE this CMake script if you have the option of
# running esptool.py directly. This script exists only for use inside CMake builds.
#
cmake_minimum_required(VERSION 3.5)

if(NOT SERIAL_TOOL OR NOT SERIAL_TOOL_ARGS)
    message(FATAL_ERROR "SERIAL_TOOL and SERIAL_TOOL_ARGS must "
        "be specified on the CMake command line. For direct execution, it is "
        "strongly recommended to run ${SERIAL_TOOL} directly.")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/get_port_args.cmake")
set(CMD "${SERIAL_TOOL} ${port_arg} ${baud_arg} ${SERIAL_TOOL_ARGS}")
include("${CMAKE_CURRENT_LIST_DIR}/run_cmd.cmake")
