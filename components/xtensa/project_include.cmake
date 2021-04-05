# Check toolchain is configured properly in cmake
if(NOT ( ${CMAKE_SYSTEM_NAME} STREQUAL "Generic" AND ${CMAKE_C_COMPILER} MATCHES xtensa))
    message(FATAL_ERROR "Internal error, toolchain has not been set correctly by project "
        "(or an invalid CMakeCache.txt file has been generated somehow)")
endif()
