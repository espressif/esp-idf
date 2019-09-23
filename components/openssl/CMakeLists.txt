idf_component_register(SRCS "library/ssl_cert.c"
                            "library/ssl_lib.c"
                            "library/ssl_methods.c"
                            "library/ssl_pkey.c"
                            "library/ssl_stack.c"
                            "library/ssl_x509.c"
                            "platform/ssl_pm.c"
                            "platform/ssl_port.c"
                    REQUIRES mbedtls
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS include/internal include/platform include/openssl)
