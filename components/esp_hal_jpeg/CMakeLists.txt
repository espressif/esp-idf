idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(includes)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()
list(APPEND includes "include")

set(srcs)

# JPEG related source files
if(CONFIG_SOC_JPEG_CODEC_SUPPORTED)
    list(APPEND srcs "jpeg_hal.c" "${target}/jpeg_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
