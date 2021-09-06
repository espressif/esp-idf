# Embed CA, certificate & key directly into binary
idf_component_register(SRCS "wifi_enterprise_main.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ca.pem client.crt client.key)
