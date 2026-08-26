idf_component_register(SRCS "esp_ot_br.c"
                       PRIV_REQUIRES esp_psram openthread esp_coex esp_wifi nvs_flash
                       INCLUDE_DIRS ".")
