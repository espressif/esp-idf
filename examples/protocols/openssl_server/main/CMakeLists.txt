# Embed the certificate & key data directly in the built binary
idf_component_register(SRCS "openssl_server_example_main.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES cacert.pem prvtkey.pem)