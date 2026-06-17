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

# Cordic related source files
if(CONFIG_SOC_CORDIC_SUPPORTED)
    list(APPEND srcs "cordic_hal.c" "${target}/cordic_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
