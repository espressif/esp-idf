set(COMPONENT_SRCS "openssl_server_example_main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")


# Embed the certificate & key data directly in the built binary
set(COMPONENT_EMBED_TXTFILES cacert.pem prvtkey.pem)

register_component()
