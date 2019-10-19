idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES unity test_utils spi_flash bootloader_support app_update)

if(CONFIG_SPI_FLASH_USE_LEGACY_IMPL)
    set(COMPONENT_SRCEXCLUDE "test_esp_flash.c" "test_partition_ext.c")
endif()
