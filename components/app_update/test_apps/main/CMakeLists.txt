idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS "."
                       PRIV_REQUIRES cmock test_utils app_update bootloader_support nvs_flash driver spi_flash
                      WHOLE_ARCHIVE)
