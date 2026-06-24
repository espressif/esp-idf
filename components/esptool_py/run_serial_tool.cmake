# A CMake script to run serial tool commands supporting ESPPORT and
# ESPBAUD environment variables from within ninja or make or another
# cmake-based build runner.
#
# It is recommended to NOT USE this CMake script if you have the option of
# running the tool directly. This script exists only for use inside CMake builds.
cmake_minimum_required(VERSION 3.22)

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

# Extract binary file paths from esptool argfile content.
# The argfile contains "0xOFFSET file" pairs; this returns just the file paths.
function(_esptoolpy_extract_flash_files args_content out_var)
    string(REPLACE "\r\n" "\n" args_content "${args_content}")
    separate_arguments(_tokens NATIVE_COMMAND "${args_content}")
    set(_result "")
    set(_expect_file FALSE)
    foreach(_tok IN LISTS _tokens)
        if(_expect_file)
            if(NOT _tok MATCHES "^-")
                list(APPEND _result "${_tok}")
            endif()
            set(_expect_file FALSE)
        elseif(_tok MATCHES "^0x[0-9a-fA-F]+$")
            set(_expect_file TRUE)
        endif()
    endforeach()
    set(${out_var} "${_result}" PARENT_SCOPE)
endfunction()

# Fast-reflashing support (esptool --diff-with and --skip-flashed).
#
# Build-time inputs (passed as -D flags from project_include.cmake):
#   SERIAL_TOOL_IS_WRITE_FLASH  — set to 1 for write-flash targets
#   SERIAL_TOOL_FLASH_ARGS_FILE — argfile name relative to WORKING_DIRECTORY
#
# Runtime inputs (environment variables, can change without reconfiguring CMake):
#   IDF_FLASH_FULL=1            — disable fast reflash (idf.py flash -a/--all)
#   IDF_TRUST_FLASH_CONTENT=1   — skip MD5 verification of unchanged files (idf.py flash -t)

set(_flash_full FALSE)
if(DEFINED ENV{IDF_FLASH_FULL} AND "$ENV{IDF_FLASH_FULL}" STREQUAL "1")
    set(_flash_full TRUE)
endif()

set(_trust_flash_content FALSE)
if(DEFINED ENV{IDF_TRUST_FLASH_CONTENT} AND "$ENV{IDF_TRUST_FLASH_CONTENT}" STREQUAL "1")
    set(_trust_flash_content TRUE)
endif()

# Read the flash files list once (used for both pre-flash --diff-with and post-flash saving).
set(_flash_files "")
if(SERIAL_TOOL_IS_WRITE_FLASH AND SERIAL_TOOL_FLASH_ARGS_FILE
        AND DEFINED WORKING_DIRECTORY AND WORKING_DIRECTORY
        AND EXISTS "${WORKING_DIRECTORY}/${SERIAL_TOOL_FLASH_ARGS_FILE}")
    file(READ "${WORKING_DIRECTORY}/${SERIAL_TOOL_FLASH_ARGS_FILE}" _args_content)
    _esptoolpy_extract_flash_files("${_args_content}" _flash_files)
endif()

# Main purpose of this script: we can't expand these environment variables in the main IDF CMake build,
# because we want to expand them at flashing time not at CMake runtime (so they can change
# without needing a CMake re-run)
set(serial_tool_cmd ${SERIAL_TOOL})

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

list(APPEND serial_tool_cmd $ENV{SERIAL_TOOL_EXTRA_PRE_CMD_ARGS})
list(APPEND serial_tool_cmd ${SERIAL_TOOL_ARGS})

# Append fast-reflashing arguments when flash files are available.
if(_flash_files AND NOT _flash_full)
    # Build --diff-with list: use *_flashed.bin if present, otherwise "skip".
    set(_diff_with_files "")
    set(_have_any FALSE)
    foreach(_file IN LISTS _flash_files)
        string(REGEX REPLACE "\\.bin$" "_flashed.bin" _ref "${_file}")
        if(EXISTS "${WORKING_DIRECTORY}/${_ref}")
            list(APPEND _diff_with_files "${_ref}")
            set(_have_any TRUE)
        else()
            list(APPEND _diff_with_files "skip")
        endif()
    endforeach()

    if(_have_any)
        list(APPEND serial_tool_cmd "--diff-with" ${_diff_with_files})
        if(_trust_flash_content)
            list(APPEND serial_tool_cmd "--trust-flash-content")
        endif()
    else()
        if(_trust_flash_content)
            message(WARNING "No previously flashed binaries found in build directory, "
                    "--trust-flash-content will be ignored.")
        endif()
        list(APPEND serial_tool_cmd "--skip-flashed")
    endif()
endif()

list(APPEND serial_tool_cmd $ENV{SERIAL_TOOL_EXTRA_ARGS})

# Run the serial tool.
set(_execute_extra_args "")
if(${SERIAL_TOOL_SILENT})
    list(APPEND _execute_extra_args OUTPUT_VARIABLE SERIAL_TOOL_OUTPUT_LOG)
endif()

execute_process(COMMAND ${serial_tool_cmd}
    WORKING_DIRECTORY "${WORKING_DIRECTORY}"
    RESULT_VARIABLE result
    ${_execute_extra_args}
)

if(${result})
    message(FATAL_ERROR "${SERIAL_TOOL} failed. \n${SERIAL_TOOL_OUTPUT_LOG}")
endif()

# After successful flash, save copies of all flashed files as *_flashed.bin
# for use with --diff-with on the next reflash.
foreach(_file IN LISTS _flash_files)
    if(EXISTS "${WORKING_DIRECTORY}/${_file}")
        string(REGEX REPLACE "\\.bin$" "_flashed.bin" _dst "${_file}")
        file(COPY_FILE
            "${WORKING_DIRECTORY}/${_file}"
            "${WORKING_DIRECTORY}/${_dst}"
            ONLY_IF_DIFFERENT
            RESULT _copy_result)
        if(NOT _copy_result EQUAL "0")
            message(WARNING "Could not save ${_dst}: ${_copy_result}, "
                "next flash will run without comparison data.")
        endif()
    endif()
endforeach()
