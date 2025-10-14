idf_component_register(
    SRC_DIRS "."
    PRIV_INCLUDE_DIRS "."
    PRIV_REQUIRES
        cmock
        test_utils
        app_update
        bootloader_support
        nvs_flash
        esp_driver_gpio
        spi_flash
        esp_psram
        efuse
    WHOLE_ARCHIVE
)
