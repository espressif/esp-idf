# A CMake script to run dfu-util from within ninja or make
# or another cmake-based build runner
#
# It is recommended to NOT USE this CMake script directly

cmake_minimum_required(VERSION 3.16)

set(UF2_CMD "${UF2_CMD}")
set(UF2_ARGS "${UF2_ARGS}")

if(NOT UF2_CMD)
    message(FATAL_ERROR "UF2_CMD must be specified on the CMake command line.")
endif()

if(NOT UF2_ARGS)
    message(FATAL_ERROR "UF2_ARGS must be specified on the CMake command line.")
endif()

set(uf2_cmd_with_args ${UF2_CMD})
list(APPEND uf2_cmd_with_args ${UF2_ARGS})

execute_process(COMMAND ${uf2_cmd_with_args}
    RESULT_VARIABLE result)

if(${result})
    message(FATAL_ERROR "${UF2_CMD} failed")
endif()
