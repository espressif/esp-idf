idf_component_register(SRCS "coap_server_example_main.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES coap_ca.pem coap_server.crt coap_server.key)
