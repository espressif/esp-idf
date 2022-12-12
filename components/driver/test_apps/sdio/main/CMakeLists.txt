set(srcs
    "test_app_main.c"
    "test_sdio.c"
    "test_sdmmc_sdspi_init.cpp"
)

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES test_utils test_driver_utils driver esp_timer sdmmc esp_serial_slave_link
    WHOLE_ARCHIVE
)
