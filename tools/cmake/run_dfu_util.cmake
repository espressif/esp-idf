# A CMake script to run dfu-util from within ninja or make
# or another cmake-based build runner
#
# It is recommended to NOT USE this CMake script directly

cmake_minimum_required(VERSION 3.22)

set(TOOL "dfu-util")
set(CMD "${TOOL}")

if(${ESP_DFU_LIST})
    list(APPEND CMD "--list")
else()
    # The following works even when ESP_DFU_PID is not defined.
    list(APPEND CMD "-d" "303a:${ESP_DFU_PID}")

    if(NOT $ENV{ESP_DFU_PATH} STREQUAL "")
        list(APPEND CMD "--path" $ENV{ESP_DFU_PATH})
    endif()
    list(APPEND CMD "-D" ${ESP_DFU_BIN})
endif()

message("Command list: ${CMD}")
execute_process(COMMAND ${CMD} RESULT_VARIABLE result)

# dfu-util may fail on the first attempt while switching from runtime to DFU mode,
# e.g. with "Lost device after RESET?" on Windows/macOS. Retry once.
if(${result} AND NOT ${ESP_DFU_LIST})
    message(WARNING "${TOOL} failed, retrying once...")
    execute_process(COMMAND ${CMD} RESULT_VARIABLE result)
endif()

if(${result})
    message(FATAL_ERROR "${TOOL} failed")
endif()
