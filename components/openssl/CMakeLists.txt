set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS include/internal include/platform include/openssl)
set(COMPONENT_SRCS "library/ssl_cert.c"
                   "library/ssl_lib.c"
                   "library/ssl_methods.c"
                   "library/ssl_pkey.c"
                   "library/ssl_stack.c"
                   "library/ssl_x509.c"
                   "platform/ssl_pm.c"
                   "platform/ssl_port.c")

set(COMPONENT_REQUIRES mbedtls)

register_component()
