set(COMPONENT_SRCS "app_wifi.c"
                   "esp_http_client_example.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")


# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
set(COMPONENT_EMBED_TXTFILES howsmyssl_com_root_cert.pem)

register_component()
