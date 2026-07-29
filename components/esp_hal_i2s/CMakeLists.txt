idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(includes "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

# TODO(IDF-15988): potentially remove __ULP_BUILDV2 handling from esp_hal_i2s
if(__ULP_BUILDV2)
    # LP-core VAD shared code includes LP I2S HAL headers from
    # ulp/lp_core/shared/ulp_lp_core_lp_vad_shared.c.
    idf_component_register(INCLUDE_DIRS ${includes}
                           REQUIRES soc hal)
    return()
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
