idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "linux")
    return()
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND srcs getrandom.c assert_func.c)
endif()

idf_component_register(INCLUDE_DIRS include
                       REQUIRED_IDF_TARGETS linux
                       SRCS ${srcs})
