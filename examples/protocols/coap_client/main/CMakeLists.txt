# Embed CA, certificate & key directly into binary
idf_component_register(SRCS "coap_client_example_main.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES coap_ca.pem coap_client.crt coap_client.key)
