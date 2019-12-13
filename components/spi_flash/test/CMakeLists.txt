idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    EXCLUDE_SRCS "test_esp_flash.c" "test_partition_ext.c"
                    REQUIRES unity test_utils spi_flash bootloader_support app_update)
