idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(requires soc hal esp_hal_dma)
if(${target} STREQUAL "esp32")
    list(APPEND requires esp_hal_i2s)
endif()

set(srcs)
set(includes "include")
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
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
