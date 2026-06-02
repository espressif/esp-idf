
set(srcs
    "test_app_main.c"
    "test_spi_slave.c"
    "test_spi_slave_queue.c"
)


# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    # esp_psram is required for CONFIG_SPIRAM, used by the release configs
    PRIV_REQUIRES esp_driver_spi esp_driver_gpio esp_mspi esp_psram
    WHOLE_ARCHIVE
)
