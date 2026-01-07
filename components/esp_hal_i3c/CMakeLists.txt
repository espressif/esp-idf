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

# I3C related source files
if(CONFIG_SOC_I3C_MASTER_SUPPORTED)
    list(APPEND srcs "i3c_master_hal.c" "${target}/i3c_master_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
