# A CMake script to run esptool commands from within ninja or make
# or another cmake-based build runner
#
# (Needed to expand environment variables, for backwards compatibility.)
#
# It is recommended to NOT USE this CMake script if you have the option of
# running esptool.py directly. This script exists only for use inside CMake builds.
#
cmake_minimum_required(VERSION 3.5)

set(TOOL "esptool.py")

if(NOT ESPTOOLPY OR NOT ESPTOOL_ARGS)
    message(FATAL_ERROR "ESPTOOLPY and ESPTOOL_ARGS must "
        "be specified on the CMake command line. For direct execution, it is "
        "strongly recommended to run ${TOOL} directly.")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/get_port_args.cmake")
set(CMD "${ESPTOOLPY} ${port_arg} ${baud_arg} ${ESPTOOL_ARGS}")
include("${CMAKE_CURRENT_LIST_DIR}/run_cmd.cmake")