idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES unity test_utils mbedtls libsodium
                    EMBED_TXTFILES server_cert_chain.pem prvtkey.pem server_cert_bundle)


idf_component_get_property(mbedtls mbedtls COMPONENT_LIB)
target_compile_definitions(${mbedtls} INTERFACE "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedtls PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedcrypto PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedx509 PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
