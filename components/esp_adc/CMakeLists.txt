idf_build_get_property(target IDF_TARGET)

set(includes "include" "interface" "${target}/include" "deprecated/include")

set(srcs "adc_cali.c"
         "adc_cali_curve_fitting.c"
         "adc_oneshot.c"
         "adc_common.c"
         "deprecated/esp_adc_cal_common_legacy.c")

if(CONFIG_SOC_ADC_DMA_SUPPORTED)
    list(APPEND srcs "adc_continuous.c")
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

# legacy calibration driver
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deprecated/${target}/esp_adc_cal_legacy.c")
    list(APPEND srcs "deprecated/${target}/esp_adc_cal_legacy.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES driver efuse
                       LDFRAGMENTS linker.lf)
