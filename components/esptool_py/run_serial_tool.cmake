# A CMake script to run serial tool commands supporting ESPPORT and
# ESPBAUD environment variables from within ninja or make or another
# cmake-based build runner.
#
# It is recommended to NOT USE this CMake script if you have the option of
# running the tool directly. This script exists only for use inside CMake builds.
cmake_minimum_required(VERSION 3.16)

if(NOT IDF_PATH)
    message(FATAL_ERROR "IDF_PATH not set.")
endif()

if(NOT SERIAL_TOOL OR NOT SERIAL_TOOL_ARGS)
    message(FATAL_ERROR "SERIAL_TOOL and SERIAL_TOOL_ARGS must "
        "be specified on the CMake command line. For direct execution, it is "
        "strongly recommended to run ${SERIAL_TOOL} directly.")
endif()

# Propagate the IDF_ENV_FPGA to esptool, for Espressif internal use only
if(DEFINED ENV{IDF_ENV_FPGA})
    set(ENV{ESPTOOL_ENV_FPGA} 1)
    message("Note: IDF_ENV_FPGA is set, propagating to esptool with ESPTOOL_ENV_FPGA = 1")
endif()

set(serial_tool_cmd ${SERIAL_TOOL})

# Main purpose of this script: we can't expand these environment variables in the main IDF CMake build,
# because we want to expand them at flashing time not at CMake runtime (so they can change
# without needing a CMake re-run)
set(ESPPORT $ENV{ESPPORT})
if(NOT ESPPORT)
    message("Note: ${SERIAL_TOOL} will search for a serial port. "
            "To specify a port, set the ESPPORT environment variable.")
else()
    list(APPEND serial_tool_cmd -p ${ESPPORT})
endif()

set(ESPBAUD $ENV{ESPBAUD})
if(NOT ESPBAUD)
    message("Note: ${SERIAL_TOOL} will attempt to set baud rate automatically. "
            "To specify a baud rate, set the ESPBAUD environment variable.")
else()
    list(APPEND serial_tool_cmd -b ${ESPBAUD})
endif()

# SERIAL_TOOL_ARGS is defined during the first cmake run
# EXTRA_ARGS and EXTRA_PRE_CMD_ARGS are used for additional arguments from the command line during run-time
list(APPEND serial_tool_cmd $ENV{SERIAL_TOOL_EXTRA_PRE_CMD_ARGS})
list(APPEND serial_tool_cmd ${SERIAL_TOOL_ARGS})
list(APPEND serial_tool_cmd $ENV{SERIAL_TOOL_EXTRA_ARGS})

if(${SERIAL_TOOL_SILENT})
    execute_process(COMMAND ${serial_tool_cmd}
        WORKING_DIRECTORY "${WORKING_DIRECTORY}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE SERIAL_TOOL_OUTPUT_LOG
    )
else()
    execute_process(COMMAND ${serial_tool_cmd}
        WORKING_DIRECTORY "${WORKING_DIRECTORY}"
        RESULT_VARIABLE result
    )
endif()

if(${result})
    # No way to have CMake silently fail, unfortunately
    message(FATAL_ERROR "${SERIAL_TOOL} failed. \n${SERIAL_TOOL_OUTPUT_LOG}")
endif()
