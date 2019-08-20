# A CMake script to run idf_monitor from within ninja or make
# or another cmake-based build runner
#
# (Needed to expand environment variables, for backwards compatibility.)
#
# It is recommended to NOT USE this CMake script if you have the option of
# running idf_monitor.py directly. This script exists only for use inside CMake builds.
#
cmake_minimum_required(VERSION 3.5)

set(TOOL "idf_monitor.py")

if(NOT IDF_MONITOR OR NOT ELF_FILE)
    message(FATAL_ERROR "IDF_MONITOR and ELF_FILE must "
        "be specified on the CMake command line. For direct execution, it is "
        "strongly recommended to run ${TOOL} directly.")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/get_port_args.cmake")
set(CMD "${IDF_MONITOR} ${port_arg} ${baud_arg} ${ELF_FILE}")
include("${CMAKE_CURRENT_LIST_DIR}/run_cmd.cmake")
