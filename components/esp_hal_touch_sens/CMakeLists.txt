idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(includes)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()
# "include" should be behind "${target}/include", because `include_next` has sequence requirement
list(APPEND includes "include")

# TODO(IDF-15989): potentially remove __ULP_BUILDV2 handling from esp_hal_touch_sens
if(__ULP_BUILDV2)
    # ULP touch support includes touch sensor HAL headers from
    # ulp/ulp_riscv/ulp_core/ulp_riscv_touch.c and
    # ulp/lp_core/lp_core/lp_core_touch.c.
    idf_component_register(INCLUDE_DIRS ${includes}
                           REQUIRES soc)
    return()
endif()

# Touch Sensor related source files
if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    # Source files for the legacy touch hal driver
    if(CONFIG_SOC_TOUCH_SENSOR_VERSION LESS 3)
        list(APPEND srcs "${target}/touch_sensor_legacy_hal.c"
                         "touch_sensor_legacy_hal.c"
        )
    endif()
    list(APPEND srcs "touch_sens_hal.c" "${target}/touch_sensor_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
