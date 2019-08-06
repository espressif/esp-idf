set(src_dirs ".")
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND src_dirs "esp32")
endif()
idf_component_register(SRC_DIRS ${src_dirs}
                    INCLUDE_DIRS "."
                    REQUIRES unity test_utils spi_flash bootloader_support app_update)