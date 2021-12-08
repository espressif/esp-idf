idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES cmock test_utils nvs_flash bootloader_support
                    EMBED_TXTFILES encryption_keys.bin partition_encrypted.bin sample.bin)

if(CONFIG_NVS_ENCRYPTION)
    target_link_libraries(${COMPONENT_LIB} PUBLIC idf::mbedtls)
endif()
