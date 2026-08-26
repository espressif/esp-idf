set(srcs "test_app_main.c")

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "test_dedicated_gpio.c")
endif()

if(CONFIG_SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER OR (CONFIG_SOC_GPIO_FLEX_GLITCH_FILTER_NUM GREATER 0))
    list(APPEND srcs "test_gpio_filter.c")
endif()

if(CONFIG_SOC_GPIO_SUPPORT_PIN_HYS_FILTER)
    list(APPEND srcs "test_hysteresis.c")
endif()

if(CONFIG_SOC_GPIO_SUPPORT_ETM)
    list(APPEND srcs "test_gpio_etm.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_gpio esp_psram
                       WHOLE_ARCHIVE)

message(STATUS "Checking gpio_ext registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "gpio_ext"
                            HAL_MODULES "gpio_etm" "gpio_glitch_filter")
