set(COMPONENT_SRCS "wpa2_enterprise_main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")


# Embed CA, certificate & key directly into binary
set(COMPONENT_EMBED_TXTFILES wpa2_ca.pem wpa2_client.crt wpa2_client.key)

register_component()
