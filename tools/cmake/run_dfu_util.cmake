# A CMake script to run dfu-util from within ninja or make
# or another cmake-based build runner
#
# It is recommended to NOT USE this CMake script directly

cmake_minimum_required(VERSION 3.16)

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

if(${result})
    message(FATAL_ERROR "${TOOL} failed")
endif()
