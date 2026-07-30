idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

# Public ADC and temperature sensor HAL headers include hal/regi2c_ctrl.h on
# several targets, so CMake v2 builds need esp_hal_regi2c explicitly.
set(requires soc hal esp_hal_dma esp_hal_regi2c)
if(${target} STREQUAL "esp32")
    list(APPEND requires esp_hal_i2s)
endif()

set(srcs)
set(includes "include")
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

# TODO(IDF-15985): potentially remove __ULP_BUILDV2 handling from esp_hal_ana_conv
if(__ULP_BUILDV2)
    # ULP ADC code includes ADC HAL headers from
    # ulp/ulp_riscv/ulp_core/ulp_riscv_adc.c and
    # ulp/lp_core/shared/include/ulp_lp_core_lp_adc_shared.h. Some target
    # ADC/temperature sensor headers include hal/regi2c_ctrl.h.
    idf_component_register(INCLUDE_DIRS ${includes}
                           REQUIRES ${requires})
    return()
endif()

# ADC related source files
if(CONFIG_SOC_ADC_SUPPORTED)
    list(APPEND srcs "${target}/adc_periph.c" "adc_hal_common.c" "adc_oneshot_hal.c")
    if(CONFIG_SOC_ADC_DMA_SUPPORTED)
        list(APPEND srcs "adc_hal.c")
    endif()
endif()

# DAC related source files
if(CONFIG_SOC_DAC_SUPPORTED)
    list(APPEND srcs "${target}/dac_periph.c")
endif()

# Temperature sensor related source files
if(CONFIG_SOC_TEMP_SENSOR_SUPPORTED)
    list(APPEND srcs "temperature_sensor_hal.c" "${target}/temperature_sensor_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES ${requires})
