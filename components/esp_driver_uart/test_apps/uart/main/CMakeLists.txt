set(srcs "test_app_main.c"
         "test_uart.c")

if(CONFIG_PM_ENABLE)
    list(APPEND srcs "test_uart_auto_lightsleep.c")
endif()

# Only if the target supports uart retention and the sdkconfig.ci.xxx contains at least PM_ENABLE=y
if(CONFIG_SOC_UART_SUPPORT_SLEEP_RETENTION AND CONFIG_PM_ENABLE)
    list(APPEND srcs "test_uart_retention.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    REQUIRES esp_driver_uart unity esp_psram test_utils esp_driver_gpio esp_pm
    PRIV_INCLUDE_DIRS .
    WHOLE_ARCHIVE
)
