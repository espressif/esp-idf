idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(includes "include" "interface" "${target}/include")

set(srcs)

if(CONFIG_SOC_ADC_SUPPORTED)
    list(APPEND srcs
        "adc_oneshot.c"
        "adc_common.c"
        "adc_cali.c"
        "adc_cali_curve_fitting.c"
    )

    if(CONFIG_SOC_ADC_DMA_SUPPORTED)
        list(APPEND srcs "adc_continuous.c")
        if(CONFIG_SOC_ADC_MONITOR_SUPPORTED)
            list(APPEND srcs "adc_monitor.c")
        endif()
        if(CONFIG_SOC_GDMA_SUPPORTED)
            list(APPEND srcs "gdma/adc_dma.c")
        elseif(${target} STREQUAL "esp32")
            list(APPEND srcs "esp32/adc_dma.c")
        elseif(${target} STREQUAL "esp32s2")
            list(APPEND srcs "esp32s2/adc_dma.c")
        endif()
    endif()
endif()

if(CONFIG_SOC_ADC_DIG_IIR_FILTER_SUPPORTED)
    list(APPEND srcs "adc_filter.c")
endif()

# line fitting scheme
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/adc_cali_line_fitting.c")
    list(APPEND srcs "${target}/adc_cali_line_fitting.c")
endif()

# curve fitting scheme coefficients
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/curve_fitting_coefficients.c")
    list(APPEND srcs "${target}/curve_fitting_coefficients.c")
endif()

set(extra_requires)
idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "esp32")
    list(APPEND extra_requires esp_driver_i2s)
endif()
if(${target} STREQUAL "esp32s2")
    list(APPEND extra_requires esp_driver_spi)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES esp_driver_gpio esp_driver_dma efuse esp_pm esp_ringbuf esp_mm ${extra_requires}
                       REQUIRES esp_hal_ana_conv
                       LDFRAGMENTS linker.lf)
