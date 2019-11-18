if(NOT IDF_PATH)
    message(FATAL_ERROR "IDF_PATH not set.")
endif()
include("${IDF_PATH}/tools/cmake/utilities.cmake")
spaces2list(CMD)

execute_process(COMMAND ${CMD}
    WORKING_DIRECTORY "${WORKING_DIRECTORY}"
    RESULT_VARIABLE result
    )

if(${result})
    # No way to have CMake silently fail, unfortunately
    message(FATAL_ERROR "${TOOL} failed")
endif()
