set(src_dirs ".")
set(exclude_srcs)

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND src_dirs "esp32")
    if(CONFIG_SPI_FLASH_USE_LEGACY_IMPL)
        set(exclude_srcs "esp32/test_esp_flash.c" "esp32/test_partition_ext.c")
    endif()
endif()

idf_component_register(SRC_DIRS ${src_dirs}
                       EXCLUDE_SRCS ${exclude_srcs}
                       INCLUDE_DIRS "."
                       REQUIRES unity test_utils spi_flash bootloader_support app_update)

