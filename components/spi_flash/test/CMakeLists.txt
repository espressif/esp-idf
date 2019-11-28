if(CONFIG_SPI_FLASH_USE_LEGACY_IMPL)
    set(exclude_srcs "test_esp_flash.c" "test_partition_ext.c")
endif()

idf_component_register(SRC_DIRS "."
                       EXCLUDE_SRCS "${exclude_srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES unity test_utils spi_flash bootloader_support app_update)

