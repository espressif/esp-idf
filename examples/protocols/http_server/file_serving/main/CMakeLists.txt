idf_component_register(SRCS "main.c" "file_server.c" "mount.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES fatfs esp_event vfs esp_netif nvs_flash spiffs esp_http_server
                    EMBED_FILES "favicon.ico" "upload_script.html")
