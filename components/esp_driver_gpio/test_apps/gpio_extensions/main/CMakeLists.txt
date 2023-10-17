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

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_gpio
                       WHOLE_ARCHIVE)
