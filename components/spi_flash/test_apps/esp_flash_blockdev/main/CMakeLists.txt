set(srcs "test_app_main.c"
         "test_spi_flash.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity spi_flash esp_driver_spi test_flash_utils
                       WHOLE_ARCHIVE)
