set(srcs "test_app_main.c"
         "test_gpio.c")

if(CONFIG_SOC_RTCIO_PIN_COUNT GREATER 0)
    list(APPEND srcs "test_rtcio.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_gpio spi_flash esp_psram
                       WHOLE_ARCHIVE)
