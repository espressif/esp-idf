# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
set(requires esp-tls nvs_flash esp_event esp_netif esp_http_client)
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs protocol_examples_common)
endif()
idf_component_register(SRCS "esp_http_client_example.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${requires}
                    EMBED_TXTFILES howsmyssl_com_root_cert.pem
                    postman_root_cert.pem)
