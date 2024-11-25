idf_component_register(SRCS main.c
                       PRIV_REQUIRES esp_event nvs_flash esp_wifi esp_http_server dns_server
                       EMBED_FILES root.html)
