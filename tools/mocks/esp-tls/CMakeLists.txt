# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building ESP-TLS MOCKS")

idf_component_get_property(original_esp_tls_dir esp-tls COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_esp_tls_dir}"
                                "${original_esp_tls_dir}/esp-tls-crypto"
                   MOCK_HEADER_FILES ${original_esp_tls_dir}/esp_tls.h
                                     ${original_esp_tls_dir}/esp-tls-crypto/esp_tls_crypto.h
                    REQUIRES mbedtls
                   )
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-array-parameter)
