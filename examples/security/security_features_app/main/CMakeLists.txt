idf_component_register(SRCS "security_features_app_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash spi_flash efuse bootloader_support nvs_sec_provider
                    REQUIRED_IDF_TARGETS esp32c3 esp32s3)
