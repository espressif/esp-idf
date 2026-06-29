idf_build_get_property(target IDF_TARGET)

set(srcs)
set(includes "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

# ULP GPIO helpers include GPIO/RTC IO HAL headers from
# ulp/ulp_riscv/ulp_core/include/ulp_riscv_gpio.h and
# ulp/lp_core/lp_core/include/ulp_lp_core_gpio.h.
# TODO(IDF-15948): potentially remove __ULP_BUILD source exclusions from esp_hal_gpio
if(CONFIG_SOC_GPIO_PORT GREATER 0 AND NOT __ULP_BUILD)
    list(APPEND srcs "gpio_hal.c")
endif()

if(CONFIG_SOC_RTCIO_PIN_COUNT GREATER 0 AND NOT __ULP_BUILD)
    list(APPEND srcs "rtc_io_hal.c" "${target}/rtc_io_periph.c")
endif()

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED AND NOT __ULP_BUILD)
    list(APPEND srcs "${target}/dedic_gpio_periph.c")
endif()

if(CONFIG_SOC_SDM_SUPPORTED AND NOT __ULP_BUILD)
    list(APPEND srcs "sdm_hal.c" "${target}/sdm_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES soc hal)
