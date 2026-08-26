# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
idf_component_register(SRCS "https_mbedtls_example_main.c"
                    PRIV_REQUIRES esp_event nvs_flash esp_netif mbedtls
                    INCLUDE_DIRS ".")
