idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS "../../private_include"
                       PRIV_REQUIRES cmock test_utils esp_timer spi_flash esp_psram
                       WHOLE_ARCHIVE)
