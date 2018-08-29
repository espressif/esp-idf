set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS include/internal include/platform include/openssl)
set(COMPONENT_SRCDIRS library platform)

set(COMPONENT_REQUIRES mbedtls)

register_component()
