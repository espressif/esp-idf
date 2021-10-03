if(CONFIG_SPI_FLASH_USE_LEGACY_IMPL)
    set(exclude_srcs "test_esp_flash.c" "test_partition_ext.c")
endif()

idf_component_register(SRC_DIRS "."
                       EXCLUDE_SRCS "${exclude_srcs}"
                       PRIV_INCLUDE_DIRS "."
                       PRIV_REQUIRES cmock test_utils spi_flash bootloader_support app_update driver)
