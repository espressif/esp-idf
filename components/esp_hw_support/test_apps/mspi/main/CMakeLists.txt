set(srcs
    "test_app_main.c"
    "test_flash_psram.c"
    "test_mspi.c"
)

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_timer spi_flash esp_partition
                       WHOLE_ARCHIVE)
