idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    idf_component_register()
    if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        find_library(LIBRT rt)
        target_link_libraries(${COMPONENT_LIB} INTERFACE ${LIBRT})
    endif()
    return()
endif()

idf_component_register(SRCS "FreeRTOS_POSIX_mqueue.c" "FreeRTOS_POSIX_utils.c"
                    PRIV_INCLUDE_DIRS "private_include"
                    INCLUDE_DIRS "include")
