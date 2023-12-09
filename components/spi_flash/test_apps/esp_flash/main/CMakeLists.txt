set(srcs "test_app_main.c"
         "test_spi_flash.c"
         "test_esp_flash_drv.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity test_utils spi_flash esp_timer driver bootloader_support
                       WHOLE_ARCHIVE)
