idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES cmock test_utils nvs_flash bootloader_support spi_flash
                    EMBED_TXTFILES encryption_keys.bin partition_encrypted.bin sample.bin)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

if(CONFIG_NVS_ENCRYPTION)
    target_link_libraries(${COMPONENT_LIB} PUBLIC idf::mbedtls)
endif()
