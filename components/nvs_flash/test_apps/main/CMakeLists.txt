idf_component_register(SRC_DIRS "."
                       PRIV_REQUIRES cmock test_utils nvs_flash nvs_sec_provider
                                     bootloader_support spi_flash
                       EMBED_TXTFILES encryption_keys.bin partition_encrypted.bin
                                      partition_encrypted_hmac.bin sample.bin
                       WHOLE_ARCHIVE)

if(CONFIG_NVS_ENCRYPTION OR CONFIG_SOC_HMAC_SUPPORTED)
    target_link_libraries(${COMPONENT_LIB} PUBLIC idf::mbedtls)
endif()
