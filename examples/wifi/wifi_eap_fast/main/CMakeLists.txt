# Embed CA, certificate & key directly into binary
idf_component_register(SRCS "wifi_eap_fast_main.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ca.pem pac_file.pac)
