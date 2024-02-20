idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS "."
                       PRIV_REQUIRES cmock test_utils spi_flash bootloader_support app_update
                                     driver esp_timer)
