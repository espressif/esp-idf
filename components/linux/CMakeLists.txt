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

if(${IDF_TARGET} STREQUAL "linux")
    find_library(LIB_BSD bsd)
    if(LIB_BSD)
        target_link_libraries(${COMPONENT_LIB} PRIVATE ${LIB_BSD})
    elseif(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        message(WARNING "Missing LIBBSD library. Install libbsd-dev package and/or check linker directories.")
    endif()
endif()
