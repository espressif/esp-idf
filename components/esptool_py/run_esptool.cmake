# A CMake script to run esptool commands from within ninja or make
# or another cmake-based build runner
#
# (Needed to expand environment variables, for backwards compatibility.)
#
# It is recommended to NOT USE this CMake script if you have the option of
# running esptool.py directly. This script exists only for use inside CMake builds.
#
cmake_minimum_required(VERSION 3.5)

if(NOT IDF_PATH OR NOT ESPTOOLPY OR NOT ESPTOOL_ARGS OR NOT ESPTOOL_WORKING_DIR)
    message(FATAL_ERROR "IDF_PATH, ESPTOOLPY, ESPTOOL_ARGS, and ESPTOOL_WORKING_DIR must "
        "be specified on the CMake command line. For direct esptool execution, it is "
        "strongly recommended to run esptool.py directly.")
endif()

# Note: we can't expand these environment variables in the main IDF CMake build,
# because we want to expand them at flashing time not at CMake runtime (so they can change
# without needing a CMake re-run)
set(ESPPORT $ENV{ESPPORT})
if(NOT ESPPORT)
    message("Note: esptool.py will search for a serial port. To specify a port, set the ESPPORT environment variable.")
else()
    set(port_arg "-p ${ESPPORT}")
endif()

set(ESPBAUD $ENV{ESPBAUD})
if(NOT ESPBAUD)
    message("Note: Using default baud rate 460800. To modify, set ESPBAUD environment variable.")
    set(ESPBAUD 460800)
endif()

include("${IDF_PATH}/tools/cmake/utilities.cmake")

set(cmd "${ESPTOOLPY} ${port_arg} -b ${ESPBAUD} ${ESPTOOL_ARGS}")
spaces2list(cmd)

execute_process(COMMAND ${cmd}
    WORKING_DIRECTORY "${ESPTOOL_WORKING_DIR}"
    RESULT_VARIABLE result
    )

if(${result})
    # No way to have CMake silently fail, unfortunately
    message(FATAL_ERROR "esptool.py failed")
endif()
