idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(includes "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

# I2S related source files
if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs "i2s_hal.c" "${target}/i2s_periph.c")
endif()

if(CONFIG_SOC_LP_I2S_SUPPORTED)
    list(APPEND srcs "lp_i2s_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
