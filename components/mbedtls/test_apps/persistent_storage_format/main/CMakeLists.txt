idf_component_register(
                    SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES efuse cmock test_utils mbedtls esp_timer
                                  unity spi_flash esp_security nvs_flash
                    WHOLE_ARCHIVE)

# The RSA-DS opaque driver is the unit under test for these format
# stability checks; the linker wraps below let the sign path complete
# without a real eFuse HMAC key on the runner. Mirrors mbedtls_ut.
target_link_options(
    ${COMPONENT_LIB} INTERFACE
    "-Wl,--wrap=esp_ds_finish_sign,--wrap=esp_ds_start_sign,--wrap=esp_efuse_get_key_purpose"
    )
