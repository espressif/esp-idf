set(TEST_CRTS "crts/server_cert_chain.pem"
              "crts/prvtkey.pem"
              "crts/server_cert_bundle"
              "crts/bad_md_crt.pem"
              "crts/wrong_sig_crt_esp32_com.pem"
              "crts/correct_sig_crt_esp32_com.pem"
              "crts/ecdsa_cert_bundle"
              "crts/ecdsa_correct_sig_crt.pem"
              "crts/ecdsa_wrong_sig_crt.pem")

idf_component_register(
                    SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "." "../include"
                    PRIV_REQUIRES efuse cmock test_utils mbedtls esp_timer unity spi_flash esp_psram esp_security
                    EMBED_TXTFILES ${TEST_CRTS}
                    WHOLE_ARCHIVE)

idf_component_get_property(mbedtls mbedtls COMPONENT_LIB)
target_compile_definitions(${mbedtls} INTERFACE "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedtls PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedx509 PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")

target_link_options(
    ${COMPONENT_LIB} INTERFACE
    "-Wl,--wrap=esp_ds_finish_sign,--wrap=esp_ds_start_sign,--wrap=esp_efuse_get_key_purpose"
    )
