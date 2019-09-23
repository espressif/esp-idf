idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES unity test_utils mbedtls)

idf_component_get_property(mbedtls mbedtls COMPONENT_LIB)
target_compile_definitions(${mbedtls} INTERFACE "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedtls PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedcrypto PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedx509 PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
