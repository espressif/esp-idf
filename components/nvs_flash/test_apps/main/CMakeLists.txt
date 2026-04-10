idf_component_register(SRC_DIRS "."
                       PRIV_REQUIRES cmock test_utils nvs_flash nvs_sec_provider
                                     esp_psram efuse
                       EMBED_TXTFILES encryption_keys.bin partition_encrypted.bin
                                      partition_encrypted_hmac.bin sample.bin
                       WHOLE_ARCHIVE)

if(CONFIG_NVS_ENCRYPTION OR CONFIG_SOC_HMAC_SUPPORTED)
    target_link_libraries(${COMPONENT_LIB} PUBLIC idf::mbedtls)
    # Add private_include directory from nvs_flash for accessing internal headers like nvs_xts_aes.h in the test app
    target_include_directories(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../private_include")
endif()
