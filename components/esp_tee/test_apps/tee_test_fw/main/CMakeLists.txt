idf_build_get_property(idf_path IDF_PATH)

set(priv_requires bootloader_support driver esp_tee esp_timer mbedtls spi_flash)
# Test FW related
list(APPEND priv_requires cmock json nvs_flash test_utils unity)
# TEE related
list(APPEND priv_requires tee_sec_storage tee_attestation tee_ota_ops test_sec_srv)

set(srcs "app_main.c")

list(APPEND srcs "test_esp_tee_ctx_switch.c"
                 "test_esp_tee_interrupt.c"
                 "test_esp_tee_panic.c"
                 "test_esp_tee_sec_stg.c"
                 "test_esp_tee_ota.c"
                 "test_esp_tee_flash_prot.c")

if(CONFIG_SECURE_TEE_ATTESTATION)
    list(APPEND srcs "test_esp_tee_att.c")
endif()

set(mbedtls_test_srcs_dir "${idf_path}/components/mbedtls/test_apps/main")

# AES
list(APPEND srcs "${mbedtls_test_srcs_dir}/test_aes.c"
                 "${mbedtls_test_srcs_dir}/test_aes_gcm.c"
                 "${mbedtls_test_srcs_dir}/test_aes_perf.c")
# SHA
list(APPEND srcs "${mbedtls_test_srcs_dir}/test_mbedtls_sha.c"
                 "${mbedtls_test_srcs_dir}/test_sha.c"
                 "${mbedtls_test_srcs_dir}/test_sha_perf.c")

# Mixed
list(APPEND srcs "${mbedtls_test_srcs_dir}/test_aes_sha_parallel.c")

# ECC
list(APPEND srcs "${mbedtls_test_srcs_dir}/test_ecp.c")

# Utility
list(APPEND srcs "${mbedtls_test_srcs_dir}/test_apb_dport_access.c"
                 "${mbedtls_test_srcs_dir}/test_mbedtls_utils.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE)
