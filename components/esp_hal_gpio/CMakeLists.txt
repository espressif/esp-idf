idf_build_get_property(target IDF_TARGET)

set(srcs)
set(includes "include" "${target}/include")

if(CONFIG_SOC_GPIO_PORT GREATER 0)
    list(APPEND srcs "gpio_hal.c")
endif()

if(CONFIG_SOC_RTCIO_PIN_COUNT GREATER 0)
    list(APPEND srcs "rtc_io_hal.c" "${target}/rtc_io_periph.c")
endif()

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "${target}/dedic_gpio_periph.c")
endif()

if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "sdm_hal.c" "${target}/sdm_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES soc hal)
