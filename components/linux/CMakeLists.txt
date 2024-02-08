idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "linux")
    return()
endif()

set(includes "include")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND srcs getrandom.c assert_func.c)
else()
    list(APPEND srcs fls.c)
    list(APPEND includes "linux_include")
endif()

idf_component_register(INCLUDE_DIRS ${includes}
                       REQUIRED_IDF_TARGETS linux
                       SRCS ${srcs})
