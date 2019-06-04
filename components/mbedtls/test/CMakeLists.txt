set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES unity test_utils mbedtls)

register_component()

idf_component_get_property(mbedtls mbedtls COMPONENT_LIB)
target_compile_definitions(${mbedtls} PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedtls PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedcrypto PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
target_compile_definitions(mbedx509 PUBLIC "-DMBEDTLS_DEPRECATED_WARNING")
